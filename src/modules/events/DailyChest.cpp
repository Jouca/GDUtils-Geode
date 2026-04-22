#include "DailyChest.hpp"
#include <arc/prelude.hpp>
#include "../include/RateEvent.hpp"
using namespace asp::time;

#if defined(GEODE_IS_WINDOWS)
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <time.h>
#endif
inline double rob_seconds(int remaining) {
    if (remaining == 0) return 0;
#ifdef GEODE_IS_WINDOWS
	__timeb64 current;
    _ftime64_s(&current);
	// nice robert, i dunno if the bitmasking was intentional
	auto result = (((double)((current.time & 0xFFFFF) * 1000 + current.millitm)) / 1000.0);
#else
	struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    auto result = ((double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0);
#endif
	if (remaining < 0) return result;
	return remaining - result;
}
inline double rob_seconds() {
	return rob_seconds(-1);
}
enum class ChestKind {
    Small,
    Large
};
struct ChestMsg {
    enum class Type {
        Refresh,
        Opened,
        Stop
    } type;

    uint32_t smallTime = 0;
    uint32_t largeTime = 0;
    ChestKind opened{};
};
static GJRewardItem* time1 = nullptr;
static GJRewardItem* time2 = nullptr;
static std::optional<arc::mpsc::Sender<ChestMsg>> g_chestTx;

void DailyChest::getRewards() {
    auto glm = GameLevelManager::sharedState();
    if (glm) {
        glm->m_GJRewardDelegate = this;
        glm->getGJRewards(0);
    }
};

#include <Geode/utils/general.hpp>
void DailyChest::rewardsStatusFinished(int p0) {
    GameStatsManager* gsm;
    // Check if rewards are loaded
    gsm = GameStatsManager::sharedState();
    if (gsm->m_rewardItems->count() == 0) {
        log::debug("[DailyChest] no reward items, aborting");
        return;
    }

    // Get the rewards
    time1 = typeinfo_cast<GJRewardItem*>(gsm->m_rewardItems->objectForKey(0x1));
    time2 = typeinfo_cast<GJRewardItem*>(gsm->m_rewardItems->objectForKey(0x2));

    uint32_t t1 = 0;
    uint32_t t2 = 0;
    if (time1) {
        t1 = rob_seconds(time1->m_timeRemaining);
    }
    if (time2) {
        t2 = rob_seconds(time2->m_timeRemaining);
    }

    log::debug("[DailyChest] parsed timers: small={}s large={}s", t1, t2);

    if (g_chestTx) {
        auto data = ChestMsg {
            .type = ChestMsg::Type::Refresh,
            .smallTime = t1,
            .largeTime = t2
        };
        (void)g_chestTx->trySend(std::move(data));
    }
};

void DailyChest::rewardsStatusFailed() {
    log::error("[DailyChest] Failed to get rewards");
};

static void refreshRewards() {
    auto dailyChest = new DailyChest();
    dailyChest->getRewards();

    uint32_t t1 = 0;
    uint32_t t2 = 0;
    if (time1) t1 = rob_seconds(time1->m_timeRemaining);
    if (time2) t2 = rob_seconds(time2->m_timeRemaining);
}

void giveChestNotify(bool large) {
    Loader::get()->queueInMainThread([large]() {
        EventData data;
        data.sprite = "GJ_square04.png";
        data.level_name = " ";
        data.level_creator = " ";
        if (large) {
            data.type = EventType::largeChest;
            data.title = "Big Daily Chest available!";
        } else {
            data.type = EventType::smallChest;
            data.title = "Small Daily Chest available!";
        }
        GDUtils::Events::RateEvent::emit(data);
    });
}

// TODO: test RewardUnlockLayer::playRewardEffect to fix mod menus from skipping step2
#include <Geode/modify/RewardUnlockLayer.hpp>
class $modify(RULHook, RewardUnlockLayer) {
    void playRewardEffect() {
        int tag = m_chestType;
        if (g_chestTx) {
            if (tag == 1) {
                (void)g_chestTx->trySend({
                    .type = ChestMsg::Type::Opened,
                    .opened = ChestKind::Small
                });
            } else if (tag == 2) {
                (void)g_chestTx->trySend({
                    .type = ChestMsg::Type::Opened,
                    .opened = ChestKind::Large
                });
            }
        }
        refreshRewards();
        RewardUnlockLayer::playRewardEffect();
    }
};

static void toggleHook(bool value) {
    for (auto& hook : geode::Mod::get()->getHooks()) {
        if (hook->getDisplayName() == "RewardUnlockLayer::playRewardEffect") {
            (void)(value ? hook->enable() : hook->disable());
        }
    }
}
$on_game(Loaded) {
    if (!Mod::get()->getSettingValue<bool>("largeChest") && !Mod::get()->getSettingValue<bool>("smallChest")) {
        toggleHook(false);
        return;
    }
    auto [tx, rx] = arc::mpsc::channel<ChestMsg>();
    g_chestTx = std::move(tx);
    // wait 5 seconds before
    async::runtime().spawn([rx = std::move(rx)]() mutable -> arc::Future<> {
        uint32_t smallRemaining = 0;
        uint32_t largeRemaining = 0;
        bool smallAwait = false;
        bool largeAwait = false;
        bool smallNotified = false;
        bool largeNotified = false;
        arc::CancellationToken cancel;
        bool running = true;

        while (running) {
            uint32_t nextWake = UINT32_MAX;
            if (smallAwait) nextWake = std::min(nextWake, smallRemaining);
            if (largeAwait) nextWake = std::min(nextWake, largeRemaining);

            log::debug(
                "[DailyChest] loop: small={} ({}s), large={} ({}s)",
                smallAwait, smallRemaining,
                largeAwait, largeRemaining
            );

            co_await arc::select(
                arc::selectee(
                    cancel.waitCancelled(),
                    [&] { running = false; }
                ),
                arc::selectee(
                    (nextWake != UINT32_MAX)
                    ? arc::sleepFor(Duration::fromSecs(nextWake))
                    : arc::sleepFor(Duration::fromSecs(30)),
                    [&] {
                        log::debug("[DailyChest] sleep -> called! (after {}s)", nextWake);
                        if (smallAwait && smallRemaining == nextWake) {
                            log::debug("[DailyChest] sleep -> small chest notify");
                            smallAwait = false;
                            smallNotified = true;
                            giveChestNotify(false);
                        }
                        if (largeAwait && largeRemaining == nextWake) {
                            log::debug("[DailyChest] sleep -> large chest notify");
                            largeAwait = false;
                            largeNotified = true;
                            giveChestNotify(true);
                        }
                    }
                ),
                arc::selectee(rx.recv(), [&](auto req) {
                    if (req) {
                        log::debug("[DailyChest] rx -> called!");
                        ChestMsg msg = std::move(req).unwrap();
                        switch (msg.type) {
                            case ChestMsg::Type::Opened:
                                log::debug("[DailyChest] rx -> chest opened: {}", msg.opened == ChestKind::Small ? "small" : "large");
                                // smallRemaining = msg.smallTime;
                                // largeRemaining = msg.largeTime;
                                // smallAwait = smallRemaining > 0;
                                // largeAwait = largeRemaining > 0;
                                if (msg.opened == ChestKind::Small) {
                                    smallAwait = false;
                                    smallNotified = false;
                                } else {
                                    largeAwait = false;
                                    largeNotified = false;
                                }
                                break;
                            case ChestMsg::Type::Refresh:
                                log::debug(
                                    "[DailyChest] rx -> refresh: small={}s large={}s",
                                    msg.smallTime, msg.largeTime
                                );
                                if (msg.smallTime == 0) {
                                    if (!smallAwait && !smallNotified) {
                                        log::debug("[DailyChest] rx -> small chest notify");
                                        giveChestNotify(false);
                                    }
                                    smallAwait = false;
                                    smallNotified = true;
                                    smallRemaining = 0;
                                } else {
                                    smallRemaining = msg.smallTime;
                                    smallAwait = true;
                                }
                                if (msg.largeTime == 0) {
                                    if (!largeAwait && !largeNotified) {
                                        log::debug("[DailyChest] rx -> large chest notify");
                                        giveChestNotify(true);
                                    }
                                    largeAwait = false;
                                    largeNotified = true;
                                    largeRemaining = 0;
                                } else {
                                    largeRemaining = msg.largeTime;
                                    largeAwait = true;
                                }
                                break;
                            case ChestMsg::Type::Stop:
                                log::debug("[DailyChest] rx -> stop");
                                cancel.cancel();
                                return;
                        }
                    }
                })
            );
        }
    });
    listenForSettingChanges<bool>("smallChest", [](bool value) {
        if (!value && !Mod::get()->getSettingValue<bool>("largeChest")) {
            if (g_chestTx) {
                toggleHook(false);
                (void)g_chestTx->trySend({ ChestMsg::Type::Stop });
            }
        }
    });
    listenForSettingChanges<bool>("largeChest", [](bool value) {
        if (!value && !Mod::get()->getSettingValue<bool>("smallChest")) {
            if (g_chestTx) {
                toggleHook(false);
                (void)g_chestTx->trySend({ ChestMsg::Type::Stop });
            }
        }
    });
    refreshRewards();
}
