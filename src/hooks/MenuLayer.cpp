#include <network/AMQClient.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <random>
#include <arc/sync/mpsc.hpp>
#include <modules/events/EventHandler.hpp>

bool is_socketserver_started = false;
class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        return true;

        /*if (!is_dailychest_ready) {
            std::thread hThread(dailyChestThread);
            hThread.detach();
            is_dailychest_ready = true;
        }*/
        if (!is_socketserver_started) {
            bool startSocketServer = Mod::get()->template getSettingValue<bool>("socketServer");
            if (startSocketServer) {
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
                    return true;
                }
                /*auto [tx, rx] = arc::mpsc::channel<std::string>(1024); // i mean why would robtop ever rate that many levels
                async::runtime().spawn([rx = std::move(rx)]() mutable -> arc::Future<> {
                    while (true) {
                        auto opt = co_await rx.recv();
                        if (!opt) {
                            co_return;
                        }
                        std::string msg = std::move(opt).unwrap();

                        Loader::get()->queueInMainThread([m = std::move(msg)]() mutable {
                            auto director = CCDirector::sharedDirector();
                            if (!director) return;
                            auto scene = CCDirector::sharedDirector()->getRunningScene();
                            if (!scene) return;
                            if (scene->getChildrenCount() == 0) return;
                            EventHandler::aaa();
                        });
                    }
                    co_return;
                });
                auto director = CCDirector::sharedDirector();
                
                if (director) {
                    auto scene = CCDirector::sharedDirector()->getRunningScene();
                    if (scene && scene->getChildrenCount() > 0) {
                        //arc::spawn(EventHandler::consumer(std::move(rx), scene));
                    }
                }*/
                //arc::spawn(EventHandler::eventLoop(scene, std::move(rx)));
                //[tx = std::move(tx)
                std::thread hThread([]() mutable {
                    AMQT handler(Mod::get()->getSavedValue<std::string>("clientId"));
                    handler.connect();
                });
                hThread.detach();
            }
            is_socketserver_started = true;
        }
        return true;
    }
};

