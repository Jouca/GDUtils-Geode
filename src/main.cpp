// GDUtils Geode Port
// Ported with help by Firee
// Mod made by Jouca & Firee

#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/CCSprite.hpp>
#include <Geode/modify/CCScale9Sprite.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/loader/Log.hpp>
#include <Geode/utils/web.hpp>
#include "includes.h"
#include "Settings/CustomSettings.hpp"
#include "Notifications/EventsPush.h"
#include "ProcessLambdas.h"
#include <fmt/format.h>
#include <chrono>
#include <random>
#include <thread>
#include <queue>
#include <unordered_map>
#include <string>
#include "Notifications/DailyChest.h"
#include <codecvt>

int reconnectionDelay = 1000;
int reconnectionDelayMax = 5000;
int reconnectionAttempts = 100;

bool show_connected = false;

std::queue<mqtt::const_message_ptr> dataQueue;
std::queue<int> chestQueue;

std::queue<mqtt::const_message_ptr> eventQueue;

class MQTT {
    private:
    mqtt::async_client* m_client;
    mqtt::connect_options m_connOpts;
    std::string client_id;
    //const std::string topic("#");
    std::atomic<bool> connect_finish{false};
    std::atomic<bool> still_connected{false};

    class Callback : public virtual mqtt::callback, public virtual mqtt::iaction_listener {
        private:
            MQTT* handler;
        public:
            Callback(MQTT* handler) : handler(handler) {}

            void reconnect() {
                if (reconnectionAttempts-- <= 0) {
                    log::error("Maximum reconnection attempts reached, cannot reconnect. Restart the game to reconnect to the MQTT server.");
                    return;
                }
                log::warn("Disconnected from server. Attempting to reconnect... ({} left)", reconnectionAttempts);
                std::this_thread::sleep_for(std::chrono::milliseconds(reconnectionDelay));
                handler->connect_finish = true;
                handler->still_connected = false;
                //handler->m_client->connect(handler->m_connOpts, nullptr, *this);
            }

            void connected(const std::string& reason) override {
                log::info("MQTT Connection successful!");
                show_connected = true;
                handler->connect_finish = true;
                handler->still_connected = true;
                handler->m_client->subscribe("rate", 1);
            }
            void connection_lost(const std::string& reason) override {
                log::info("Connection closed: {}", reason);
                reconnect();
                handler->still_connected = false;
            }
            void on_success(const mqtt::token& tok) override {}
            void on_failure(const mqtt::token& tok) override {
                log::error("Connection failed: {}", tok.get_error_message());
                reconnect();
            }
            

            void message_arrived(mqtt::const_message_ptr data) override {
                log::info("call rate event");
                eventQueue.push(data);
            }

            void delivery_complete(mqtt::delivery_token_ptr token) override {}
    };
    std::unique_ptr<Callback> cb;
    bool m_calledConnect = false;

