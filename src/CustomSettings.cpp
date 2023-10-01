#include "CustomSettings.hpp"
#include <Geode/loader/Dirs.hpp>
#include <filesystem>
int cycleTypes = -1;

SettingNode* SettingSectionValue::createNode(float width) {
    return SettingSectionNode::create(this, width);
}
SettingNode* SettingTestValue::createNode(float width) {
    return SettingTestNode::create(this, width);
}
SettingNode* SettingAppValue::createNode(float width) {
    return SettingAppNode::create(this, width);
}
SettingNode* SettingPosValue::createNode(float width) {
    return SettingPosNode::create(this, width);
}
SettingNode* SettingCreditsValue::createNode(float width) {
    return SettingCreditsNode::create(this, width);
}
#ifdef GEODE_IS_WINDOWS
std::string GetOpenFileName() {
    OPENFILENAME ofn;
    char szFile[MAX_PATH] = { 0 };

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "Executable Files (*.exe)\0*.exe\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrInitialDir = "\%appdata\%\\Spotify"; // Set your desired default path here
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    if (GetOpenFileName(&ofn))
        return std::string(szFile);
    else
        return "";
    return "";
}
#endif
void SettingAppNode::onPickFile(CCObject*) {
    #ifdef GEODE_IS_MACOS
    if (auto path = file::pickFile(
        file::PickMode::OpenFile,
        {
            "/Applications", // sorry mac users but idk the path for spotify
            {}
        }
    )) {
        std::string strPath = path.unwrap().string();
        size_t lastBackslashPos = strPath.find_last_of('/');
        if (lastBackslashPos != std::string::npos) {
            std::string lastPart = strPath.substr(lastBackslashPos + 1);
            m_currentApp = lastPart;
            defaultApp_input->setString(lastPart.c_str());
            this->dispatchChanged();
        }
    }
    #else
    std::string filePath = GetOpenFileName();
    if (!filePath.empty()) {
        std::filesystem::path fullPath(filePath);
        std::string appName = fullPath.filename().string();
        if (!appName.empty()) {
            m_currentApp = appName;
            defaultApp_input->setString(appName.c_str());
            this->dispatchChanged();
        }
    }
    #endif
}

void SettingTestNode::onTestBtn(CCObject*) {
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    cycleTypes = cycleTypes + 1;
    if (cycleTypes > 2) cycleTypes = 0;
    bool newRate = Mod::get()->getSettingValue<bool>("newRate");
    bool daily = Mod::get()->getSettingValue<bool>("daily");
    bool weekly = Mod::get()->getSettingValue<bool>("weekly");
    if (!newRate && cycleTypes == 0) cycleTypes = 1;
    if (!daily && cycleTypes == 1) cycleTypes = 2;
    if (!weekly && cycleTypes == 2) cycleTypes = 0;
    if (!newRate && !daily && !weekly) {
        cycleTypes = 3;
    }

    sio::message::ptr data = sio::object_message::create();
    //{'demon': 1, 'type': 1, 'starsum': 6, 'stars': 10, 'rate': 1, 'title': 'BRAINPOWER Let the base kick!', 'level_name': 'O-oooooooooo AAAAE-A-A-I-A-U-', 'level_creator': ' JO-oooooooooooo AAE-O-A-A-U-U-A- E-eee-ee-eee AAAAE-A-E-I-E-A- JO-ooo-oo-oo-oo EEEEO-A-AAA-AAAA', 'sprite': 'GJ_square01.png'}
    data->get_map()["rate"] = sio::int_message::create(1);
    data->get_map()["coins"] = sio::int_message::create(3);
    data->get_map()["verified_coins"] = sio::int_message::create(1);
    switch (cycleTypes) {
        case 0: // new rate
            data->get_map()["title"] = sio::string_message::create("New Rated Level !");
            data->get_map()["demon"] = sio::int_message::create(0);
            data->get_map()["type"] = sio::int_message::create(0);
            data->get_map()["stars"] = sio::int_message::create(1);
            data->get_map()["starsum"] = sio::int_message::create(10);
            data->get_map()["level_name"] = sio::string_message::create("Stereo Madness");
            data->get_map()["level_creator"] = sio::string_message::create("by RobTop");
            data->get_map()["sprite"] = sio::string_message::create("GJ_square01.png");
            break;
        case 1: // daily
            data->get_map()["title"] = sio::string_message::create("New Daily Level !");
            data->get_map()["demon"] = sio::int_message::create(0);
            data->get_map()["type"] = sio::int_message::create(1);
            data->get_map()["stars"] = sio::int_message::create(9);
            data->get_map()["starsum"] = sio::int_message::create(40);
            data->get_map()["level_name"] = sio::string_message::create("Cycles");
            data->get_map()["level_creator"] = sio::string_message::create("by RobTop");
            data->get_map()["sprite"] = sio::string_message::create("GJ_square01.png");
            break;
        case 2: // weekly
            data->get_map()["title"] = sio::string_message::create("New Weekly Level !");
            data->get_map()["demon"] = sio::int_message::create(1);
            data->get_map()["type"] = sio::int_message::create(2);
            data->get_map()["stars"] = sio::int_message::create(14);
            data->get_map()["starsum"] = sio::int_message::create(7);
            data->get_map()["level_name"] = sio::string_message::create("Theory of Everything 2");
            data->get_map()["level_creator"] = sio::string_message::create("by RobTop");
            data->get_map()["sprite"] = sio::string_message::create("GJ_square05.png");
            break;
        case 3: // in case the user disables everything
            data->get_map()["title"] = sio::string_message::create("New Rated Level !");
            data->get_map()["demon"] = sio::int_message::create(1);
            data->get_map()["type"] = sio::int_message::create(-1);
            data->get_map()["stars"] = sio::int_message::create(2);
            data->get_map()["starsum"] = sio::int_message::create(6);
            data->get_map()["level_name"] = sio::string_message::create("Back On Track");
            data->get_map()["level_creator"] = sio::string_message::create("by RobTop");
            data->get_map()["sprite"] = sio::string_message::create("GJ_square01.png");
            break;
        default: // not possible
            data->get_map()["title"] = sio::string_message::create("hi there hxd user, ghidra user or ida user. (or hi RobTop) Here's a little easter egg for you :)");
            data->get_map()["demon"] = sio::int_message::create(1);
            data->get_map()["type"] = sio::int_message::create(2);
            data->get_map()["stars"] = sio::int_message::create(14);
            data->get_map()["starsum"] = sio::int_message::create(7);
            data->get_map()["sprite"] = sio::string_message::create("This will crash! :D");
            
            break;
    }
    EventsPush::pushRateLevel(scene, data);

}
