// GDUtils Geode Port
// Ported with help by Firee
// Mod made by Jouca & Firee

#include <Geode/Geode.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/FriendsProfilePage.hpp>
#include <Geode/modify/SecretLayer2.hpp>
#include <Geode/loader/Log.hpp>
#include "includes.h"
#include "CustomSettings.hpp"
#include "EventsPush.h"
#include "ProcessLambdas.h"
#ifdef GEODE_IS_WINDOWS
#include "DownloadManager.h"
#else // mac
#include <pthread.h>
#include <ctime>
#include <unistd.h>
#include <cstdlib> // ADD THIS
#endif
#include <queue>

using namespace geode::prelude;

int reconnectionDelay = 1000;
int reconnectionDelayMax = 5000;
int reconnectionAttempts = 0;

bool still_connected = false;
bool connect_finish = false;

bool event_fired = false;

std::queue<sio::message::ptr> dataQueue;

sio::message::ptr event_data = nullptr;
std::mutex lock;
std::unique_lock<std::mutex> unique_lock(lock);
std::condition_variable cond;
#ifdef GEODE_IS_WINDOWS
HANDLE hThread;
#else
pthread_t hThread;
#endif
sio::socket::ptr current_socket;

namespace ConnectionHandler {
    void onSuccess() {
        log::info("Socket Connection successful!");
        connect_finish = true;
        still_connected = true;
        cond.notify_all();
    }

    void onClose(sio::client::close_reason const& reason) {
        log::warn("Connection closed: " + std::to_string(reason));
        still_connected = false;
    }

    void onFail() {
        log::error("Connection failed.");
        still_connected = false;
    }