    public:
    MQTT(const std::string& client_id) : client_id(client_id) {
        m_connOpts.v5();
        m_connOpts.set_user_name("gd");
        m_connOpts.set_password("GeometryDashisahorizontalrunnerstylegamedevelopedandpublishedbyRobTopGames");
        m_connOpts.set_automatic_reconnect(true);
        m_connOpts.set_automatic_reconnect(
            reconnectionDelay / 1000,
            reconnectionDelayMax / 1000
        );
        // false = retain, true = dies after disconnect
        //m_connOpts.set_clean_session(true);
        m_connOpts.set_clean_session(false); 
        m_connOpts.set_connect_timeout(10);
    }
    void connect() {
        // https://github.com/eclipse-paho/paho.mqtt.cpp/blob/master/examples/async_subscribe.cpp
        if (m_calledConnect) return; // just so we don't cause any unnecessary problems...
        m_calledConnect = true;
        while (true) {
            log::info("Starting MQTT...");
            m_client = new mqtt::async_client("mqtt://gdutils.clarifygdps.com:1883", client_id);
            cb.reset(new Callback(this));
            m_client->set_callback(*cb);
            connect_finish = false;
            m_client->connect(m_connOpts, nullptr, *cb);
            auto start_time = std::chrono::steady_clock::now();
            while (!connect_finish) {
                /*if (std::chrono::steady_clock::now() - start_time > std::chrono::seconds(10)) {
                    log::error("Connection timeout");
                    break;
                }*/
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            if (connect_finish) {
                while (still_connected) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }
            if (reconnectionAttempts <= 0) {
                log::error("Maximum reconnection attempts reached, stopping to prevent any crashes.");
                break;
            }
            if (m_client && connect_finish) {
                delete m_client;
                m_client = nullptr;
            }
        }
    }
};

static std::unordered_map<std::string, web::WebTask> RUNNING_REQUESTS {};
static std::mutex lock_var;

std::string xorEncrypt(const std::string& input, const std::string& key) {
    std::string result;
    for (size_t i = 0; i < input.size(); ++i) {
        result += input[i] ^ key[i % key.size()];
    }
    return result;
}

// Daily chests notifications
/*void dailyChestThread() {
    while (true) {
        chestQueue.push(1);
        std::this_thread::sleep_for(std::chrono::minutes(1));
    }
}*/

void processEvent(CCScene* self) {
    if (!dataQueue.empty()) {
        auto data = dataQueue.front();
        dataQueue.pop();
        EventsPush::pushRateLevel(self, data);
    }
}

void processChestEvent(CCScene* self) {
    if (!chestQueue.empty()) {
        auto data = chestQueue.front();
        chestQueue.pop();
        auto dailyChest = new DailyChest();
        dailyChest->getRewards(0);
    }
}

class EventHandler : public CCObject {
    protected:
        std::string m_currentLayer;
    public:
        static EventHandler* create() {
            auto pRet = new EventHandler();
            if (pRet) {
                pRet->autorelease();
                return pRet;
            }
            CC_SAFE_DELETE(pRet);
            return nullptr;
        }
        void checkForEvent() {
            auto director = CCDirector::sharedDirector();
            if (!director) return;
            bool waitUntilExit = false;
            auto scene = CCDirector::sharedDirector()->getRunningScene();
            if (!scene) return;
            if (scene->getChildrenCount() == 0) return;
            if (show_connected) {
                show_connected = false;
                Notification::create("Connected to Rate Server!", NotificationIcon::Success)->show();
            }
            auto layer = scene->getChildren()->objectAtIndex(0);
            if (ProcessLambdas::shouldProcessMenuHandler()) {
                ProcessLambdas::processMenuHandler();
            }
            std::string layerName = misc::getNodeName(layer);
            if (layerName == "cocos2d::CCLayerColor") return;
            if (layerName == "LoadingLayer") return;
            if (m_currentLayer != layerName) {
                m_currentLayer = layerName;
                EventsPush::stopNow(scene);
                bool everywhereElse = Mod::get()->template getSettingValue<bool>("everywhereElse");
                if ((layerName != "LevelEditorLayer" && layerName != "PlayLayer") && !everywhereElse) return;
                if ((layerName != "LevelEditorLayer" && layerName != "PlayLayer") && everywhereElse) {
                    processChestEvent(scene);
                    processEvent(scene);
                }
            }
            if (eventQueue.empty()) return;
            bool everywhereElse = Mod::get()->template getSettingValue<bool>("everywhereElse");
            bool inLevels = Mod::get()->template getSettingValue<bool>("inLevels");
            bool inEditor = Mod::get()->template getSettingValue<bool>("inEditor");
            bool inPlatformers = Mod::get()->template getSettingValue<bool>("inPlatformers");
            
            bool pushEvent = true;
            if (layerName == "PlayLayer") {
                if (!inLevels) pushEvent = false;
                
                PlayLayer* playLayer = reinterpret_cast<PlayLayer*>(layer);
                GJGameLevel* level = playLayer->m_level;
                if (level->isPlatformer() && !inPlatformers) {
                    pushEvent = false;
                }
            }
            if (layerName == "LevelEditorLayer" && !inEditor) {
                pushEvent = false;
            }
            if ((layerName != "LevelEditorLayer" && layerName != "PlayLayer") && !everywhereElse) {
                pushEvent = false;
            }
            dataQueue.push(eventQueue.front());
            eventQueue.pop();
            if (pushEvent) processEvent(scene);
        }
        void checkForFiles() {
            #ifdef GEODE_IS_WINDOWS
            auto scene = CCDirector::sharedDirector()->getRunningScene();
            if (!scene) return;
            if (scene->getChildrenCount() == 0) return;
            auto layer = scene->getChildren()->objectAtIndex(0);
            if (layer == nullptr) return;
            std::string layerName = misc::getNodeName(layer);
            if (layerName == "cocos2d::CCLayerColor") return;
            if (layerName == "LoadingLayer") return;
            if (layerName == "LevelEditorLayer") return;
            if (layerName == "PlayLayer") return;
            if (layerName == "LoadingLayer") return;
            bool levelsProtocol = Mod::get()->template getSettingValue<bool>("levelsProtocol");
            if (!levelsProtocol) return;
            auto glm = GameLevelManager::sharedState();
            try {
                for (const auto & entry : std::filesystem::directory_iterator("gdutils")) {
                    if (entry.path().extension() == ".flag") {
                        std::string path = entry.path().string();
                        std::string filename = entry.path().filename().string();
                        std::string levelName = filename.substr(0, filename.length() - 5);

                        try {
                            std::filesystem::remove("gdutils/" + filename);
                        } catch (const std::exception& e) {
                            break;
                        }
                        
                        std::string url = "https://www.boomlings.com/database/getGJLevels21.php";
                        std::string fields = fmt::format("secret=Wmfd2893gb7&type=0&str={}", levelName);

                        geode::utils::web::WebRequest request = web::WebRequest();
                        request.userAgent("");
                        request.header("Content-Type", "application/x-www-form-urlencoded");
                        const std::lock_guard<std::mutex> lock(lock_var);
                        RUNNING_REQUESTS.emplace(
                            "@loaderLevelProtocolURL",
                            request.bodyString(fields).post(url).map(
                                [](web::WebResponse* response) {
                                    const std::lock_guard<std::mutex> lock(lock_var);
                                    if (response->ok()) {
                                        if (response->data().empty()) {
                                            FLAlertLayer::create(nullptr,
                                                "Error (GDUtils)",
                                                "An error has occured with <cy>Robtop's servers</c>. The response returned empty.",
                                                "OK",
                                                nullptr,
                                                180.0F
                                            )->show();
                                        } else {
                                            auto data = response->string().unwrapOrDefault();
                                            if (data != "-1") {
                                                auto scene = CCScene::create();

                                                GJGameLevel* gjgl = EventsPush::convertLevelToJSON(data);

                                                auto layer = LevelInfoLayer::create(gjgl, false);
                                                layer->downloadLevel();
                                                scene->addChild(layer);
                                                CCDirector::sharedDirector()->pushScene(cocos2d::CCTransitionFade::create(0.5f, scene));
                                            } else {
                                                FLAlertLayer::create(nullptr,
                                                    "Error",
                                                    "Level not found.",
                                                    "OK",
                                                    nullptr,
                                                    180.0F
                                                )->show();
                                            }
                                        }
                                    } else {
                                        FLAlertLayer::create(nullptr,
                                        "Error (GDUtils)",
                                            "An error has occured with <cy>Robtop's servers</c>. The reason could be because you are either ratelimited, blocked, or another issue.",
                                            "OK",
                                            nullptr,
                                            180.0F
                                        )->show();
                                    }

                                    RUNNING_REQUESTS.erase("@loaderLevelProtocolURL");
                                    return *response;
                                }
                            )
                        );
                    }
                }
            } catch (const std::exception& e) {}
            #endif
        }
        void check(float dt) {
            checkForEvent();
            checkForFiles();
        }
};

// Backgrounds
class $modify(CCSprite) {
    static CCSprite* create(char const* name) {
        auto ret = CCSprite::create(name);
        if (ret == nullptr) return ret;
        // better code, thanks camila!
        if (!strcmp(name, "GJ_gradientBG.png") && Mod::get()->template getSettingValue<bool>("activate-background")) {
	    ret->retain();
            Loader::get()->queueInMainThread([=] {
                if (ret != nullptr) {
                    if (ret->getColor() == ccc3(0, 102, 255)) {
                        auto scene = CCDirector::sharedDirector()->getRunningScene();
                        if (scene->getChildrenCount() == 0) return;
                        auto layer = scene->getChildren()->objectAtIndex(0);
                        std::string layerName = misc::getNodeName(layer);
                        ret->setColor(Mod::get()->template getSettingValue<ccColor3B>("background"));
                    }
                    ret->release();
                }
            });

        }
        return ret;
    }
};

// Child background
class $modify(CCScale9Sprite) {
    static cocos2d::extension::CCScale9Sprite* create(char const* name, CCRect rect) {
        auto ret = cocos2d::extension::CCScale9Sprite::create(name, rect);
        if (ret == nullptr) return ret;
        // better code, thanks camila!
        if ((!strcmp(name, "square02b_001.png") || !strcmp(name, "square02b_small.png")) && Mod::get()->template getSettingValue<bool>("activate-background")) {
	    ret->retain();
            Loader::get()->queueInMainThread([=] {
                if (ret != nullptr) {
                    // jouca waht
                    if (ret->getColor() == ccc3(0, 56, 141)) {
                        ccColor3B new_color = Mod::get()->template getSettingValue<ccColor3B>("child-background");
                        if (Mod::get()->template getSettingValue<bool>("syncBGcolor")) {
                            new_color = Mod::get()->template getSettingValue<ccColor3B>("background");
                        }
                        if (Mod::get()->template getSettingValue<bool>("addChildBDGradient")) {
                            new_color = ccc3(new_color.r * 0.79, new_color.g * 0.79, new_color.b * 0.79);
                        }
                        if (Mod::get()->template getSettingValue<bool>("bgchild-transparency")) {
                            int opacity = Mod::get()->template getSettingValue<int64_t>("bgchild-transparency-scroll");
                            ret->setOpacity(opacity);
                        }
                        ret->setColor(new_color);
                    } else if (ret->getColor() == ccc3(0, 39, 98)) {
                        ccColor3B new_color = Mod::get()->template getSettingValue<ccColor3B>("child-background");
                        if (Mod::get()->template getSettingValue<bool>("syncBGcolor")) {
                            new_color = Mod::get()->template getSettingValue<ccColor3B>("background");
                        }
                        if (Mod::get()->template getSettingValue<bool>("addChildBDGradient")) {
                            new_color = ccc3(new_color.r * 0.71, new_color.g * 0.71, new_color.b * 0.71);
                        }
                        if (Mod::get()->template getSettingValue<bool>("bgchild-transparency")) {
                            int opacity = Mod::get()->template getSettingValue<int64_t>("bgchild-transparency-scroll");
                            ret->setOpacity(opacity);
                        }
                        ret->setColor(new_color);
                    } else if (ret->getColor() == ccc3(0, 46, 117)) {
                        ccColor3B new_color = Mod::get()->template getSettingValue<ccColor3B>("child-background");
                        if (Mod::get()->template getSettingValue<bool>("syncBGcolor")) {
                            new_color = Mod::get()->template getSettingValue<ccColor3B>("background");
                        }
                        if (Mod::get()->template getSettingValue<bool>("addChildBDGradient")) {
                            new_color = ccc3(new_color.r * 0.74, new_color.g * 0.74, new_color.b * 0.74);
                        }
                        if (Mod::get()->template getSettingValue<bool>("bgchild-transparency")) {
                            int opacity = Mod::get()->template getSettingValue<int64_t>("bgchild-transparency-scroll");
                            ret->setOpacity(opacity);
                        }
                        ret->setColor(new_color);
                    } else if (ret->getColor() == ccc3(0, 36, 91)) {
                        ccColor3B new_color = Mod::get()->template getSettingValue<ccColor3B>("child-background");
                        if (Mod::get()->template getSettingValue<bool>("syncBGcolor")) {
                            new_color = Mod::get()->template getSettingValue<ccColor3B>("background");
                        }
                        if (Mod::get()->template getSettingValue<bool>("addChildBDGradient")) {
                            new_color = ccc3(new_color.r * 0.70, new_color.g * 0.70, new_color.b * 0.70);
                        }
                        if (Mod::get()->template getSettingValue<bool>("bgchild-transparency")) {
                            int opacity = Mod::get()->template getSettingValue<int64_t>("bgchild-transparency-scroll");
                            ret->setOpacity(opacity);
                        }
                        ret->setColor(new_color);
                    } else if (ret->getColor() == ccc3(0, 31, 79)) {
                        ccColor3B new_color = Mod::get()->template getSettingValue<ccColor3B>("child-background");
                        if (Mod::get()->template getSettingValue<bool>("syncBGcolor")) {
                            new_color = Mod::get()->template getSettingValue<ccColor3B>("background");
                        }
                        if (Mod::get()->template getSettingValue<bool>("addChildBDGradient")) {
                            new_color = ccc3(new_color.r * 0.67, new_color.g * 0.67, new_color.b * 0.67);
                        }
                        if (Mod::get()->template getSettingValue<bool>("bgchild-transparency")) {
                            int opacity = Mod::get()->template getSettingValue<int64_t>("bgchild-transparency-scroll");
                            ret->setOpacity(opacity);
                        }
                        ret->setColor(new_color);
                    }
                    ret->release();
                }
            });
        }
        return ret;
    }
};

//bool is_dailychest_ready = false;
bool is_socketserver_started = false;
class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;
        
