#pragma once
#include <Geode/loader/SettingNode.hpp>
#include "CreditsBetaMenu.h"
#include "CreditsMenu.h"
#include <Geode/ui/TextInput.hpp>
using namespace geode::prelude;

/*
    --- Test ---
*/

class SettingTestValue : public SettingV3 {
public:
    static Result<std::shared_ptr<SettingTestValue>> parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
        auto res = std::make_shared<SettingTestValue>();
        auto root = checkJson(json, "SettingTestValue");
        res->init(key, modID, root);
        res->parseNameAndDescription(root);
        root.checkUnknownKeys();
        return root.ok(res);
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
    void onTestBtn(CCObject*);
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
};

/*
    Notification Position
*/

const int DEFAULT_POS = 4;

enum class SettingPosEnum : int {
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight
};

template <>
struct matjson::Serialize<SettingPosEnum> {
    static matjson::Value to_json(SettingPosEnum const& value) {
        log::info("to_json {}", static_cast<int>(value));
        switch (value) {
            case SettingPosEnum::TopLeft:
                return 1;
            case SettingPosEnum::TopRight:
                return 2;
            case SettingPosEnum::BottomLeft:
                return 3;
            default:
            case SettingPosEnum::BottomRight:
                return 4;
        }
    }
    static SettingPosEnum from_json(matjson::Value const& value) {
        switch (value.as_int()) {
            case 1: return SettingPosEnum::TopLeft;
            case 2: return SettingPosEnum::TopRight;
            case 3: return SettingPosEnum::BottomLeft;
            case 4: return SettingPosEnum::BottomRight;
            default: throw matjson::JsonException(fmt::format("invalid SettingPosEnum value '{}'", value));
        }
    }
    static bool is_json(matjson::Value const& json) {
        return json.is_number() || json.is_string();
    }
};

class SettingPosValue : public SettingBaseValueV3<SettingPosEnum> {
public:
    static Result<std::shared_ptr<SettingPosValue>> parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
        auto res = std::make_shared<SettingPosValue>();
        auto root = checkJson(json, "SettingPosValue");
        res->parseBaseProperties(key, modID, root);
        root.checkUnknownKeys();
        return root.ok(res);
    }
    SettingNodeV3* createNode(float width) override;
};

template <>
struct geode::SettingTypeForValueType<SettingPosEnum> {
    using SettingType = SettingPosValue;
};

class SettingPosNode : public SettingValueNodeV3<SettingPosValue> {
protected:
    CCMenuItemToggler* tlBtn;
    CCMenuItemToggler* trBtn;
    CCMenuItemToggler* blBtn;
    CCMenuItemToggler* brBtn;

    int getActiveCornerTag(SettingPosEnum corner) {
        switch (corner) {
            case SettingPosEnum::TopLeft: // Top Left
                return 2004;
            case SettingPosEnum::TopRight: // Top Right
                return 2005;
            case SettingPosEnum::BottomLeft: // Bottom Left
                return 2006;
            case SettingPosEnum::BottomRight: // Bottom Right
            default:
                return 2007;
        }
    }
    SettingPosEnum tagToCorner(int tag) {
        switch (tag) {
            case 2004: // Top Left
                return SettingPosEnum::TopLeft;
            case 2005: // Top Right
                return SettingPosEnum::TopRight;
            case 2006: // Bottom Left
                return SettingPosEnum::BottomLeft;
            default:
            case 2007: // Bottom Right
                return SettingPosEnum::BottomRight;
        }
    }

