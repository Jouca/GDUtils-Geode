#include "MoreLeaderboardsCell.hpp"
#include "../MoreLeaderboards.hpp"

void MoreLeaderboardsCell::loadFromDict(cocos2d::CCDictionary* content) {
    auto name = content->valueForKey("modstring")->getCString();

    auto mediaMenu = CCMenu::create();
    float mediaXPosition = 327;
    int mediaCount = 0;
    std::string usernameTemp = "";

    std::vector<std::string> data = MoreLeaderboards::getWords(name, "?");
    std::vector<std::string> data2 = data;

    while (data.size() > 0) {
        std::string id = data[0];
        std::string val = data[1];

        if (id == "1") {
            usernameTemp = val;
        } else if (id == "3") {
            CCSprite* badge = nullptr;
            if (val == "0") {
                badge = cocos2d::CCSprite::createWithSpriteFrameName("robtop_badge.png"_spr);
            } else if (val == "2") {
                badge = cocos2d::CCSprite::createWithSpriteFrameName("modBadge_01_001.png");
            } else if (val == "1") {
                badge = cocos2d::CCSprite::createWithSpriteFrameName("modBadge_02_001.png");
            } else if (val == "3") {
                badge = cocos2d::CCSprite::createWithSpriteFrameName("modBadge_03_001.png");
            }
            if (badge) {
                badge->setPosition({30, 20});
                badge->setScale(1.225f);
                this->m_mainLayer->addChild(badge);
            }
        } else if (id == "4") {
            if (val != "NULL") {
                auto discordBtn = CCMenuItemSpriteExtra::create(
                    CCSprite::createWithSpriteFrameName("gj_discordIcon_001.png"),
                    this, menu_selector(MoreLeaderboardsCell::callback_link)
                );
                discordBtn->setUserObject(CCString::create(val));
                mediaMenu->addChild(discordBtn);
                mediaCount++;
            }
        } else if (id == "5") {
            if (val != "NULL") {
                auto twitchBtn = CCMenuItemSpriteExtra::create(
                    CCSprite::createWithSpriteFrameName("gj_twitchIcon_001.png"),
                    this, menu_selector(MoreLeaderboardsCell::callback_link)
                );
                twitchBtn->setUserObject(CCString::create(val));
                mediaMenu->addChild(twitchBtn);
                mediaCount++;
            }
        } else if (id == "6") {
            if (val != "NULL") {
                auto youtubeBtn = CCMenuItemSpriteExtra::create(
                    CCSprite::createWithSpriteFrameName("gj_ytIcon_001.png"),
                    this, menu_selector(MoreLeaderboardsCell::callback_link)
                );
                youtubeBtn->setUserObject(CCString::create(val));
                mediaMenu->addChild(youtubeBtn);
                mediaCount++;
            }
        }

        data.erase(data.begin());
        data.erase(data.begin());
    }

    auto username = cocos2d::CCLabelBMFont::create(usernameTemp.c_str(), "goldFont.fnt");
    username->setPosition({57, 20});
    username->setScale(.725f);

    auto dictData = CCDictionary::create();
    dictData->setObject(CCString::create(data2[3]), "accountID");
    dictData->setObject(CCString::create(data2[13]), "playerID");
    dictData->setObject(CCString::create(usernameTemp.c_str()), "username");

    auto usernameBtn = CCMenuItemSpriteExtra::create(
        username, this, menu_selector(MoreLeaderboardsCell::callback_user)
    );
    usernameBtn->setPosition({0, 0});
    usernameBtn->setAnchorPoint({0, 0.5});
    usernameBtn->setUserObject(dictData);

    auto menu = CCMenu::create();
    menu->addChild(usernameBtn);
    menu->setPosition({55, 20});
    this->m_mainLayer->addChild(menu);

    if (mediaCount == 1) mediaXPosition = 327;
    else if (mediaCount == 2) mediaXPosition = 308;
    else if (mediaCount == 3) mediaXPosition = 289;

    mediaMenu->alignItemsHorizontallyWithPadding(5);
    mediaMenu->setAnchorPoint({1, 0.5});
    mediaMenu->setPosition({mediaXPosition, 20});
    this->m_mainLayer->addChild(mediaMenu);

    this->m_mainLayer->setVisible(true);
    this->m_backgroundLayer->setOpacity(255);
}

MoreLeaderboardsCell::MoreLeaderboardsCell(const char* name, CCSize size)
    : GenericListCell(name, size) {}

MoreLeaderboardsCell* MoreLeaderboardsCell::create(const char* key, CCSize size) {
    auto ret = new MoreLeaderboardsCell(key, size);
    if (ret && ret->init()) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
}

void MoreLeaderboardsCell::callback_user(CCObject* pSender) {
    auto data = static_cast<CCDictionary*>(static_cast<CCNode*>(pSender)->getUserObject());
    auto accountID = numFromString<int>(data->valueForKey("accountID")->getCString()).unwrapOrDefault();
    auto playerID = numFromString<int>(data->valueForKey("playerID")->getCString()).unwrapOrDefault();
    auto username = data->valueForKey("username")->getCString();

    GameLevelManager::sharedState()->storeUserName(playerID, accountID, username);
    ProfilePage::create(accountID, true)->show();
}

void MoreLeaderboardsCell::callback_link(CCObject* pSender) {
    auto data = static_cast<CCString*>(static_cast<CCNode*>(pSender)->getUserObject());
    web::openLinkInBrowser(data->getCString());
}