        /*if (!is_dailychest_ready) {
            std::thread hThread(dailyChestThread);
            hThread.detach();
            is_dailychest_ready = true;
        }*/
        if (!is_socketserver_started) {
            bool startSocketServer = Mod::get()->template getSettingValue<bool>("socketServer");
            if (startSocketServer) {
                if (!Mod::get()->hasSavedValue("clientId")) {
                    Mod::get()->setSavedValue("clientId", misc::genClientID());
                }
                if (!Mod::get()->hasSavedValue("clientId")) {
                    Notification::create("Couldn't connect to server, invalid Client ID", NotificationIcon::Error)->show();
                    return true;
                }
                std::thread hThread([]() {
                    MQTT handler(Mod::get()->getSavedValue<std::string>("clientId"));
                    handler.connect();
                });
                hThread.detach();
            }
            is_socketserver_started = true;
        }
        return true;
    }
};

$execute {
    (void)Mod::get()->registerCustomSettingType("credit-buttons", &SettingCreditsValue::parse);
    //(void)Mod::get()->registerCustomSettingType("notificationPlacement", &SettingPosValue::parse);
    auto ret = Mod::get()->registerCustomSettingType("notification-placement", &SettingPosValue::parse);
    if (!ret) {
        log::error("Unable to register setting type: {}", ret.unwrapErr());
    }
    Loader::get()->queueInMainThread([]{
        CCScheduler::get()->scheduleSelector(schedule_selector(EventHandler::check), EventHandler::create(), 1.0F, false);
        //CCScheduler::get()->scheduleUpdateForTarget(EventHandler::create(), Priority::Last, false);
    });
}

