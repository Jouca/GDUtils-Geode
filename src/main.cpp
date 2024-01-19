// GDUtils Geode Port
// Ported with help by Firee
// Mod made by Jouca & Firee

//#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/FriendsProfilePage.hpp>
#include <Geode/modify/SecretLayer2.hpp>
#include <Geode/modify/LeaderboardsLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/InfoLayer.hpp>
#include <Geode/modify/ProfilePage.hpp>
#include <Geode/loader/Log.hpp>
#include <Geode/utils/web.hpp>
#include "includes.h"
#include "CustomSettings.hpp"
#include "EventsPush.h"
#include "ProcessLambdas.h"
#include "MoreLeaderboards.h"
#include "InfoNewLayer.h"
#include "NewProfilePage.h"
#include "Discord.h"
#include <fmt/format.h>
#include <chrono>
#ifndef GEODE_IS_MACOS
#include "DownloadManager.h"
#else // mac
#include <ctime>
#include <cxxabi.h>
#endif
#include <thread>
#include <queue>
#include <unordered_map>
#include <algorithm>

int reconnectionDelay = 1000;
int reconnectionDelayMax = 5000;
int reconnectionAttempts = 1000;

bool still_connected = false;
bool connect_finish = false;

bool event_fired = false;

std::queue<sio::message::ptr> dataQueue;

sio::message::ptr event_data = nullptr;
std::mutex lock;
std::unique_lock<std::mutex> unique_lock(lock);
std::condition_variable cond;
sio::socket::ptr current_socket;

// for some reason log and fmt dont work together on android

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
    current_socket = sock;
    log::info("listening for events");
    current_socket->emit("geode-" + Mod::get()->getVersion().toString());
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

