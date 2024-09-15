#include "CustomSettings.hpp"
#include <Geode/loader/Dirs.hpp>
#include "../Notifications/EventsPush.h"
#include "../includes.h"
int cycleTypes = -1;

SettingNodeV3* SettingTestValue::createNode(float width) {
    return SettingTestNode::create(static_pointer_cast<SettingTestValue>(shared_from_this()), width);
}
SettingNodeV3* SettingPosValue::createNode(float width) {
    return SettingPosNode::create(static_pointer_cast<SettingPosValue>(shared_from_this()), width);
}
SettingNodeV3* SettingCreditsValue::createNode(float width) {
    return SettingCreditsNode::create(static_pointer_cast<SettingCreditsValue>(shared_from_this()), width);
}

void SettingTestNode::onTestBtn(CCObject*) {
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    cycleTypes = cycleTypes + 1;
    if (cycleTypes > 4) cycleTypes = 0;
    bool newRate = Mod::get()->getSettingValue<bool>("newRate");
    bool daily = Mod::get()->getSettingValue<bool>("daily");
    bool weekly = Mod::get()->getSettingValue<bool>("weekly");
    if (!newRate && cycleTypes == 0) cycleTypes = 1;
    if (!daily && cycleTypes == 1) cycleTypes = 2;
    if (!weekly && cycleTypes == 2) cycleTypes = 3;
    if (!newRate && !daily && !weekly) {
        cycleTypes = 5;
    }

    sio::message::ptr data = sio::object_message::create();
    //{'demon': 1, 'type': 1, 'starsum': 6, 'stars': 10, 'rate': 1, 'title': 'BRAINPOWER Let the base kick!', 'level_name': 'O-oooooooooo AAAAE-A-A-I-A-U-', 'level_creator': ' JO-oooooooooooo AAE-O-A-A-U-U-A- E-eee-ee-eee AAAAE-A-E-I-E-A- JO-ooo-oo-oo-oo EEEEO-A-AAA-AAAA', 'sprite': 'GJ_square01.png'}
    data->get_map()["coins"] = sio::int_message::create(3);
    data->get_map()["verified_coins"] = sio::int_message::create(1);
    data->get_map()["platformer"] = sio::int_message::create(0);
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
            data->get_map()["rate"] = sio::int_message::create(1);
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
            data->get_map()["rate"] = sio::int_message::create(1);
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
            data->get_map()["rate"] = sio::int_message::create(1);
            break;
        case 3: // Legendary rating test
            data->get_map()["title"] = sio::string_message::create("New Rated Level !");
            data->get_map()["demon"] = sio::int_message::create(0);
            data->get_map()["type"] = sio::int_message::create(0);
            data->get_map()["stars"] = sio::int_message::create(3);
            data->get_map()["starsum"] = sio::int_message::create(20);
            data->get_map()["level_name"] = sio::string_message::create("Oh no it's happening");
            data->get_map()["level_creator"] = sio::string_message::create("by ...");
            data->get_map()["sprite"] = sio::string_message::create("GJ_square01.png");
            data->get_map()["rate"] = sio::int_message::create(3);
            break;
        case 4: // Mythic rating test
            data->get_map()["title"] = sio::string_message::create("New Rated Level !");
            data->get_map()["demon"] = sio::int_message::create(0);
            data->get_map()["type"] = sio::int_message::create(0);
            data->get_map()["stars"] = sio::int_message::create(100);
            data->get_map()["starsum"] = sio::int_message::create(20);
            data->get_map()["level_name"] = sio::string_message::create("FIRE IN THE HOLE!");
            data->get_map()["level_creator"] = sio::string_message::create("by GD Community");
            data->get_map()["sprite"] = sio::string_message::create("GJ_square01.png");
            data->get_map()["rate"] = sio::int_message::create(4);
            break;
        case 5: // in case the user disables everything
            data->get_map()["title"] = sio::string_message::create("New Rated Level !");
            data->get_map()["demon"] = sio::int_message::create(1);
            data->get_map()["type"] = sio::int_message::create(-1);
            data->get_map()["stars"] = sio::int_message::create(2);
            data->get_map()["starsum"] = sio::int_message::create(6);
            data->get_map()["level_name"] = sio::string_message::create("Back On Track");
            data->get_map()["level_creator"] = sio::string_message::create("by RobTop");
            data->get_map()["sprite"] = sio::string_message::create("GJ_square01.png");
            data->get_map()["rate"] = sio::int_message::create(1);
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