    void onError(sio::message::ptr const& message) {
        log::error("Sock Error: " + message->get_string());
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
    sio::message::ptr data = sio::object_message::create();
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
    while (true) {
        #ifdef GEODE_IS_WINDOWS
        Sleep(1000); // this code is for some reason the reason why the socket client still runs, even though it looks very wrong
        #else 
        sleep(1); // yeah sorry
        #endif
        if (!still_connected) {
            log::info("not connected, restarting...");
            start_socket_func();
            break;
        }
    }
}
#ifdef GEODE_IS_WINDOWS
DWORD WINAPI start_socket(void* self) {
    start_socket_func();
    return true;
}
#else
void* start_socket(void* self) {
    start_socket_func();
    return NULL;
}
#endif

std::string currentLayer = "";
void processEvent(CCScene* self) {
    if (!dataQueue.empty()) {
        log::debug("process event");
        auto data = dataQueue.front();
        dataQueue.pop();
        EventsPush::pushRateLevel(self, data);
    }
}

class $modify(GameManager) {
    void update(float dt) {
        GameManager::update(dt);
        bool waitUntilExit = false;
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        if (scene->getChildrenCount() == 0) return;
        auto layer = scene->getChildren()->objectAtIndex(0);
        if (layer == nullptr) return;
        if (ProcessLambdas::shouldProcessMenuHandler()) {
            ProcessLambdas::processMenuHandler();
        }
        std::string layerName = typeid(*layer).name() + 6;
        if (layerName == "cocos2d::CCLayerColor") return;
        if (currentLayer != layerName) {
            //log::debug("GameManager::update - " + layerName);
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
#else // mac os, wont work on android or ios :(
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
void* MuteApplication(void* self) {
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
    pthread_t thread;
    int threadError;
    // Create a thread to check if the application is running
    threadError = pthread_create(&thread, NULL, MuteApplication, NULL);
    if (threadError) {
        log::error("Error creating thread: " + threadError);
    }
}
#endif
class $modify(PlayLayer) {
    bool init(GJGameLevel* level) {
        if (!PlayLayer::init(level)) return false;
        if (!Mod::get()->getSettingValue<bool>("inLevelsSpotify")) return true;
        auto gm = FMODAudioEngine::sharedEngine();
        if (gm->m_backgroundMusicVolume > 0.0f) {
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
        if (gm->m_backgroundMusicVolume > 0.0f) {
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
        if (gm->m_backgroundMusicVolume > 0.0f) {
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

            /*input_username->setPosition(
                winSize.width / 2,
                winSize.height / 2 + 10
            );*/
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
    public:
    static void searchUser(const char* username) {
        auto self = reinterpret_cast<CCLayer*>(
            CCDirector::sharedDirector()->getRunningScene()->getChildren()->objectAtIndex(1)
        );

        auto customList = reinterpret_cast<TableView*>(
            reinterpret_cast<CCLayer*>(
                reinterpret_cast<CCLayer*>(
                    reinterpret_cast<CCLayer*>(
                        self->getChildren()->objectAtIndex(0)
                    )->getChildren()->objectAtIndex(1)
                )->getChildren()->objectAtIndex(0)
            )->getChildren()->objectAtIndex(0)
        );

        CCContentLayer* contentLayer = reinterpret_cast<CCContentLayer*>(
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

            if (strcmp(toLowerCase(str1), toLowerCase(username)) == 0) {
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
    FriendPage::searchUser(input_username->getString());

    BrownAlertDelegate::onClose(pSender);
}

// Utils

// touhou time
#ifdef GEODE_IS_WINDOWS
#include "BadApple.h"
std::string text_input = "";
bool bad_apple = false;

class $modify(SecretVault, SecretLayer2) {
    bool init() {
        if (!SecretLayer2::init()) return false;
        bad_apple = false;
        return true;
    }
    bool onSubmit(CCObject* obj) {
        if (!SecretLayer2::onSubmit(obj)) return false;
        CCLabelBMFont* vault_text = dynamic_cast<cocos2d::CCLabelBMFont*>(this->getChildren()->objectAtIndex(3));
        CCMenu* menu = nullptr;
        if (this->getChildrenCount() > 8) {
            menu = dynamic_cast<cocos2d::CCMenu*>(this->getChildren()->objectAtIndex(8)); //6 before
        } else {
            menu = dynamic_cast<cocos2d::CCMenu*>(this->getChildren()->objectAtIndex(6));
        }
        if (!strcmp(text_input.c_str(), "bad apple")) {
            if (!bad_apple) {
                std::filesystem::create_directory("gdutils");
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

        return true;
    }
    void launchBadApple(CCObject* pSender) {
        GameSoundManager::sharedManager()->stopBackgroundMusic();
        auto scene = cocos2d::CCScene::create();
        auto layer = BadApple::create();
        scene->addChild(layer);
        CCDirector::sharedDirector()->pushScene(scene); // cocos2d::CCTransitionFade::create(0.5f, scene)
    }

    void onLaunchBadApple(CCObject* pSender) {
        if (std::filesystem::exists("gdutils/bad_apple.mpg")) {
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
#endif
// touhou time

// When the socket connection is made
$on_mod(Loaded) {
    log::info("GDUtils Mod Loaded");
    current_socket = sio::socket::ptr();
    #ifdef GEODE_IS_WINDOWS
    hThread = CreateThread(NULL, 0, start_socket, NULL, 0, NULL);
    #else // mac
    pthread_create(&hThread, NULL, start_socket, NULL);
    #endif
    
    Mod::get()->addCustomSetting<SettingTestValue>("test-notification", "none");
    Mod::get()->addCustomSetting<SettingPosValue>("notificationPlacement", 4);
    // Sections
    Mod::get()->addCustomSetting<SettingSectionValue>("notification-section", "none");
    Mod::get()->addCustomSetting<SettingSectionValue>("notification-placement-section", "none");
    Mod::get()->addCustomSetting<SettingSectionValue>("notification-appearance-section", "none");
    Mod::get()->addCustomSetting<SettingSectionValue>("spotify-section", "none");
    Mod::get()->addCustomSetting<SettingSectionValue>("credits-section", "none");
    // ok listen here, you dont provide a way to JUST get the file name, you provide the full path which isnt what i want, so dont complain geode devs. Okay? we good? ok
    #ifdef GEODE_IS_WINDOWS
    Mod::get()->addCustomSetting<SettingAppValue>("spotifyApp", "Spotify.exe");
    #else
    Mod::get()->addCustomSetting<SettingAppValue>("spotifyApp", "Spotify.app");
    #endif
    //Mod::get()->addCustomSetting<class T>(const std::string &key, "Spotify.exe");
    Mod::get()->addCustomSetting<SettingCreditsValue>("credit-buttons", "none");
}

//#include "SecretVault.h"

$execute {
    /*Mod::get()->addHook(
        reinterpret_cast<void*>(geode::base::get() + 0x221ac0),
        &SecretVault::hook,
        "SecretVault::init",
        tulip::hook::TulipConvention::Thiscall
    );
    
    Mod::get()->addHook(
        reinterpret_cast<void*>(geode::base::get() + 0x222FC0),
        &SecretVault::hookupdateSearchLabel,
        "SecretVault::updateSearchLabel",
        tulip::hook::TulipConvention::Thiscall
    );*/
}
