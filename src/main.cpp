// GDUtils Geode Port
// Ported with help by Firee
// Mod made by Jouca & Firee

#include <Geode/modify/CCScheduler.hpp>
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
#include <thread>
#include <queue>
#include <unordered_map>
#include <string>
#include "Notifications/DailyChest.h"
#include <codecvt>

static std::unordered_map<std::string, web::WebTask> RUNNING_REQUESTS {};
static std::mutex lock_var;

int reconnectionDelay = 1000;
int reconnectionDelayMax = 5000;
int reconnectionAttempts = 1000;

std::atomic<bool> connect_finish(false);
std::atomic<bool> still_connected(false);

bool event_fired = false;


std::queue<sio::message::ptr> dataQueue;
std::queue<int> chestQueue;

sio::message::ptr event_data = nullptr;
/*std::mutex lock;
std::unique_lock<std::mutex> unique_lock(lock);
std::condition_variable cond;*/ 
sio::socket::ptr current_socket;

// for some reason log and fmt dont work together on android

std::string xorEncrypt(const std::string& input, const std::string& key) {
    std::string result;
    for (size_t i = 0; i < input.size(); ++i) {
        result += input[i] ^ key[i % key.size()];
    }
    return result;
}

namespace ConnectionHandler {
    void onSuccess() {
        log::info("Socket Connection successful!");
        connect_finish = true;
        still_connected = true;
        //cond.notify_all();
    }

    void onClose(sio::client::close_reason const& reason) {
        log::warn("Connection closed: {}", std::to_string(reason));
        still_connected = false;
    }

    void onFail() {
        log::error("Connection failed.");
        still_connected = false;
    }

    void onError(sio::message::ptr const& message) {
        log::error("Sock Error: {}", message->get_string());
    }
}
bool setSocket(sio::socket::ptr sock) {
    current_socket = sock;
    current_socket->emit(fmt::format("geode-{}", Mod::get()->getVersion()));
    current_socket->on("rate", sio::socket::event_listener_aux([&](std::string const& user, sio::message::ptr const& data, bool isAck, sio::message::list &ack_resp) {
        log::info("call rate event");
        event_fired = true;
        event_data = data;
    }));

    current_socket->on("connect", sio::socket::event_listener_aux([&](std::string const& user, sio::message::ptr const& data, bool isAck, sio::message::list &ack_resp) {
        log::info("Connect Event");
    }));
    return true;
}

// Daily chests notifications
/*void dailyChestThread() {
    while (true) {
        chestQueue.push(1);
        std::this_thread::sleep_for(std::chrono::minutes(1));
    }
}*/

