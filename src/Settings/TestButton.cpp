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

        EventData data;
        data.type = EventType::Rate;
        data.rate = 1;
        data.coins = 3;
        data.verified_coins = true;
        data.level_creator = "by RobTop";
        //{'demon': 1, 'type': 1, 'starsum': 6, 'stars': 10, 'rate': 1, 'title': 'BRAINPOWER Let the base kick!', 'level_name': 'O-oooooooooo AAAAE-A-A-I-A-U-', 'level_creator': ' JO-oooooooooooo AAE-O-A-A-U-U-A- E-eee-ee-eee AAAAE-A-E-I-E-A- JO-ooo-oo-oo-oo EEEEO-A-AAA-AAAA', 'sprite': 'GJ_square01.png'}
        switch (cycleTypes) {
            case 0: // new rate
                data.title = "New Rated Level !";
                data.stars = 1;
                data.starsum = 10;
                data.level_name = "Stereo Madness";
                break;
            case 1: // daily
                data.title = "New Daily Level !";
                data.type = EventType::Daily;
                data.stars = 9;
                data.starsum = 40;
                data.level_name = "Cycles";
                break;
            case 2: // weekly
                data.title = "New Weekly Level !";
                data.demon = true;
                data.type = EventType::Weekly;
                data.stars = 14;
                data.starsum = 7;
                data.level_name = "Theory of Everything 2";
                data.sprite = "GJ_square05.png";
                break;
            case 3: // Legendary rating test
                data.title = "New Rated Level !";
                data.stars = 3;
                data.starsum = 20;
                data.level_name = "Oh no it's happening";
                data.level_creator = "by ...";
                data.rate = 3;
                break;
            case 4: // Mythic rating test
                data.title = "New Rated Level !";
                data.stars = 100;
                data.starsum = 20;
                data.type = EventType::Event;
                data.level_name = "FIRE IN THE HOLE!";
                data.level_creator = "by GD Community";
                data.rate = 4;
                break;
            case 5: // in case the user disables everything
                data.title = "New Rated Level !";
                data.demon = true;
                data.stars = 2;
                data.starsum = 6;
                data.level_name = "Back On Track";
                data.rate = 1;
                break;
            default: // not possible
                data.title = "hi there hxd user, ghidra user or ida user. (or hi RobTop) Here's a little easter egg for you :)";
                data.demon = true;
                data.stars = 14;
                data.starsum = 7;
                data.sprite = "This will crash! :D";
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