void start_socket_func() {
    while (true) {
        try {
            log::info("Starting socket...");
            sio::client sock;
            sock.set_reconnect_delay(reconnectionDelay);
            sock.set_reconnect_delay_max(reconnectionDelayMax);
            sock.set_reconnect_attempts(reconnectionAttempts);
            sock.set_open_listener(&ConnectionHandler::onSuccess);
            sock.set_close_listener(&ConnectionHandler::onClose);
            sock.set_fail_listener(&ConnectionHandler::onFail);
            sock.connect("http://gdutils.clarifygdps.com");
            if (!connect_finish) {
                cond.wait(unique_lock);
            }
            sock.socket()->on_error(ConnectionHandler::onError);
            setSocket(sock.socket());
            while (still_connected) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        } catch (const std::exception& e) {
            log::error("Error in Socket Thread: {}", std::string(e.what()));
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
        log::debug("process event");
        auto data = dataQueue.front();
        dataQueue.pop();
        EventsPush::pushRateLevel(self, data);
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
            //Discord::update(layerName.c_str()); for next update ;)
            currentLayer = layerName;
            EventsPush::stopNow(scene);
            bool everywhereElse = Mod::get()->getSettingValue<bool>("everywhereElse");
            if ((layerName != "LevelEditorLayer" && layerName != "PlayLayer") && !everywhereElse) return;
            if ((layerName != "LevelEditorLayer" && layerName != "PlayLayer") && everywhereElse) {
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
#ifndef GEODE_IS_ANDROID
class $modify(CCScheduler) { // GD Protocol part
    void update(float dt) {
        CCScheduler::update(dt);
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        if (scene->getChildrenCount() == 0) return;
        auto layer = scene->getChildren()->objectAtIndex(0);
        if (layer == nullptr) return;
        if (ProcessLambdas::shouldProcessMenuHandler()) {
            ProcessLambdas::processMenuHandler();
        }
        std::string layerName = misc::getNodeName(layer);
        
        if (layerName == "LevelEditorLayer") return;
        if (layerName == "PlayLayer") return;
        if (layerName == "LoadingLayer") return;

        auto glm = GameLevelManager::sharedState();

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
                
                std::string const& url = "https://www.boomlings.com/database/getGJLevels21.php";
                std::string const& fields = "secret=Wmfd2893gb7&type=0&str=" + levelName;

                std::cout << fields << std::endl;
                web::AsyncWebRequest()
                    .bodyRaw(fields)
                    .postRequest()
                    .fetch(url).text()
                    .then([&](std::string & response) {
                        if (response != "-1") {
                            auto scene = CCScene::create();

                            GJGameLevel* gjgl = EventsPush::convertLevelToJSON(response);

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
                }).expect([](std::string const& error) {
                    FLAlertLayer::create(nullptr,
                        "Error",
                        "An error happened when trying to fetch the level.",
                        "OK",
                        nullptr,
                        350.0F
                    )->show();
                });
            }
        }
    }
};
#endif

// Spotify
bool is_muted = false;
#ifdef GEODE_IS_WINDOWS
#include <commdlg.h>
#include <filesystem>
#include <locale>
#include <codecvt>
#include <cctype>
#include <cstring>
#include <tchar.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audiopolicy.h>
#include <psapi.h>
#include <TlHelp32.h>

// Function to convert a multi-byte string to a wide string
std::wstring StringToWString(const std::string& str) {
    int length = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    wchar_t* buffer = new wchar_t[length];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer, length);
    std::wstring result(buffer);
    delete[] buffer;
    return result;
}

// Function to get the process ID by its executable name
DWORD GetProcessIdByName(const std::wstring& executableName) {
    DWORD processId = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(processEntry);

        if (Process32First(snapshot, &processEntry)) {
            do {
                std::wstring processNameOnly = StringToWString(processEntry.szExeFile);
                if (_wcsicmp(processNameOnly.c_str(), executableName.c_str()) == 0) {
                    processId = processEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(snapshot, &processEntry));
        }

        CloseHandle(snapshot);
    }

    return processId;
}

// Function to mute the audio session of the specified process
void MuteAudioSessionByProcessId(DWORD processId, bool mute) {
    CoInitialize(NULL);
    HRESULT hr = S_OK;

    // Create an audio device enumerator
    IMMDeviceEnumerator* pEnumerator = NULL;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator), (void**)&pEnumerator);

    if (SUCCEEDED(hr)) {
        // Get the default audio rendering endpoint
        IMMDevice* pDevice = NULL;
        hr = pEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);

        if (SUCCEEDED(hr)) {
            // Activate the audio session manager
            IAudioSessionManager2* pSessionManager = NULL;
            hr = pDevice->Activate(__uuidof(IAudioSessionManager2), CLSCTX_ALL,
                NULL, (void**)&pSessionManager);

            pDevice->Release(); // Release the device object
            if (SUCCEEDED(hr)) {
                // Get the session enumerator
                IAudioSessionEnumerator* pSessionEnumerator = NULL;
                hr = pSessionManager->GetSessionEnumerator(&pSessionEnumerator);

                if (SUCCEEDED(hr)) {
                    int count;
                    pSessionEnumerator->GetCount(&count);

                    // Iterate through all audio sessions
                    for (int i = 0; i < count; i++) {
                        IAudioSessionControl* pSessionControl = NULL;
                        hr = pSessionEnumerator->GetSession(i, &pSessionControl);

                        if (SUCCEEDED(hr)) {
                            IAudioSessionControl2* pSessionControl2 = NULL;
                            hr = pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void**)&pSessionControl2);

                            if (SUCCEEDED(hr)) {
                                // Get the session process ID
                                DWORD sessionProcessId;
                                hr = pSessionControl2->GetProcessId(&sessionProcessId);

                                if (SUCCEEDED(hr) && sessionProcessId == processId) {
                                    ISimpleAudioVolume* pSessionVolume = NULL;
                                    hr = pSessionControl->QueryInterface(__uuidof(ISimpleAudioVolume), (void**)&pSessionVolume);

                                    if (SUCCEEDED(hr)) {
                                        BOOL bMute = FALSE;
                                        pSessionVolume->GetMute(&bMute);
                                        if (mute) {
                                            log::info("Spotify has been muted.");
                                        } else {
                                            log::info("Spotify has been unmuted.");
                                        }
                                        pSessionVolume->SetMute(mute, NULL); // Mute the session
                                        pSessionVolume->Release();
                                    }
                                }

                                pSessionControl2->Release();
                            }

                            pSessionControl->Release();
                        }
                    }

                    pSessionEnumerator->Release();
                }

                pSessionManager->Release();
            }
        }

        pEnumerator->Release();
    }

    CoUninitialize();
}

std::wstring StringToWstring(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    return converter.from_bytes(str);
}

