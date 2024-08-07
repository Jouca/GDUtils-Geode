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
#include <algorithm>
#include <string>
#include <sstream>
#include "Notifications/DailyChest.h"
#include <codecvt>

static std::unordered_map<std::string, web::WebTask> RUNNING_REQUESTS {};
static std::mutex lock_var;

int reconnectionDelay = 1000;
int reconnectionDelayMax = 5000;
int reconnectionAttempts = 1000;

bool still_connected = false;
bool connect_finish = false;

bool event_fired = false;


std::queue<sio::message::ptr> dataQueue;
std::queue<int> chestQueue;

sio::message::ptr event_data = nullptr;
std::mutex lock;
std::unique_lock<std::mutex> unique_lock(lock);
std::condition_variable cond;
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
        cond.notify_all();
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
    log::info("setSocket - called");
    current_socket = sock;
    log::info("setSocket - Set current_socket");
    log::info("listening for events");
    current_socket->emit(fmt::format("geode-{}", Mod::get()->getVersion()));
    log::info("setSocket - emit event");
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
    log::info("Socket - start_socket_func");
    while (true) {
        log::info("Starting socket...");
        sio::client sock;
        sock.set_reconnect_delay(reconnectionDelay);
        sock.set_reconnect_delay_max(reconnectionDelayMax);
        sock.set_reconnect_attempts(reconnectionAttempts);
        log::info("Socket - Set options");
        sock.set_open_listener(&ConnectionHandler::onSuccess);
        log::info("Socket - Set open listener");
        sock.set_close_listener(&ConnectionHandler::onClose);
        log::info("Socket - Set close listener");
        sock.set_fail_listener(&ConnectionHandler::onFail);
        log::info("Socket - Set fail listener");
        sock.connect("http://gdutils.clarifygdps.com:13573");
        log::info("Socket - Connect to socket server");
        //sock.connect("http://gdutilstest.clarifygdps.com:46276");
        if (!connect_finish) {
            log::info("Socket - Wait for unique lock");
            cond.wait(unique_lock);
        }
        sock.socket()->on_error(ConnectionHandler::onError);
        log::info("Socket - Set on error event");
        setSocket(sock.socket());
        log::info("Socket - call setSocket");
        while (still_connected) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
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

        bool waitUntilExit = false;
        auto scene = CCDirector::sharedDirector()->getRunningScene();
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
            bool everywhereElse = Mod::get()->getSettingValue<bool>("everywhereElse");
            if ((layerName != "LevelEditorLayer" && layerName != "PlayLayer") && !everywhereElse) return;
            if ((layerName != "LevelEditorLayer" && layerName != "PlayLayer") && everywhereElse) {
                processChestEvent(scene);
                processEvent(scene);
            }
        }
        if (!event_fired) return;
        if (event_data == nullptr) return;
        event_fired = false;
        bool everywhereElse = Mod::get()->getSettingValue<bool>("everywhereElse");
        bool inLevels = Mod::get()->getSettingValue<bool>("inLevels");
        bool inEditor = Mod::get()->getSettingValue<bool>("inEditor");
        
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

        bool levelsProtocol = Mod::get()->getSettingValue<bool>("levelsProtocol");
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
                                            "Error",
                                            "An server error happened.",
                                            "OK",
                                            nullptr,
                                            180.0F
                                        )->show();
                                    } else {
                                        auto data = response->string().value();
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
                                        "Error",
                                        "An server error happened.",
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
        if (!strcmp(name, "GJ_gradientBG.png") && Mod::get()->getSettingValue<bool>("activate-background")) {
            Loader::get()->queueInMainThread([=] {
                if (ret->getColor() == ccc3(0, 102, 255)) {
                    auto scene = CCDirector::sharedDirector()->getRunningScene();
                    if (scene->getChildrenCount() == 0) return;
                    auto layer = scene->getChildren()->objectAtIndex(0);
                    std::string layerName = misc::getNodeName(layer);
                    ret->setColor(Mod::get()->getSettingValue<ccColor3B>("background"));
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
        if ((!strcmp(name, "square02b_001.png") || !strcmp(name, "square02b_small.png")) && Mod::get()->getSettingValue<bool>("activate-background")) {
            Loader::get()->queueInMainThread([=] {
                // jouca waht
                if (ret->getColor() == ccc3(0, 56, 141)) {
                    ccColor3B new_color = Mod::get()->getSettingValue<ccColor3B>("child-background");
                    if (Mod::get()->getSettingValue<bool>("syncBGcolor")) {
                        new_color = Mod::get()->getSettingValue<ccColor3B>("background");
                    }
                    if (Mod::get()->getSettingValue<bool>("addChildBDGradient")) {
                        new_color = ccc3(new_color.r * 0.79, new_color.g * 0.79, new_color.b * 0.79);
                    }
                    if (Mod::get()->getSettingValue<bool>("bgchild-transparency")) {
                        int opacity = Mod::get()->getSettingValue<int64_t>("bgchild-transparency-scroll");
                        ret->setOpacity(opacity);
                    }
                    ret->setColor(new_color);
                } else if (ret->getColor() == ccc3(0, 39, 98)) {
                    ccColor3B new_color = Mod::get()->getSettingValue<ccColor3B>("child-background");
                    if (Mod::get()->getSettingValue<bool>("syncBGcolor")) {
                        new_color = Mod::get()->getSettingValue<ccColor3B>("background");
                    }
                    if (Mod::get()->getSettingValue<bool>("addChildBDGradient")) {
                        new_color = ccc3(new_color.r * 0.71, new_color.g * 0.71, new_color.b * 0.71);
                    }
                    if (Mod::get()->getSettingValue<bool>("bgchild-transparency")) {
                        int opacity = Mod::get()->getSettingValue<int64_t>("bgchild-transparency-scroll");
                        ret->setOpacity(opacity);
                    }
                    ret->setColor(new_color);
                } else if (ret->getColor() == ccc3(0, 46, 117)) {
                    ccColor3B new_color = Mod::get()->getSettingValue<ccColor3B>("child-background");
                    if (Mod::get()->getSettingValue<bool>("syncBGcolor")) {
                        new_color = Mod::get()->getSettingValue<ccColor3B>("background");
                    }
                    if (Mod::get()->getSettingValue<bool>("addChildBDGradient")) {
                        new_color = ccc3(new_color.r * 0.74, new_color.g * 0.74, new_color.b * 0.74);
                    }
                    if (Mod::get()->getSettingValue<bool>("bgchild-transparency")) {
                        int opacity = Mod::get()->getSettingValue<int64_t>("bgchild-transparency-scroll");
                        ret->setOpacity(opacity);
                    }
                    ret->setColor(new_color);
                } else if (ret->getColor() == ccc3(0, 36, 91)) {
                    ccColor3B new_color = Mod::get()->getSettingValue<ccColor3B>("child-background");
                    if (Mod::get()->getSettingValue<bool>("syncBGcolor")) {
                        new_color = Mod::get()->getSettingValue<ccColor3B>("background");
                    }
                    if (Mod::get()->getSettingValue<bool>("addChildBDGradient")) {
                        new_color = ccc3(new_color.r * 0.70, new_color.g * 0.70, new_color.b * 0.70);
                    }
                    if (Mod::get()->getSettingValue<bool>("bgchild-transparency")) {
                        int opacity = Mod::get()->getSettingValue<int64_t>("bgchild-transparency-scroll");
                        ret->setOpacity(opacity);
                    }
                    ret->setColor(new_color);
                } else if (ret->getColor() == ccc3(0, 31, 79)) {
                    ccColor3B new_color = Mod::get()->getSettingValue<ccColor3B>("child-background");
                    if (Mod::get()->getSettingValue<bool>("syncBGcolor")) {
                        new_color = Mod::get()->getSettingValue<ccColor3B>("background");
                    }
                    if (Mod::get()->getSettingValue<bool>("addChildBDGradient")) {
                        new_color = ccc3(new_color.r * 0.67, new_color.g * 0.67, new_color.b * 0.67);
                    }
                    if (Mod::get()->getSettingValue<bool>("bgchild-transparency")) {
                        int opacity = Mod::get()->getSettingValue<int64_t>("bgchild-transparency-scroll");
                        ret->setOpacity(opacity);
                    }
                    ret->setColor(new_color);
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
        return true;
    }
};

// When the socket connection is made
$on_mod(Loaded) {
    log::info("GDUtils Mod Loaded");

    chestQueue.push(1);

    //Discord::init(); for next update ;)
    // also for whatever reason, discord rpc doesnt work on vanilla gd, instead it shows absolutes MH icon for some reason even though I dont own MH. can someone explain that
    
    Mod::get()->addCustomSetting<SettingTestValue>("test-notification", "none");
    Mod::get()->addCustomSetting<SettingPosValue>("notificationPlacement", 4);
    // Sections
    Mod::get()->addCustomSetting<SettingSectionValue>("notification-section", "none");
    Mod::get()->addCustomSetting<SettingSectionValue>("notification-placement-section", "none");
    Mod::get()->addCustomSetting<SettingSectionValue>("notification-appearance-section", "none");
    Mod::get()->addCustomSetting<SettingSectionValue>("spotify-section", "none");
    Mod::get()->addCustomSetting<SettingSectionValue>("misc-section", "none");
    Mod::get()->addCustomSetting<SettingSectionValue>("background-section", "none");
    Mod::get()->addCustomSetting<SettingSectionValue>("credits-section", "none");
    // ok listen here, you dont provide a way to JUST get the file name, you provide the full path which isnt what i want, so dont complain geode devs. Okay? we good? ok
    #ifdef GEODE_IS_WINDOWS
    Mod::get()->addCustomSetting<SettingAppValue>("spotifyApp", "Spotify.exe");
    #else
    Mod::get()->addCustomSetting<SettingAppValue>("spotifyApp", "Spotify.app");
    #endif
    Mod::get()->addCustomSetting<SettingCreditsValue>("credit-buttons", "none");

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

