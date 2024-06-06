// Spotify
bool is_muted = false;
#include "../includes.h"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include "../Settings/CustomSettings.hpp"
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
    std::wstring targetExeName = StringToWstring(Mod::get()->getSettingValue<SettingAppStruct>("spotifyApp").m_application);
    DWORD processId = GetProcessIdByName(targetExeName);
    if (processId) {
        MuteAudioSessionByProcessId(processId, is_muted);
    }
}
#elif defined(GEODE_IS_MACOS) // mac os, wont work on android or ios :(
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
#ifdef GEODE_IS_DESKTOP 
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