    bool init(std::shared_ptr<SettingPosValue> setting, float width) {
        if (!SettingValueNodeV3::init(setting, width))
            return false;
        this->setContentSize({ width, 70.f });
        CCSprite* toggleOn = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        CCSprite* toggleOff = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        toggleOn->setScale(.7F);
        toggleOff->setScale(.7F);
        tlBtn = CCMenuItemToggler::create(
            toggleOn,
            toggleOff,
            this,
            menu_selector(SettingPosNode::onCornerClick)
        );
        trBtn = CCMenuItemToggler::create(
            toggleOn,
            toggleOff,
            this,
            menu_selector(SettingPosNode::onCornerClick)
        );
        blBtn = CCMenuItemToggler::create(
            toggleOn,
            toggleOff,
            this,
            menu_selector(SettingPosNode::onCornerClick)
        );
        brBtn = CCMenuItemToggler::create(
            toggleOn,
            toggleOff,
            this,
            menu_selector(SettingPosNode::onCornerClick)
        );
        tlBtn->setTag(getActiveCornerTag(SettingPosEnum::TopLeft));
        trBtn->setTag(getActiveCornerTag(SettingPosEnum::TopRight));
        blBtn->setTag(getActiveCornerTag(SettingPosEnum::BottomLeft));
        brBtn->setTag(getActiveCornerTag(SettingPosEnum::BottomRight));
        getButtonMenu()->setContentSize({width / 2, 45.F});
        getButtonMenu()->addChildAtPosition(tlBtn, Anchor::TopLeft);
        getButtonMenu()->addChildAtPosition(trBtn, Anchor::TopRight);
        getButtonMenu()->addChildAtPosition(blBtn, Anchor::BottomLeft);
        getButtonMenu()->addChildAtPosition(brBtn, Anchor::BottomRight);
        getButtonMenu()->updateLayout();
        getButtonMenu()->setAnchorPoint({0.5, 0.5});
        getButtonMenu()->updateAnchoredPosition(Anchor::Center, {0, 0});
        this->getNameLabel()->setVisible(false);
        auto currentCorner = static_cast<SettingPosEnum>(this->getValue());
        tlBtn->toggle(!(tlBtn->getTag() == getActiveCornerTag(currentCorner)));
        trBtn->toggle(!(trBtn->getTag() == getActiveCornerTag(currentCorner)));
        blBtn->toggle(!(blBtn->getTag() == getActiveCornerTag(currentCorner)));
        brBtn->toggle(!(brBtn->getTag() == getActiveCornerTag(currentCorner)));
        this->updateState(nullptr);
        return true;
    }
    
    void updateState(CCNode* invoker) override {
        SettingValueNodeV3::updateState(invoker);
        auto currentCorner = static_cast<SettingPosEnum>(this->getValue());
    }
    void onCornerClick(CCObject* sender) {
        tlBtn->toggle(true);
        trBtn->toggle(true);
        blBtn->toggle(true);
        brBtn->toggle(true);
        SettingPosEnum value = tagToCorner(sender->getTag());
        this->setValue(value, static_cast<CCNode*>(sender));
    };
    void onResetToDefault() override {
        tlBtn->toggle(true);
        trBtn->toggle(true);
        blBtn->toggle(true);
        brBtn->toggle(false);
        this->setValue(SettingPosEnum::BottomRight, nullptr);
    }
public:
    static SettingPosNode* create(std::shared_ptr<SettingPosValue> setting, float width) {
        auto ret = new SettingPosNode();
        if (ret && ret->init(setting, width)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

/*
    Credits
*/

class SettingCreditsValue : public SettingV3 {
public:
    static Result<std::shared_ptr<SettingCreditsValue>> parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
        auto res = std::make_shared<SettingCreditsValue>();
        auto root = checkJson(json, "SettingCreditsValue");
        res->init(key, modID, root);
        res->parseNameAndDescription(root);
        root.checkUnknownKeys();
        return root.ok(res);
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


class SettingCreditsNode : public SettingNodeV3 {
protected:
    bool init(std::shared_ptr<SettingCreditsValue> setting, float width) {
        if (!SettingNodeV3::init(setting, width))
            return false;
        this->setContentSize({ width, 40.f });
        auto creditsSpr = ButtonSprite::create("Credits", "bigFont.fnt", "GJ_button_01.png");
        creditsSpr->setScale(.6F);
        auto creditsBtn = CCMenuItemSpriteExtra::create(
            creditsSpr,
            this,
            menu_selector(SettingCreditsNode::onCreditsBtn)
        );
        auto betaTestersSpr = ButtonSprite::create("Beta Testers", "bigFont.fnt", "GJ_button_01.png");
        betaTestersSpr->setScale(.6F);
        auto betaTestersBtn = CCMenuItemSpriteExtra::create(
            betaTestersSpr,
            this,
            menu_selector(SettingCreditsNode::onBetaBtn)
        );
        getButtonMenu()->setContentWidth(width);
        getButtonMenu()->addChildAtPosition(creditsBtn, Anchor::Center, {-55, 0});
        getButtonMenu()->addChildAtPosition(betaTestersBtn, Anchor::Center, {85, 0});
        getButtonMenu()->updateLayout();
        getNameLabel()->setVisible(false);
        return true;
    }
    void onCommit() override {}
    void onResetToDefault() override {}

public:
    void onCreditsBtn(CCObject*) {
        CreditsMenu::create()->show();
    }
    void onBetaBtn(CCObject*) {
        CreditsBetaMenu::create()->show();
    }

    bool hasUncommittedChanges() const override {
        return false;
    }
    bool hasNonDefaultValue() const override {
        return false;
    }
    static SettingCreditsNode* create(std::shared_ptr<SettingCreditsValue> setting, float width) {
        auto ret = new SettingCreditsNode();
        if (ret && ret->init(setting, width)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};
