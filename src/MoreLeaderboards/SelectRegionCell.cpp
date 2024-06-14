#include "SelectRegionCell.h"

int SelectRegionCell::country_id = 0;

bool SelectRegionCell::init(std::string mod) {
    std::vector<std::string> data = SelectRegion::getWords(mod, ":");
    std::vector<std::string> data2 = data;

    while (data.size() > 0) {
        std::string id = data[0];
        std::string name = data[1];

        // sprite
        if (id == "1") {
            auto username = cocos2d::CCLabelBMFont::create(name.c_str(), "bigFont.fnt");
            username->setPosition({ 38, 21 });
            username->setAnchorPoint({ 0, 0.5 });
            username->limitLabelWidth(100, .550f, .2f);
            addChild(username);
        } else if (id == "3") {
            if (name != "NAN") {
                std::string spriteName(name);
                std::transform(spriteName.begin(), spriteName.end(), spriteName.begin(), ::tolower);
                if (auto flagSpr = CCSprite::createWithSpriteFrameName((""_spr+spriteName+".png").c_str())) {
                    flagSpr->setPosition({20, 15});
                    flagSpr->setScaleY(30 / flagSpr->getContentHeight() * 0.50);
                    flagSpr->setScaleX(std::min(1.0f, 40 / flagSpr->getContentWidth()));
                    addChild(flagSpr);
                }
            } else {
                auto earthSpr = CCSprite::createWithSpriteFrameName("earth_btn.png"_spr);
                earthSpr->setScale(.425f);
                earthSpr->setPosition({ 20, 14 });
                addChild(earthSpr);
            }
        } else if (id == "5") {
            auto nbPlayers = cocos2d::CCLabelBMFont::create((name + " players").c_str(), "goldFont.fnt");
            nbPlayers->setPosition({ 38, 8 });
            nbPlayers->setAnchorPoint({ 0, 0.5 });
            nbPlayers->setScale(.375f);
            addChild(nbPlayers);
        } else if (id == "4") {
            id_ctr = std::stoi(name);
        }

        data.erase(data.begin());
        data.erase(data.begin());
    }

    CCMenu* btnMenu = CCMenu::create();
    btnMenu->setPosition(0, 0);

    if (id_ctr != country_id) {
        auto viewSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        viewSpr->setScale(.575f);
        auto viewBtn = CCMenuItemSpriteExtra::create(
            viewSpr,
            this,
            menu_selector(SelectRegionCell::onClick)
        );
        viewBtn->setPosition(196, 15);
        btnMenu->addChild(viewBtn);
    } else {
        auto viewSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        viewSpr->setScale(.575f);
        auto viewBtn = CCMenuItemSpriteExtra::create(
            viewSpr,
            this,
            menu_selector(SelectRegionCell::onClick)
        );
        viewBtn->setPosition(196, 15);
        btnMenu->addChild(viewBtn);
    }

    addChild(btnMenu);

    return true;
};

void SelectRegionCell::onClick(cocos2d::CCObject* pSender) {
    selectedRegion(country_id = id_ctr);
}

SelectRegionCell* SelectRegionCell::create(std::string mod, const std::function<void(int)>& callback) {
    auto ret = new SelectRegionCell(callback);
    if (ret && ret->init(mod)) {
        ret->retain();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}