void toggleSpotifyMute(bool automatic = false, bool muted = false) {
    if (!automatic) {
        is_muted = !is_muted;
    } else {
        is_muted = muted;
    }

    if (is_muted) {
        log::info("Muting Spotify...");
    } else {
        log::info("Unmuting Spotify...");
    }
    //std::wstring targetExeName = L"Spotify.exe";
    std::wstring targetExeName = StringToWstring(Mod::get()->getSettingValue<SettingAppStruct>("spotifyApp").m_application);
    log::info("Mute Application " + Mod::get()->getSettingValue<SettingAppStruct>("spotifyApp").m_application);
    DWORD processId = GetProcessIdByName(targetExeName);
    if (processId) {
        log::debug("Spotify process found (PID: " + std::to_string(processId) + ").");
        MuteAudioSessionByProcessId(processId, is_muted);
    }
}
#elif GEODE_IS_MACOS // mac os, wont work on android or ios :(
bool isApplicationRunning(const std::string& appName) {
    // Construct the osascript command
    std::string script = "osascript -e 'tell application \"System Events\" to (name of processes) contains \"" + appName + "\"'";
    
    // Open a pipe to execute the command and capture its output
    FILE* pipe = popen(script.c_str(), "r");
    if (!pipe) {
        log::error("Error: Failed to execute command.");
        return false;
    }

    // Read the command's output
    char buffer[128];
    std::string result = "";
    while (!feof(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
    }

    // Close the pipe
    pclose(pipe);

    // Check if the result contains "true" (application is running)
    return (result.find("true") != std::string::npos);
}
void MuteApplication() {
    std::string targetName = Mod::get()->getSettingValue<SettingAppStruct>("spotifyApp").m_application;
    // Remove the ".app" extension if it exists
    size_t dotAppPos = targetName.find(".app");
    if (dotAppPos != std::string::npos) {
        targetName.erase(dotAppPos, 4); // Remove the ".app" extension
    }
    log::info("Mute Application " + targetName);
    // chat jippity, applescript is amazing, i wish i could do this on windows instead of having to do all of this IMM device stuff
    std::string checkCommand = "osascript -e 'tell application \"System Events\" to (name of processes) contains \"" + targetName + "\"'";
    if (isApplicationRunning(targetName)) { // will show a popup, also listen i am not responsible if some malicious user does stuff here regarding applescript. please consider disabling all spotify options if you dont want to be vulnerable, then again this would only happen if the attacker were to modify the .json file, which wouldnt they inject their own dylib file anyways? tldr; you are responsible if you install malware onto your mac system ok
        std::string muteCommand = "osascript -e 'tell application \"" + targetName + "\" to set volume ";
        if (is_muted) {
            system((muteCommand + "100'").c_str());
            log::info(targetName + " has been unmuted.");
        } else {
            system((muteCommand + "0'").c_str());
            log::info(targetName + " has been muted.");
        }
    } else {
        log::info("Couldn't find application " + targetName + ", aborting.");
    }
}
void toggleSpotifyMute(bool automatic = false, bool muted = false) {
    if (!automatic) {
        is_muted = !is_muted;
    } else {
        is_muted = muted;
    }
    if (is_muted) {
        log::info("Muting Spotify...");
    } else {
        log::info("Unmuting Spotify...");
    }
    std::thread hThread(MuteApplication);
    hThread.detach();
}
#else // any other platform
void toggleSpotifyMute(bool automatic = false, bool muted = false) {
    // nothing because im not sure!
}
#endif
#ifndef GEODE_IS_ANDROID
class $modify(PlayLayer) {
    bool init(GJGameLevel* level, bool p1, bool p2) {
        if (!PlayLayer::init(level, p1, p2)) return false;
        if (!Mod::get()->getSettingValue<bool>("inLevelsSpotify")) return true;
        //auto gm = FMODAudioEngine::sharedEngine();
        //if (gm->m_backgroundMusicVolume > 0.0f) {
        // uncomment when fmod gets these fields 
        if (1.0F > 0.0F) {
            toggleSpotifyMute(true, true);
        }
        return true;
    }
    void onQuit() {
        PlayLayer::onQuit();
        if (!Mod::get()->getSettingValue<bool>("inLevelsSpotify")) return;
        if (is_muted) {
            toggleSpotifyMute(true, false);
        }
    }
};

class $modify(EditorUI) {
    void onPlaytest(CCObject* sender) {
        EditorUI::onPlaytest(sender);
        if (!Mod::get()->getSettingValue<bool>("inEditorSpotify")) return;
        auto gm = FMODAudioEngine::sharedEngine();
        //if (gm->m_backgroundMusicVolume > 0.0f) {
        if (1 > 0.0F) { // temp until someone adds fields
            toggleSpotifyMute();
        }
    }
    void onStopPlaytest(CCObject* sender) {
        EditorUI::onStopPlaytest(sender);
        if (!Mod::get()->getSettingValue<bool>("inEditorSpotify")) return;

        if (is_muted) {
            toggleSpotifyMute(true, false);
        }
    }
    void onPlayback(CCObject* sender) {
        EditorUI::onPlayback(sender);
        if (!Mod::get()->getSettingValue<bool>("inEditorSpotify")) return;

        auto gm = FMODAudioEngine::sharedEngine();
        //if (gm->m_backgroundMusicVolume > 0.0f) {
        if (1 > 0.0F) { // temp until someone adds fields
            toggleSpotifyMute();
        }
    }
    void onPause(CCObject* sender) {
        if (is_muted) {
            if (Mod::get()->getSettingValue<bool>("inEditorSpotify")) {
                toggleSpotifyMute(true, false);
            }
        }
        EditorUI::onPause(sender);
    }
};
#endif
// Spotify

// Utils
#include "BrownAlertDelegate.hpp"

// Function to convert a const char* to lowercase
const char* toLowerCase(const char* str) {
    char* result = new char[strlen(str) + 1];
    size_t i = 0;

    while (str[i]) {
        result[i] = std::tolower(str[i]);
        i++;
    }

    result[i] = '\0';
    return result;
}

class SearchUserLayer : public BrownAlertDelegate {
    protected:
        virtual void setup() {
            auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
            input_username->setPositionY(10);
            this->m_buttonMenu->addChild(input_username);
            auto validate_spr = ButtonSprite::create("Search", 60, true, "bigFont.fnt", "GJ_button_01.png", 30, .5F);
            auto validate_btn = CCMenuItemSpriteExtra::create(
                validate_spr,
                this,
                menu_selector(SearchUserLayer::onValidate)
            );

            validate_btn->setPosition({
                0,
                -35
            });
            this->m_buttonMenu->addChild(validate_btn, 1);
            this->m_mainLayer->addChild(this->m_buttonMenu);
            setTouchEnabled(true);
        }
        cocos2d::CCSize m_sScrLayerSize;
        void onClose(cocos2d::CCObject* pSender) {
            BrownAlertDelegate::onClose(pSender);
        }
        void onValidate(cocos2d::CCObject*);
        float m_fWidth = s_defWidth;
        float m_fHeight = s_defHeight;
    public:
        InputNode* input_username = InputNode::create(200.0F, "Username", "bigFont.fnt", "", 20);
        static constexpr const float s_defWidth = 260.0f;
        static constexpr const float s_defHeight = 120.0f;
        static SearchUserLayer* create() {
            auto pRet = new SearchUserLayer();
            if (pRet && pRet->init(SearchUserLayer::s_defWidth, SearchUserLayer::s_defHeight, "GJ_square01.png")) {
                pRet->autorelease();
                return pRet;
            }
            CC_SAFE_DELETE(pRet);
            return nullptr;
        }
};

class $modify(FriendPage, FriendsProfilePage) {
    bool init(UserListType type) {
        if (!FriendsProfilePage::init(type)) return false;
        if (!Mod::get()->getSettingValue<bool>("friendSearch")) return true;
        auto menu = this->m_buttonMenu;

        auto downSpr = CCSprite::createWithSpriteFrameName("edit_downBtn2_001.png");
        auto downBtn = CCMenuItemSpriteExtra::create(
            downSpr,
            this,
            menu_selector(FriendPage::onDown)
        );
        downBtn->setPosition(404, -160);
        menu->addChild(downBtn);

        auto upSpr = CCSprite::createWithSpriteFrameName("edit_upBtn2_001.png");
        auto upBtn = CCMenuItemSpriteExtra::create(
            upSpr,
            this,
            menu_selector(FriendPage::onUp)
        );
        upBtn->setPosition(404, -130);
        menu->addChild(upBtn);

        auto searchSpr = CCSprite::createWithSpriteFrameName("gj_findBtn_001.png");
        auto searchBtn = CCMenuItemSpriteExtra::create(
            searchSpr,
            this,
            menu_selector(FriendPage::onSearch)
        );
        searchBtn->setPosition(404, -88);
        menu->addChild(searchBtn);
        return true;
    }
    
    
    void onDown(CCObject*) {
        // JOUCA WHAT IS THIS LOL
        auto customList = reinterpret_cast<TableView*>(
            reinterpret_cast<CCLayer*>(
                reinterpret_cast<CCLayer*>(
                    reinterpret_cast<CCLayer*>(
                        this->getChildren()->objectAtIndex(0)
                    )->getChildren()->objectAtIndex(1)
                )->getChildren()->objectAtIndex(0)
            )->getChildren()->objectAtIndex(0)
        );

        customList->scrollLayer(200);
    }
    void onUp(CCObject*) {
        // NANI
        auto customList = reinterpret_cast<TableView*>(
            reinterpret_cast<CCLayer*>(
                reinterpret_cast<CCLayer*>(
                    reinterpret_cast<CCLayer*>(
                        this->getChildren()->objectAtIndex(0)
                    )->getChildren()->objectAtIndex(1)
                )->getChildren()->objectAtIndex(0)
            )->getChildren()->objectAtIndex(0)
        );
        customList->scrollLayer(-200);
    }
    void onSearch(CCObject*) {
        SearchUserLayer::create()->show();
    }
    static void searchUser(const char* username) {
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        auto sceneChildren = scene->getChildren();
        CCLayer* test1 = dynamic_cast<CCLayer*>(misc::findNode("FriendsProfilePage"));
        if (test1 == nullptr) {
            // safeguard from crashing
            FLAlertLayer::create(nullptr,
                "Error",
                "The mod could not find the <cy>FriendsProfilePage</c> layer. Please either <cg>try again later</c>, removing mods that may be interfering with the scene, or report this to the developers.",
                "OK",
                nullptr,
                350.0F
            )->show();
            return;
        }
        test1 = dynamic_cast<CCLayer*>(test1->getChildren()->objectAtIndex(0));
        auto test2 = static_cast<CCLayer*>(test1->getChildren()->objectAtIndex(1));
        auto test3 = static_cast<CCLayer*>(test2->getChildren()->objectAtIndex(0));
        if (test3->getChildrenCount() <= 0) {
            // another safeguard
            FLAlertLayer::create(nullptr,
                "Error",
                "You have <cy>no friends</c>!",
                "OK",
                nullptr,
                200.0F
            )->show();
            return;
        }
        auto customList = static_cast<TableView*>(test3->getChildren()->objectAtIndex(0));
        CCContentLayer* contentLayer = static_cast<CCContentLayer*>(
            customList->getChildren()->objectAtIndex(0)
        );
        int counter_page = 0;
        bool found = false;
        for (int i = 0; i < contentLayer->getChildrenCount(); i++) {
            CCMenu* cell;
            CCLabelBMFont* label;
            cell = dynamic_cast<CCMenu*>(
                reinterpret_cast<CCLayer*>(
                    reinterpret_cast<CCLayer*>(
                        reinterpret_cast<CCLayer*>(
                            contentLayer->getChildren()->objectAtIndex(i)
                        )
                    )->getChildren()->objectAtIndex(1)
                )->getChildren()->objectAtIndex(0)
            );

            if (cell == nullptr) {
                label = reinterpret_cast<CCLabelBMFont*>(
                    reinterpret_cast<CCLayer*>(
                        reinterpret_cast<CCLayer*>(
                            reinterpret_cast<CCLayer*>(
                                reinterpret_cast<CCLayer*>(
                                    reinterpret_cast<CCLayer*>(
                                        contentLayer->getChildren()->objectAtIndex(i)
                                    )
                                )->getChildren()->objectAtIndex(1)
                            )->getChildren()->objectAtIndex(1)
                        )->getChildren()->objectAtIndex(0)
                    )->getChildren()->objectAtIndex(0)
                );
            } else {
                label = reinterpret_cast<CCLabelBMFont*>(
                    reinterpret_cast<CCLayer*>(
                        reinterpret_cast<CCLayer*>(
                            cell->getChildren()->objectAtIndex(0)
                        )->getChildren()->objectAtIndex(0)
                    )
                );
            }
            const char* str1 = label->getString();
            if (strstr(toLowerCase(str1), toLowerCase(username)) != nullptr) {
                customList->scrollLayer(-9999999);
                customList->scrollLayer(counter_page);

                found = true;

                break;
            }
            counter_page += 45;
        }
        if (!found) {
            std::string str = username;
            FLAlertLayer::create(nullptr,
                "Error",
                "User not found : " + str,
                "OK",
                nullptr,
                200.0F
            )->show();
        }
    }
};

void SearchUserLayer::onValidate(CCObject* pSender) {
    FriendPage::searchUser(input_username->getString().c_str());
    BrownAlertDelegate::onClose(pSender);
}

class $modify(LeaderboardsLayer) {
    bool init(LeaderboardState state) {
        if (!LeaderboardsLayer::init(state)) return false;

        auto menu = CCMenu::create();

        auto plusSpr = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
        plusSpr->setScale(.8f);
        auto plusBtn = CCMenuItemSpriteExtra::create(
            plusSpr,
            this,
            menu_selector(MoreLeaderboards::onMoreLeaderboards)
        );
        plusBtn->setPosition(239, 20);
        menu->addChild(plusBtn);

        auto modsSpr = CCSprite::createWithSpriteFrameName("modBadge_01_001.png");
        modsSpr->setScale(1.5f);
        auto modsBtn = CCMenuItemSpriteExtra::create(
            modsSpr,
            this,
            menu_selector(MoreLeaderboards::onModsList)
        );
        modsBtn->setPosition(239, -24);
        menu->addChild(modsBtn);

        this->addChild(menu);

        return true;
    }
};

// Utils

// touhou time
//#ifdef GEODE_IS_WINDOWS
#include "BadApple.h"
std::string text_input = "";
bool bad_apple = false;

class $modify(SecretVault, SecretLayer2) {
    bool init() {
        if (!SecretLayer2::init()) return false;
        bad_apple = false;
        return true;
    }
    void onSubmit(CCObject* obj) {
        SecretLayer2::onSubmit(obj);
        CCLabelBMFont* vault_text = dynamic_cast<cocos2d::CCLabelBMFont*>(this->getChildren()->objectAtIndex(3));
        CCMenu* menu = nullptr;
        if (this->getChildrenCount() > 8) {
            menu = dynamic_cast<cocos2d::CCMenu*>(this->getChildren()->objectAtIndex(8)); //6 before
        } else {
            menu = dynamic_cast<cocos2d::CCMenu*>(this->getChildren()->objectAtIndex(6));
        }
        if (!strcmp(text_input.c_str(), "bad apple")) {
            if (!bad_apple) {
                ghc::filesystem::create_directory("gdutils");
                bad_apple = true;

                vault_text->setString("It's time... Touhou Fan");
                vault_text->setColor({ 255, 0, 0 });
                auto badappleSpr = CCSprite::createWithSpriteFrameName("GJ_musicOnBtn_001.png");
                auto badappleBtn = CCMenuItemSpriteExtra::create(
                    badappleSpr,
                    this,
                    menu_selector(SecretVault::onLaunchBadApple)
                );
                badappleBtn->setPosition(17, -89);
                menu->addChild(badappleBtn);

                auto badappleLabel = CCLabelBMFont::create("Bad Apple", "bigFont.fnt");
                badappleLabel->setScale(.4f);
                badappleLabel->setPosition(44, 237);
                this->addChild(badappleLabel);
            }
        }
    }
    void launchBadApple(CCObject* pSender) {
        //GameSoundManager::sharedManager()->stopBackgroundMusic();
        auto scene = cocos2d::CCScene::create();
        auto layer = BadApple::create();
        scene->addChild(layer);
        CCDirector::sharedDirector()->pushScene(scene); // cocos2d::CCTransitionFade::create(0.5f, scene)
    }

    void onLaunchBadApple(CCObject* pSender) {
        if (ghc::filesystem::exists("gdutils/bad_apple.mpg")) {
            auto node = CCNode::create();
            node->setTag(10);
            SecretVault::launchBadApple(node);
        } else {
            log::info("wait for bad apple");
            DownloadManager::create("https://clarifygdps.com/videos/bad_apple.mpg", "gdutils/bad_apple.mpg", menu_selector(SecretVault::launchBadApple))->show();
        }
    }

    void updateSearchLabel(const char* text) {
        if (strlen(text) > 0) {
            text_input = text;
        }
        SecretLayer2::updateSearchLabel(text);
    }
};
// touhou time

// demon list
//

std::unordered_map<int, int> demonListCache; // Will clear after game exit, or if user deletes level

// love url encoded characters :D
// also for some reason this is required on mac because Geode's web requests doesnt automatically append this for some reason.
// SURELY THIS WONT AFFECT LEVELS WITH SOME OTHER BREAKING SYMBOLS.
std::string url_encode(const std::string& value) {
    std::string encoded;
    encoded.reserve(value.size());
    for (char c : value) {
        if (c == ' ') {
            encoded += "%20";
        } else {
            encoded += c;
        }
    }
    return encoded;
}

class $modify(LevelInfoLayer) {
    // chat jippity
    void set(int key, int value) {
        demonListCache[key] = value;
    }

    // Function to check if a key exists in the cache.
    bool exists(int key) {
        return demonListCache.find(key) != demonListCache.end();
    }

    // Function to get a value from the cache.
    int get(int key) {
        auto it = demonListCache.find(key);
        if (it != demonListCache.end()) {
            return it->second;
        }
        // Return -1 or some other default value to indicate the key wasn't found.
        return -1;
    }

    bool remove(int key) {
        return demonListCache.erase(key) == 1;
    }
    float getScaleBasedPos(int pos) {
        if (pos > 0 && pos < 10) return 0.5F;
        if (pos > 10 && pos < 100) return 0.4F;
        if (pos > 100 && pos < 1000) return 0.3F;
        return 0.1F;
    }
    bool init(GJGameLevel* level, bool p1) { // inspiration le gdbrowser, what does p1 do? idk
        if (!LevelInfoLayer::init(level, p1)) return false;
        if (!Mod::get()->getSettingValue<bool>("demonListPlacement")) return true;
        
        if (level->m_demon.value() == 0 || level->m_stars.value() != 10) return true;
        if (level->m_demonDifficulty != 6) return true;
        
        auto director = CCDirector::sharedDirector();
        auto winSize = director->getWinSize();
        auto demonSpr = CCSprite::createWithSpriteFrameName("diffIcon_06_btn_001.png");
        //auto demonSpr = CCSprite::createWithSpriteFrameName("difficulty_06_btn_001.png");
        demonSpr->setPosition({(winSize.width / 2) - 85, (winSize.height / 2) + 18});
        demonSpr->setScale(0.5F);
        auto positionLabel = CCLabelBMFont::create("N/A", "bigFont.fnt");
        positionLabel->setPosition({(winSize.width / 2) - 107, (winSize.height / 2) + 18}); // 178
        positionLabel->setScale(0.5F);
        positionLabel->setVisible(false);
        demonSpr->setVisible(false);
        this->addChild(demonSpr);
        this->addChild(positionLabel);
        if (level->m_coins > 0) {
            positionLabel->setPositionY(positionLabel->getPositionY() - 18);
            demonSpr->setPositionY(demonSpr->getPositionY() - 18);
        }
        int levelID = level->m_levelID.value();
        if (exists(levelID)) {
            int position = get(levelID);
            if (position > 0) {
                positionLabel->setString(fmt::format("#{}", position).c_str());
                positionLabel->setScale(getScaleBasedPos(position));
                positionLabel->setVisible(true);
                demonSpr->setVisible(true);
            }
        } else {
            this->retain();
            auto loading_circle = LoadingCircle::create();
            if (level->m_coins > 0) {
                loading_circle->setPosition({-100, -2});
                loading_circle->setScale(0.3F);
            } else {
                loading_circle->setPosition({-100, 8});
                loading_circle->setScale(0.4F);
            }
            loading_circle->setParentLayer(this);
            loading_circle->show();
            log::info("Sending a request to pointercrate...");
            web::AsyncWebRequest()
                .fetch(fmt::format("https://pointercrate.com/api/v2/demons/listed/?name={}", url_encode(level->m_levelName).c_str()))
                .json()
                .then([this, level, levelID, loading_circle, positionLabel, demonSpr, winSize](matjson::Value const& json) {
                    if (loading_circle != nullptr) {
                        loading_circle->fadeAndRemove();
                    }
                    auto scene = CCDirector::sharedDirector()->getRunningScene();
                    //auto layer = misc::findNode("LevelInfoLayer");
                    //if (layer == nullptr) return;
                    if (json.dump() == "[]") { //idk how to check size, doing .count crashes
                        log::info("Level not found in pointercrate.");
                        this->release();
                    } else {
                        auto info = json.get<matjson::Value>(0);
                        auto position = info.get<int>("position");
                        positionLabel->setString(fmt::format("#{}", position).c_str());
                        positionLabel->setScale(getScaleBasedPos(position));
                        positionLabel->setVisible(true);
                        demonSpr->setVisible(true);
                        set(levelID, position);
                        log::info("Level found in Pointercrate! {} at #{}", level->m_levelName.c_str(), position);
                        this->release();
                    }
                })
                .expect([this, loading_circle](std::string const& error) {
                    if (loading_circle != nullptr) {
                        loading_circle->fadeAndRemove();
                    }
                    log::error("Error while sending a request to Pointercrate: {}", error);
                    FLAlertLayer::create(nullptr,
                        "Error",
                        "Failed to make a request to <cy>Pointercrate</c>. Please either <cg>try again later</c>, look at the error logs to see what might have happened, or report this to the developers.",
                        "OK",
                        nullptr,
                        350.0F
                    )->show();
                    this->release();
                });
        }
        
        return true;
    }
};
// demon list

// Copy button for sharing levels
class $modify(InfoLayer) {
    bool init(GJGameLevel* level, GJUserScore* userscore, GJLevelList* levellist) {
        if (!InfoLayer::init(level, userscore, levellist)) return false;

        // Check if it's InfoLayer for level
        if (level == nullptr) return true;

        std::string levelID = std::to_string(level->m_levelID.value());

        auto menu = this->m_buttonMenu;
        auto copySpr = CCSprite::createWithSpriteFrameName("GJ_undoBtn_001.png");
        copySpr->setScale(.7f);
        copySpr->setFlipX(true);
        auto copyBtn = CCMenuItemSpriteExtra::create(
            copySpr,
            this,
            menu_selector(InfoNewLayer::onCopyLevelURL)
        );
        copyBtn->setUserObject(cocos2d::CCString::create(gd::string(levelID)));
        copyBtn->setPosition(-195, -70);
        menu->addChild(copyBtn);
        return true;
    }
};

// Mod badges descriptions & GDUtils dev badge
class $modify(ProfilePage) {
    void loadPageFromUserInfo(GJUserScore* a2) {
        auto layer = m_mainLayer;

        ProfilePage::loadPageFromUserInfo(a2);

        // GDUtils dev badge
        if (layer) {
            std::vector<int> gdutils_accountID_devs = { 7026949, 6253758, 5509312 };
            if (std::find(gdutils_accountID_devs.begin(), gdutils_accountID_devs.end(), a2->m_accountID) != gdutils_accountID_devs.end()) {
                CCLabelBMFont* label = nullptr;
                CCObject* obj3 = nullptr;
                CCPoint* pos_label = nullptr;
                CCARRAY_FOREACH(layer->getChildren(), obj3) {
                    if (misc::getNodeName(obj3) != "cocos2d::CCLabelBMFont") continue;
                    auto label_ = static_cast<CCLabelBMFont*>(obj3);
                    if (label_ != nullptr) {
                        if (strcmp(label_->getString(), a2->m_userName.c_str()) == 0) {
                            label = label_;
                            pos_label = new CCPoint(label_->getPosition());
                            break;
                        }
                    }
                }

                if (label != nullptr) {
                    auto badgeGDUtil = CCSprite::create(Mod::get()->expandSpriteName("gdutils_badge.png"));
                    badgeGDUtil->setScale(.3f);
                    auto badgeGDUtilBtn = CCMenuItemSpriteExtra::create(
                        badgeGDUtil,
                        this,
                        menu_selector(NewProfilePage::onGDUtilsBadgePressed)
                    );
                    
                    badgeGDUtilBtn->setPosition({207.5f - (strlen(label->getString()) * (label->getScale() * 12)), -11});
                    auto menu = this->m_buttonMenu;
                    menu->addChild(badgeGDUtilBtn);
                }
            }
        }

        // mod description badge
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        if(layer) {
            // inspecting all children of the layer to find the badge
            CCSprite* badge = nullptr;
            CCObject* obj = nullptr;
            CCPoint pos = {0, 0};
            bool finished = false;
            CCARRAY_FOREACH(layer->getChildren(), obj) {
                if (misc::getNodeName(obj) != "cocos2d::CCSprite") continue;
                auto sprite = static_cast<CCSprite*>(obj);
                if (sprite == nullptr) continue;
                if (finished) continue;
                auto* cachedFrames = CCSpriteFrameCache::sharedSpriteFrameCache()->m_pSpriteFrames;
                const auto rect = sprite->getTextureRect();
                for (auto [key, frame] : CCDictionaryExt<std::string, CCSpriteFrame*>(cachedFrames)) {
                    if (frame->getTexture() == sprite->getTexture() && frame->getRect() == rect) {
                        if (key.starts_with("modBadge")) {
                            pos = sprite->getPosition();
                            badge = CCSprite::createWithSpriteFrame(frame);
                            sprite->removeFromParentAndCleanup(true);
                            finished = true;
                        }
                    }
                }
            }
            
            if (badge != nullptr) {
                CCMenu* menu = nullptr;
                CCObject* obj2 = nullptr;
                CCARRAY_FOREACH(layer->getChildren(), obj2) {
                    if (misc::getNodeName(obj2) != "cocos2d::CCMenu") continue;
                    menu = static_cast<CCMenu*>(obj2);
                    break;
                }

                auto badgeBtn = CCMenuItemSpriteExtra::create(
                    badge,
                    this,
                    menu_selector(NewProfilePage::onBadgePressed)
                );
                badgeBtn->setUserObject(a2);
                badgeBtn->setPosition({pos.x - menu->getPosition().x, pos.y - menu->getPosition().y});
                menu->addChild(badgeBtn);
            }
        }
    }
};

// When the socket connection is made
$on_mod(Loaded) {
    log::info("GDUtils Mod Loaded");
    current_socket = sio::socket::ptr();
    std::thread hThread(start_socket_func);
    hThread.detach();
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
    Mod::get()->addCustomSetting<SettingSectionValue>("credits-section", "none");
    // ok listen here, you dont provide a way to JUST get the file name, you provide the full path which isnt what i want, so dont complain geode devs. Okay? we good? ok
    #ifdef GEODE_IS_WINDOWS
    Mod::get()->addCustomSetting<SettingAppValue>("spotifyApp", "Spotify.exe");
    #else
    Mod::get()->addCustomSetting<SettingAppValue>("spotifyApp", "Spotify.app");
    #endif
    //Mod::get()->addCustomSetting<class T>(const std::string &key, "Spotify.exe");
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

    ghc::filesystem::create_directory("gdutils");
    if (!ghc::filesystem::exists("gdutils/gd_protocol.bat")) {
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

