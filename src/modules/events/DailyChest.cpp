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

static std::optional<arc::mpsc::Sender<ChestMsg>> g_chestTx;
static DailyChest* g_pendingRefresh = nullptr;

static void clearDelegateIfSelf(DailyChest* self) {
    auto glm = GameLevelManager::sharedState();
    if (glm && glm->m_GJRewardDelegate == self) {
        glm->m_GJRewardDelegate = nullptr;
    }
}

DailyChest::~DailyChest() {
    clearDelegateIfSelf(this);
}

void DailyChest::getRewards() {
    auto glm = GameLevelManager::sharedState();
    if (glm) {
        glm->m_GJRewardDelegate = this;
        glm->getGJRewards(0);
    }
};

#include <Geode/utils/general.hpp>
void DailyChest::rewardsStatusFinished(int p0) {
    auto gsm = GameStatsManager::sharedState();
    if (gsm->m_rewardItems->count() == 0) {
        log::debug("[DailyChest] no reward items, aborting");
        clearDelegateIfSelf(this);
        if (g_pendingRefresh == this) { g_pendingRefresh = nullptr; this->release(); }
        return;
    }

    auto item1 = typeinfo_cast<GJRewardItem*>(gsm->m_rewardItems->objectForKey(0x1));
    auto item2 = typeinfo_cast<GJRewardItem*>(gsm->m_rewardItems->objectForKey(0x2));
    uint32_t t1 = item1 ? (uint32_t)rob_seconds(item1->m_timeRemaining) : 0;
    uint32_t t2 = item2 ? (uint32_t)rob_seconds(item2->m_timeRemaining) : 0;

    log::debug("[DailyChest] parsed timers: small={}s large={}s", t1, t2);

    if (g_chestTx) {
        (void)g_chestTx->trySend(ChestMsg{
            .type = ChestMsg::Type::Refresh,
            .smallTime = t1,
            .largeTime = t2
        });
    }
    clearDelegateIfSelf(this);
    if (g_pendingRefresh == this) { g_pendingRefresh = nullptr; this->release(); }
};

void DailyChest::rewardsStatusFailed() {
    log::error("[DailyChest] Failed to get rewards");
    clearDelegateIfSelf(this);
    if (g_pendingRefresh == this) { g_pendingRefresh = nullptr; this->release(); }
};

static void refreshRewards() {
    if (g_pendingRefresh) {
        g_pendingRefresh->release();
        g_pendingRefresh = nullptr;
    }
    auto dailyChest = new DailyChest();
    g_pendingRefresh = dailyChest;
    dailyChest->getRewards();
}

static void giveChestNotify(bool large) {
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
    void step2() {
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
        RewardUnlockLayer::step2();
        // Delay the refresh so it doesn't race with the chest UI's own getGJRewards request.
        // GD only allows one getGJRewards request at a time.
        // A typical request takes 1-3s; 5s gives enough margin even on slow connections.
        async::runtime().spawn([]() -> arc::Future<> {
            co_await arc::sleepFor(Duration::fromSecs(5));
            Loader::get()->queueInMainThread(refreshRewards);
        });
    }
};

static void toggleHook(bool value) {
    for (auto& hook : geode::Mod::get()->getHooks()) {
        if (hook->getDisplayName() == "RewardUnlockLayer::playRewardEffect") {
            (void)(value ? hook->enable() : hook->disable());
        }
    }
}

static void startChestWatcher() {
    if (g_chestTx) return;
    auto [tx, rx] = arc::mpsc::channel<ChestMsg>();
    g_chestTx = std::move(tx);
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
    refreshRewards();
}

static void stopChestWatcher() {
    if (!g_chestTx) return;
    toggleHook(false);
    (void)g_chestTx->trySend({ ChestMsg::Type::Stop });
    g_chestTx = std::nullopt;
}

$on_game(Loaded) {
    if (Mod::get()->getSettingValue<bool>("largeChest") || Mod::get()->getSettingValue<bool>("smallChest")) {
        startChestWatcher();
    } else {
        toggleHook(false);
    }
    listenForSettingChanges<bool>("smallChest", [](bool value) {
        if (value) {
            startChestWatcher();
        } else if (!Mod::get()->getSettingValue<bool>("largeChest")) {
            stopChestWatcher();
        }
    });
    listenForSettingChanges<bool>("largeChest", [](bool value) {
        if (value) {
            startChestWatcher();
        } else if (!Mod::get()->getSettingValue<bool>("smallChest")) {
            stopChestWatcher();
        }
    });
}