void start_socket_func() {
    while (true) {
        log::info("Starting socket...");
        sio::client sock;
        connect_finish = false;
        sock.set_reconnect_delay(reconnectionDelay);
        sock.set_reconnect_delay_max(reconnectionDelayMax);
        sock.set_reconnect_attempts(reconnectionAttempts);
        sock.set_open_listener(&ConnectionHandler::onSuccess);
        sock.set_close_listener(&ConnectionHandler::onClose);
        sock.set_fail_listener(&ConnectionHandler::onFail);
        sock.set_logs_quiet();
        sock.connect("http://gdutils.clarifygdps.com:13573");
        auto start_time = std::chrono::steady_clock::now();
        while (!connect_finish) {
            if (std::chrono::steady_clock::now() - start_time > std::chrono::seconds(10)) {
                log::error("Connection timeout");
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        if (connect_finish) {
            sock.socket()->on_error(ConnectionHandler::onError);
            setSocket(sock.socket());
            while (still_connected) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        if (!still_connected) {
            log::warn("Disconnected from server. Attempting to reconnect...");
            std::this_thread::sleep_for(std::chrono::seconds(reconnectionDelay));
        }
        if (reconnectionAttempts-- <= 0) {
            log::error("Maximum reconnection attempts reached, stopping to prevent any crashes.");
            break;
        }
    }
}

std::string currentLayer = "";
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

class $modify(CCScheduler) { // used to be GameManager
    void update(float dt) {
        CCScheduler::update(dt);
        auto director = CCDirector::sharedDirector();
        if (!director) return;
        bool waitUntilExit = false;
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        if (!scene) return;
        if (scene->getChildrenCount() == 0) return;
        auto layer = scene->getChildren()->objectAtIndex(0);
        if (layer == nullptr) return;
        if (ProcessLambdas::shouldProcessMenuHandler()) {
            ProcessLambdas::processMenuHandler();
        }
        std::string layerName = misc::getNodeName(layer);
        if (layerName == "cocos2d::CCLayerColor") return;
        if (layerName == "LoadingLayer") return;
        if (currentLayer != layerName) {
            currentLayer = layerName;
            EventsPush::stopNow(scene);
            bool everywhereElse = Mod::get()->template getSettingValue<bool>("everywhereElse");
            if ((layerName != "LevelEditorLayer" && layerName != "PlayLayer") && !everywhereElse) return;
            if ((layerName != "LevelEditorLayer" && layerName != "PlayLayer") && everywhereElse) {
                processChestEvent(scene);
                processEvent(scene);
            }
        }
        if (!event_fired) return;
        if (event_data == nullptr) return;
        event_fired = false;
        bool everywhereElse = Mod::get()->template getSettingValue<bool>("everywhereElse");
        bool inLevels = Mod::get()->template getSettingValue<bool>("inLevels");
        bool inEditor = Mod::get()->template getSettingValue<bool>("inEditor");
        
        bool pushEvent = true;
        if (layerName == "PlayLayer" && !inLevels) {
            pushEvent = false;
        }
        if (layerName == "LevelEditorLayer" && !inEditor) {
            pushEvent = false;
        }
        if ((layerName != "LevelEditorLayer" && layerName != "PlayLayer") && !everywhereElse) {
            pushEvent = false;
        }
        dataQueue.push(event_data);
        event_data = nullptr;
        if (pushEvent) processEvent(scene);
    }
};

// NOTE: try catch statements crash on android

#ifndef GEODE_IS_ANDROID
class $modify(CCScheduler) { // GD Protocol part
    void update(float dt) {
        CCScheduler::update(dt);
        auto scene = CCDirector::sharedDirector()->getRunningScene();
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
    }
};
#endif

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
                current_socket = sio::socket::ptr();
                std::thread hThread(start_socket_func);
                hThread.detach();
            }
            is_socketserver_started = true;
        }
        return true;
    }
};

/*
class $modify(MyMenuLayerGDUTILS, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;


        // delete this 
        auto myButton = CCMenuItemSpriteExtra::create(
			CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
			this,
			menu_selector(MyMenuLayerGDUTILS::onMyButton)
		);
        // delete this
        auto menu = this->getChildByID("bottom-menu");
		menu->addChild(myButton);

		myButton->setID("my-button"_spr);

		menu->updateLayout();
        return true;
    }
    void onMyButton(CCObject*) {
        if (!is_socketserver_started) {
            log::info("Socket - MenuLayer::init");
            bool startSocketServer = Mod::get()->getSettingValue<bool>("socketServer");
            if (startSocketServer) {
                log::info("Socket - Prepare Starting Socket Server");
                current_socket = sio::socket::ptr();
                log::info("Socket - Set current_socket");
                std::thread hThread(start_socket_func);
                log::info("Socket - Create new thread");
                hThread.detach();
                log::info("Socket - Detach thread");
            }
            is_socketserver_started = true;
        }

    }
};
*/

$execute {
    (void)Mod::get()->registerCustomSettingType("credit-buttons", &SettingCreditsValue::parse);
    //(void)Mod::get()->registerCustomSettingType("notificationPlacement", &SettingPosValue::parse);
    auto ret = Mod::get()->registerCustomSettingType("notification-placement", &SettingPosValue::parse);
    if (!ret) {
        log::error("Unable to register setting type: {}", ret.unwrapErr());
    }
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
