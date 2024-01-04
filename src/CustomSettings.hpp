#pragma once
#include <Geode/loader/SettingNode.hpp>
#include "CreditsBetaMenu.h"
#include "CreditsMenu.h"
#include "EventsPush.h"
#include "includes.h"
using namespace geode::prelude;

/*
    --- Test ---
*/

class SettingTestValue;

class SettingTestValue : public SettingValue {
protected:
    std::string m_placeholder;
public:
    SettingTestValue(std::string const& key, std::string const& modID, std::string const& placeholder)
      : SettingValue(key, modID), m_placeholder(placeholder) {}

    bool load(matjson::Value const& json) override {
        return true;
    }
    bool save(matjson::Value& json) const override {
        return true;
    }
    SettingNode* createNode(float width) override;
};


class SettingTestNode : public SettingNode {
protected:
 bool init(SettingTestValue* value, float width) {
        if (!SettingNode::init(value))
            return false;
        this->setContentSize({ width, 35.f });
        auto menu = CCMenu::create();
        auto label = CCLabelBMFont::create("Test", "bigFont.fnt");
        label->setScale(.6F);
        label->setPositionX(-130);
        auto playSpr = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
        playSpr->setScale(.4F);
        //auto infoBtn
        auto testBtn = CCMenuItemSpriteExtra::create(
            playSpr,
            this,
            menu_selector(SettingTestNode::onTestBtn)
        );
        testBtn->setPositionX(140);
        menu->setPosition(width / 2, 18.f);
        menu->addChild(label);
        menu->addChild(testBtn);
        this->addChild(menu);
        return true;
    }

public:
    // because for some reason SettingNode doesnt expose description, i have to do all of this, smh.
    void onTestBtn(CCObject*);

    void commit() override {
        // Let the UI know you have committed the value
        this->dispatchCommitted();
    }

    // Geode calls this to query if the setting value has been changed, 
    // and those changes haven't been committed
    bool hasUncommittedChanges() override {
        return false;
    }

    // Geode calls this to query if the setting has a value that is 
    // different from its default value
    bool hasNonDefaultValue() override {
        return true;
    }

