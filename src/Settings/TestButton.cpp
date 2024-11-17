#include <Geode/Geode.hpp>
#include "../Notifications/EventsPush.h"

using namespace geode::prelude;

/*
    --- Test ---
*/

int cycleTypes = -1;

class SettingTestValue : public SettingV3 {
public:
    static Result<std::shared_ptr<SettingV3>> parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
        auto res = std::make_shared<SettingTestValue>();
        auto root = checkJson(json, "SettingTestValue");
        res->init(key, modID, root);
        res->parseNameAndDescription(root);
        res->parseEnableIf(root);
        root.checkUnknownKeys();
        return root.ok(std::static_pointer_cast<SettingV3>(res));
    }

    bool load(matjson::Value const& json) override {
        return true;
    }
    bool save(matjson::Value& json) const override {
        return true;
    }
    bool isDefaultValue() const override {
        return true;
    }
    void reset() override {}
    SettingNodeV3* createNode(float width) override;
};

class SettingTestNode : public SettingNodeV3 {
protected:
    bool init(std::shared_ptr<SettingTestValue> setting, float width) {
        if (!SettingNodeV3::init(setting, width))
            return false;
        this->setContentSize({ width, 35.f });
        auto label = CCLabelBMFont::create("Test", "bigFont.fnt");

        label->setScale(.6F);

        auto playSpr = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");

        playSpr->setScale(.4F);

        auto testBtn = CCMenuItemSpriteExtra::create(
            playSpr,
            this,
            menu_selector(SettingTestNode::onTestBtn)
        );

        getButtonMenu()->setContentWidth(width);
        getButtonMenu()->addChildAtPosition(label, Anchor::Left, {42, 0});
        getButtonMenu()->addChildAtPosition(testBtn, Anchor::Right, {-15, 0});
        getButtonMenu()->updateLayout();

        getNameLabel()->setVisible(false);

        return true;
    }
    void onCommit() override {}
    void onResetToDefault() override {}

public:
    void onTestBtn(CCObject*) {
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
    bool hasUncommittedChanges() const override {
        return false;
    }
    bool hasNonDefaultValue() const override {
        return false;
    }
    static SettingTestNode* create(std::shared_ptr<SettingTestValue> setting, float width) {
        auto ret = new SettingTestNode();
        if (ret && ret->init(setting, width)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
    std::shared_ptr<SettingTestValue> getSetting() const {
        return std::static_pointer_cast<SettingTestValue>(SettingNodeV3::getSetting());
    }
};

SettingNodeV3* SettingTestValue::createNode(float width) {
    return SettingTestNode::create(static_pointer_cast<SettingTestValue>(shared_from_this()), width);
}

$execute {
    (void)Mod::get()->registerCustomSettingType("test-notification", &SettingTestValue::parse);
}