// When the socket connection is made
$on_mod(Loaded) {
    log::info("GDUtils Mod Loaded");

    chestQueue.push(1);

    //Discord::init(); for next update ;)
    // also for whatever reason, discord rpc doesnt work on vanilla gd, instead it shows absolutes MH icon for some reason even though I dont own MH. can someone explain that

    #ifdef GEODE_IS_WINDOWS
    #include <Windows.h>
    #include <iostream>

    std::array<WCHAR, MAX_PATH> buffer;
    GetModuleFileNameW(NULL, buffer.data(), MAX_PATH);
    auto aaaa = buffer.data();
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string nameStr = fmt::format("cmd /k cd \"{}\\gdutils\" && \"gd_protocol.bat\" %1 && exit", dirs::getGameDir());

    std::wstring fullPath = std::wstring(buffer.data());
    size_t lastBackslashPos = fullPath.find_last_of(L"\\");
    std::wstring exeName;
    if (lastBackslashPos != std::wstring::npos) {
        exeName = fullPath.substr(lastBackslashPos + 1);
    }

    // Create the key for the custom protocol in user registry
    HKEY hCurrentUserKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Classes", 0, KEY_SET_VALUE, &hCurrentUserKey) == ERROR_SUCCESS) {
        HKEY hKey;
        if (RegCreateKeyEx(hCurrentUserKey, "gdutils", 0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
            // Define the default value for the protocol key
            if (RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE*)"URL:gdutils Protocol", sizeof("URL:gdutils Protocol")) == ERROR_SUCCESS) {
                // Create the key "shell\open\command" for specify the exe path
                HKEY hCommandKey;
                if (RegCreateKeyEx(hKey, "shell\\open\\command", 0, NULL, 0, KEY_SET_VALUE, NULL, &hCommandKey, NULL) == ERROR_SUCCESS) {
                    if (RegSetValueExA(hCommandKey, NULL, 0, REG_SZ, (const BYTE*)nameStr.c_str(), nameStr.length() + 1) == ERROR_SUCCESS) {
                        log::info("Le protocole a été enregistré avec succès pour cet utilisateur.");
                    } else {
                        log::error("Erreur lors de la définition de la valeur du chemin de l'exécutable.");
                    }
                    RegCloseKey(hCommandKey);
                } else {
                    log::error("Erreur lors de la création de la clé shell\\open\\command.");
                }

                RegSetValueEx(hKey, "URL Protocol", 0, REG_SZ, NULL, NULL);
            } else {
                log::error("Erreur lors de la définition de la valeur par défaut du protocole.");
            }
            RegCloseKey(hKey);
        } else {
            log::error("Erreur lors de la création de la clé du protocole.");
        }
        RegCloseKey(hCurrentUserKey);
    } else {
        log::error("Erreur lors de l'ouverture de la clé du registre utilisateur.");
    }

    std::filesystem::create_directory("gdutils");
    if (!std::filesystem::exists("gdutils/gd_protocol.bat")) {
        std::ofstream file("gdutils/gd_protocol.bat");
        file << R"(
@echo off
setlocal enabledelayedexpansion

        )";
        file << fmt::format("set \"appName={}\"", converter.to_bytes(exeName));
        file << R"(
set "argument=%1"
set "number="

REM Check if the argument contains /
echo !argument! | find "/" > nul
if not errorlevel 1 (
    REM If / is found, extract the number part after the /
    for /f "tokens=2 delims=/" %%a in ("!argument!") do (
        set "number=%%a"
    )
) else (
    REM If / is not found, the argument is in the format gdutils:123
    for /f "tokens=2 delims=:" %%a in ("!argument!") do (
        set "number=%%a"
    )
)

if not defined number (
    echo Invalid argument format: %1
    exit /b 1
)

set "flagFile=%number%.flag"

REM Check if the application is already running
tasklist /FI "IMAGENAME eq %appName%" | find /i "%appName%" > nul
if errorlevel 1 (
    REM If not running, start the application with the provided argument
    cd ..
    start "" "%appName%"
    cd gdutils
    type nul > "%flagFile%"
) else (
    REM If running, create an empty file based on the extracted number
    type nul > "%flagFile%"
)

endlocal
        )";
        file.close();
    }

    #endif
}