    // Geode calls this to reset the setting's value back to default
    void resetToDefault() override {

    }
    static SettingTestNode* create(SettingTestValue* value, float width) {
        auto ret = new SettingTestNode;
        if (ret && ret->init(value, width)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

/*
    --- Section ---
*/

class SettingSectionValue;

class SettingSectionValue : public SettingValue {
protected:
    std::string m_placeholder;
public:
    SettingSectionValue(std::string const& key, std::string const& modID, std::string const& placeholder)
      : SettingValue(key, modID), m_placeholder(placeholder) {}

    bool load(matjson::Value const& json) override {
        return true;
    }
    bool save(matjson::Value& json) const override {
        return true;
    }
    SettingNode* createNode(float width) override;
};

class SettingSectionNode : public SettingNode {
protected:
    bool init(SettingSectionValue* value, float width) {
        if (!SettingNode::init(value))
            return false;
        this->setContentSize({ width, 40.f });
        auto menu = CCMenu::create();
        std::string sectionName = Mod::get()->getSettingDefinition(value->getKey())->get<CustomSetting>()->json->get<std::string>("name");
        auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        infoSpr->setScale(.7F);
        //auto infoBtn
        auto infoBtn = CCMenuItemSpriteExtra::create(
            infoSpr,
            this,
            menu_selector(SettingSectionNode::onInfoBtn)
        );
        infoBtn->setID(value->getKey());
        infoBtn->setPositionX(Mod::get()->getSettingDefinition(value->getKey())->get<CustomSetting>()->json->get<int>("posX"));
        auto label = CCLabelBMFont::create(sectionName.c_str(), "goldFont.fnt");
        
        label->setScale(.6F);
        menu->setPosition(width / 2, 23.f);
        menu->addChild(label);
        menu->addChild(infoBtn);
        this->addChild(menu);
        return true;
    }

    void onInfoBtn(CCObject* sender) {
        // i dont want to deal with template hell
        auto node = reinterpret_cast<CCMenuItemSpriteExtra*>(sender);
        if (node == nullptr) return;
        FLAlertLayer::create(nullptr,
            Mod::get()->getSettingDefinition(node->getID())->get<CustomSetting>()->json->get<std::string>("name").c_str(),
            Mod::get()->getSettingDefinition(node->getID())->get<CustomSetting>()->json->get<std::string>("description").c_str(),
            "OK",
            nullptr,
            Mod::get()->getSettingDefinition(node->getID())->get<CustomSetting>()->json->get<int>("scale")
        )->show();
    }

public:
    void commit() override {
        // Let the UI know you have committed the value
        this->dispatchCommitted();
    }

    // Geode calls this to query if the setting value has been changed, 
    // and those changes haven't been committed
    bool hasUncommittedChanges() override {
        return false;
    }

    // Geode calls this to query if the setting has a value that is 
    // different from its default value
    bool hasNonDefaultValue() override {
        return true;
    }

    // Geode calls this to reset the setting's value back to default
    void resetToDefault() override {

    }
    static SettingSectionNode* create(SettingSectionValue* value, float width) {
        auto ret = new SettingSectionNode;
        if (ret && ret->init(value, width)) {
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

struct SettingPosStruct {
    int m_pos;
};

class SettingPosValue;

class SettingPosValue : public SettingValue {
protected:
    int m_pos;
public:
    SettingPosValue(std::string const& key, std::string const& modID, int const& position)
      : SettingValue(key, modID), m_pos(position) {}

    bool load(matjson::Value const& json) override {
        try {
            m_pos = static_cast<int>(json.as<int>());
            return true;
        } catch(...) {
            return false;
        }
    }
    bool save(matjson::Value& json) const override {
        json = static_cast<int>(m_pos);
        return true;
    }
    SettingNode* createNode(float width) override;
    void setPos(int pos) {
        m_pos = pos;
    }
    int getPos() const {
        return m_pos;
    }
};

template<>
struct SettingValueSetter<SettingPosStruct> {
    static SettingPosStruct get(SettingValue* setting) {
        auto posSetting = static_cast<SettingPosValue*>(setting);
        struct SettingPosStruct defaultStruct = { posSetting->getPos() };
        return defaultStruct;
    };
    static void set(SettingPosValue* setting, SettingPosStruct const& value) {
        setting->setPos(value.m_pos);
    };
};

class SettingPosNode : public SettingNode {
protected:
    int m_currentPos;
    CCMenuItemToggler* tlBtn;
    CCMenuItemToggler* trBtn;
    CCMenuItemToggler* blBtn;
    CCMenuItemToggler* brBtn;

    int getActiveCornerTag(int corner) {
        switch (corner) {
            case 1: // Top Left
                return 2004;
            case 2: // Top Right
                return 2005;
            case 3: // Bottom Left
                return 2006;
            case 4: // Bottom Right
            default:
                return 2007;
        }
    }
    int tagToCorner(int tag) {
        switch (tag) {
            case 2004: // Top Left
                return 1;
            case 2005: // Top Right
                return 2;
            case 2006: // Bottom Left
                return 3;
            default:
            case 2007: // Bottom Right
                return 4;
        }
    }
    bool init(SettingPosValue* value, float width) {
        if (!SettingNode::init(value))
            return false;
        m_currentPos = value->getPos();
        this->setContentSize({ width, 70.f });
        auto menu = CCMenu::create();
        CCSprite* toggleOn = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        CCSprite* toggleOff = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        toggleOn->setScale(.7F);
        toggleOff->setScale(.7F);
        menu->setPosition(width / 2, 23.f);
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
        tlBtn->setPosition({ -100, 35 });
        trBtn->setPosition({ 100, 35 });
        blBtn->setPosition({ -100, -10 });
        brBtn->setPosition({ 100, -10 });

        tlBtn->setTag(getActiveCornerTag(1));
        trBtn->setTag(getActiveCornerTag(2));
        blBtn->setTag(getActiveCornerTag(3));
        brBtn->setTag(getActiveCornerTag(4));
        int currentCorner = m_currentPos;
        tlBtn->toggle(!(tlBtn->getTag() == getActiveCornerTag(currentCorner)));
        trBtn->toggle(!(trBtn->getTag() == getActiveCornerTag(currentCorner)));
        blBtn->toggle(!(blBtn->getTag() == getActiveCornerTag(currentCorner)));
        brBtn->toggle(!(brBtn->getTag() == getActiveCornerTag(currentCorner)));
        
        menu->addChild(tlBtn);
        menu->addChild(trBtn);
        menu->addChild(blBtn);
        menu->addChild(brBtn);

        this->addChild(menu);
        return true;
    }
    void onCornerClick(CCObject* sender) {
        tlBtn->toggle(true);
        trBtn->toggle(true);
        blBtn->toggle(true);
        brBtn->toggle(true);
        m_currentPos = tagToCorner(sender->getTag());
        this->dispatchChanged();
    };
    void onInfoBtn(CCObject* sender) {
        FLAlertLayer::create(
            Mod::get()->getSettingDefinition(this->m_value->getKey())->get<CustomSetting>()->json->get<std::string>("name").c_str(),
            Mod::get()->getSettingDefinition(this->m_value->getKey())->get<CustomSetting>()->json->get<std::string>("description").c_str(),
            "OK"
        )->show();
    }
public:
    void commit() override {
        static_cast<SettingPosValue*>(m_value)->setPos(m_currentPos);
        this->dispatchCommitted();
    }
    bool hasUncommittedChanges() override {
        return m_currentPos != static_cast<SettingPosValue*>(m_value)->getPos();
    }
    bool hasNonDefaultValue() override {
        return m_currentPos != DEFAULT_POS;
    }

    // Geode calls this to reset the setting's value back to default
    void resetToDefault() override {
        tlBtn->toggle(true);
        trBtn->toggle(true);
        blBtn->toggle(true);
        brBtn->toggle(false);
        m_currentPos = DEFAULT_POS;
    }
    static SettingPosNode* create(SettingPosValue* value, float width) {
        auto ret = new SettingPosNode;
        if (ret && ret->init(value, width)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

/*
    Application
*/

const std::string DEFAULT_APP = "Spotify.exe";

struct SettingAppStruct {
    std::string m_application;
};

class SettingAppValue;

class SettingAppValue : public SettingValue {
protected:
    std::string m_application;
public:
    SettingAppValue(std::string const& key, std::string const& modID, std::string const& application)
      : SettingValue(key, modID), m_application(application) {}

    bool load(matjson::Value const& json) override {
        try {
            m_application = static_cast<std::string>(json.as<std::string>());
            return true;
        } catch(...) {
            return false;
        }
    }
    bool save(matjson::Value& json) const override {
        json = static_cast<std::string>(m_application);
        return true;
    }
    SettingNode* createNode(float width) override;
    void setApp(std::string app) {
        m_application = app;
    }
    std::string getApp() const {
        return m_application;
    }
};

template<>
struct SettingValueSetter<SettingAppStruct> {
    static SettingAppStruct get(SettingValue* setting) {
        auto appSetting = static_cast<SettingAppValue*>(setting);
        struct SettingAppStruct defaultStruct = { appSetting->getApp() };
        return defaultStruct;
    };
    static void set(SettingAppValue* setting, SettingAppStruct const& value) {
        setting->setApp(value.m_application);
    };
};


// I LOVE TEMPLATES

class SettingAppNode : public SettingNode {
protected:
    std::string m_currentApp;
    CCMenuItemSpriteExtra* m_resetBtn;
    bool init(SettingAppValue* value, float width) {
        if (!SettingNode::init(value))
            return false;
        m_currentApp = value->getApp();
        this->setContentSize({ width, 40.f });
        auto menu = CCMenu::create();
        auto label = CCLabelBMFont::create("Application", "bigFont.fnt");
        label->setScale(.55F);
        label->setPositionX(-100);
        auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        infoSpr->setScale(.6F);
        
        //auto infoBtn
        auto infoBtn = CCMenuItemSpriteExtra::create(
            infoSpr,
            this,
            menu_selector(SettingAppNode::onInfoBtn)
        );
        infoBtn->setPositionX(-30);
        defaultApp_input->setString(m_currentApp);
        defaultApp_input->setScale(.65F);

        auto fileBtnSpr = CCSprite::createWithSpriteFrameName("gj_folderBtn_001.png");
        fileBtnSpr->setScale(.5f);

        auto fileBtn = CCMenuItemSpriteExtra::create(
            fileBtnSpr, this, menu_selector(SettingAppNode::onPickFile)
        );
        fileBtn->setPosition(.0f, .0f);
        fileBtn->setPositionX(143);
        defaultApp_input->setPosition(70 + (width / 2), 23.f);
        menu->setPosition(width / 2, 23.f);
        menu->addChild(label);
        menu->addChild(infoBtn);
        this->addChild(defaultApp_input);
        menu->addChild(fileBtn);
        this->addChild(menu);

        // Revert button
        /*auto resetBtnSpr = CCSprite::createWithSpriteFrameName("geode/reset-gold.png");
        resetBtnSpr->setScale(.5f);

        m_resetBtn = CCMenuItemSpriteExtra::create(
            resetBtnSpr, this, menu_selector(SettingAppNode::onReset)
        );
        m_resetBtn->setPosition(
            -10,
            .0f
        );
        //if (m_resetBtn) m_resetBtn->setVisible(this->hasNonDefaultValue());
        menu->addChild(m_resetBtn);*/
        return true;
    }
    void onReset(CCObject*) {
        createQuickPopup(
            "Reset",
            "Are you sure you want to <cr>reset</c> <cl>" + 
                Mod::get()->getSettingDefinition(this->m_value->getKey())->get<CustomSetting>()->json->get<std::string>("name") +
                "</c> to <cy>default</c>?",
            "Cancel", "Reset",
            [this](auto, bool btn2) {
                if (btn2) {
                    this->resetToDefault();
                }
            }
        );
    }
    void onPickFile(CCObject*);
    // nvm cant use this because stupid geode, how are we supposed to get the desc for custom settings??? whats the point of custom settings??
    void onInfoBtn(CCObject* sender) {
        /*FLAlertLayer::create(
            setting()->getDefinition().getDisplayName().c_str(),
            setting()->castDefinition().description.value(),
            "OK"
        )->show();*/
        FLAlertLayer::create(
            Mod::get()->getSettingDefinition(this->m_value->getKey())->get<CustomSetting>()->json->get<std::string>("name").c_str(),
            Mod::get()->getSettingDefinition(this->m_value->getKey())->get<CustomSetting>()->json->get<std::string>("description").c_str(),
            "OK"
        )->show();
    }
public:
    InputNode* defaultApp_input = InputNode::create(180.0F, "Application", "bigFont.fnt", "", 1);
    void commit() override {
        static_cast<SettingAppValue*>(m_value)->setApp(m_currentApp);
        //if (m_resetBtn) m_resetBtn->setVisible(this->hasNonDefaultValue());
        this->dispatchCommitted();
    }
    // Geode calls this to query if the setting value has been changed, 
    // and those changes haven't been committed
    bool hasUncommittedChanges() override {
        //if (m_resetBtn) m_resetBtn->setVisible(this->hasNonDefaultValue());
        return m_currentApp != static_cast<SettingAppValue*>(m_value)->getApp();
    }

    // Geode calls this to query if the setting has a value that is 
    // different from its default value
    bool hasNonDefaultValue() override {
        return m_currentApp != DEFAULT_APP;
    }

    // Geode calls this to reset the setting's value back to default
    void resetToDefault() override {
        m_currentApp = DEFAULT_APP;
        static_cast<SettingAppValue*>(m_value)->setApp(m_currentApp);
        //if (m_resetBtn) m_resetBtn->setVisible(false);
        defaultApp_input->setString(DEFAULT_APP);
    }
    static SettingAppNode* create(SettingAppValue* value, float width) {
        auto ret = new SettingAppNode;
        if (ret && ret->init(value, width)) {
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

class SettingCreditsValue;

class SettingCreditsValue : public SettingValue {
protected:
    std::string m_placeholder;
public:
    SettingCreditsValue(std::string const& key, std::string const& modID, std::string const& placeholder)
      : SettingValue(key, modID), m_placeholder(placeholder) {}

    bool load(matjson::Value const& json) override {
        return true;
    }
    bool save(matjson::Value& json) const override {
        return true;
    }
    SettingNode* createNode(float width) override;
};


class SettingCreditsNode : public SettingNode {
protected:
    bool init(SettingCreditsValue* value, float width) {
        if (!SettingNode::init(value))
            return false;
        this->setContentSize({ width, 40.f });
        auto menu = CCMenu::create();
        auto creditsSpr = ButtonSprite::create("Credits", "bigFont.fnt", "GJ_button_01.png");
        creditsSpr->setScale(.6F);
        auto creditsBtn = CCMenuItemSpriteExtra::create(
            creditsSpr,
            this,
            menu_selector(SettingCreditsNode::onCreditsBtn)
        );
        creditsBtn->setPositionX(-115);
        auto betaTestersSpr = ButtonSprite::create("Beta Testers", "bigFont.fnt", "GJ_button_01.png");
        betaTestersSpr->setScale(.6F);
        auto betaTestersBtn = CCMenuItemSpriteExtra::create(
            betaTestersSpr,
            this,
            menu_selector(SettingCreditsNode::onBetaBtn)
        );
        betaTestersBtn->setPositionX(90);
        menu->setPosition(width / 2, 20.f);
        menu->addChild(creditsBtn);
        menu->addChild(betaTestersBtn);
        this->addChild(menu);
        return true;
    }

public:
    void onCreditsBtn(CCObject*) {
        CreditsMenu::create()->show();
    }
    void onBetaBtn(CCObject*) {
        CreditsBetaMenu::create()->show();
    }


    void commit() override {
        // Let the UI know you have committed the value
        this->dispatchCommitted();
    }

    // Geode calls this to query if the setting value has been changed, 
    // and those changes haven't been committed
    bool hasUncommittedChanges() override {
        return false;
    }

    // Geode calls this to query if the setting has a value that is 
    // different from its default value
    bool hasNonDefaultValue() override {
        return true;
    }

    // Geode calls this to reset the setting's value back to default
    void resetToDefault() override {

    }
    static SettingCreditsNode* create(SettingCreditsValue* value, float width) {
        auto ret = new SettingCreditsNode;
        if (ret && ret->init(value, width)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};
