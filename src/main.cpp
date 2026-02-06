// GDUtils Geode Port
// Ported with help by Firee
// Mod made by Jouca & Firee

$execute {
    // Loader::get()->queueInMainThread([]{
    //     CCScheduler::get()->scheduleSelector(schedule_selector(EventHandler::check), EventHandler::create(), 1.0F, false);
    //     //CCScheduler::get()->scheduleUpdateForTarget(EventHandler::create(), Priority::Last, false);
    // });
    // listenForSettingChanges("activate-background", [](bool value) {
    //     log::info("Activate Background changed to {}", value);
    //     toggleSpriteHooks(value);
    // });
}
#include <Geode/Geode.hpp>
#include <network/AMQClient.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <random>
#include <arc/sync/mpsc.hpp>
#include <arc/sync/Mutex.hpp>
#include <arc/future/Select.hpp>
#include <modules/events/EventHandler.hpp>
#include <modules/events/EventsPush.hpp>

bool g_socket_server_ran = false;

$on_game(Loaded) {
    if (g_socket_server_ran) return;
    if (geode::Mod::get()->getSettingValue<bool>("socketServer")) {
        g_socket_server_ran = true;
        if (!Mod::get()->hasSavedValue("clientId")) {
            // this is just for statistics ok
            #ifdef GEODE_IS_WINDOWS 
            std::string platform = "Windows";
            #elif defined(GEODE_IS_ANDROID)
            std::string platform = "Android";
            #elif defined(GEODE_IS_MACOS)
            std::string platform = "Mac";
            #elif defined(GEODE_IS_IOS)
            std::string platform = "iOS";
            #else 
            std::string platform = "Unknown";
            #endif
            const std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dist(0, charset.size() - 1);
            std::string randomStr;
            for (size_t i = 0; i < 8; ++i) {
                randomStr.push_back(charset[dist(gen)]);
            }
            auto now = std::chrono::system_clock::now();
            auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
            int accountID = 0;
            if (auto gjam = GJAccountManager::get()) {
                accountID = gjam->m_accountID;
            }
            Mod::get()->setSavedValue("clientId", fmt::format("gdutils-{}-{}-{}-{}", accountID, platform, now_ms, randomStr));
        }
        if (!Mod::get()->hasSavedValue("clientId")) {
            Notification::create("Couldn't connect to server, invalid Client ID", NotificationIcon::Error)->show();
            return;
        }
        auto [tx, rx] = arc::mpsc::channel<std::string>(1024); // i mean why would robtop ever rate that many levels
        auto [etx, erx] = arc::mpsc::channel<EventData>(1024); // ditto
        auto [ctx, crx] = arc::mpsc::channel<bool>(64);
        AMQT::setTx(std::move(tx));
        EventsPush::setTx(std::move(ctx));
        async::runtime().spawn([rx = std::move(rx), crx = std::move(crx), erx = std::move(erx)]() mutable -> arc::Future<> {
            bool running = true;
            while (running) {
                co_await arc::select(
                    arc::selectee(rx.recv(), [&](auto req) {
                        if (req) {
                            std::string msg = std::move(req).unwrap();
                            Loader::get()->queueInMainThread([m = std::move(msg)]() mutable {
                                auto director = CCDirector::sharedDirector();
                                if (!director) return;
                                auto scene = CCDirector::sharedDirector()->getRunningScene();
                                if (!scene) return;
                                if (scene->getChildrenCount() == 0) return;
                                if (EventsPush::canSendEvent(scene)) {
                                    EventsPush::pushRateLevel(scene, m);
                                }
                            });
                        }
                    }),
                    arc::selectee(crx.recv(), [&](auto req) {
                        if (req) {
                            bool b = std::move(req).unwrap();
                            log::debug("Cancelled (Code = {})", b);
                        } else {
                            log::debug("Cancelled (Code = Unknown)");
                        }
                        Loader::get()->queueInMainThread([]() {
                            if (auto scene = CCScene::get()) {
                                EventsPush::eventCompletedCallback(scene);
                            }
                        });
                    })
                );
            }
        });
        std::thread hThread([]() mutable {
            AMQT handler(Mod::get()->getSavedValue<std::string>("clientId"));
            handler.connect();
        });
        hThread.detach();
    }
}
