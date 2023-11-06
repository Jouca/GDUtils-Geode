#include "MoreLeaderboardsCell.h"
#include <Geode/utils/web.hpp>

void MoreLeaderboardsCell::loadFromDict(cocos2d::CCDictionary* content) // 
{
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto name = content->valueForKey("modstring")->getCString();

    auto mediaMenu = CCMenu::create();
    std::string usernameTemp = "";

    std::vector<std::string> data = MoreLeaderboards::getWords(name, "?");
    std::vector<std::string> data2 = data;

    while (data.size() > 0) {
        std::string id = data[0];
        std::string name = data[1];

        if (id == "1") {
            usernameTemp = name;
        } else if (id == "3") {
            if (name == "1") {
                auto elderModBadge = cocos2d::CCSprite::createWithSpriteFrameName("modBadge_02_001.png");
                elderModBadge->setPosition({ 30, 20 });
                elderModBadge->setScale(1.225f);
                this->m_mainLayer->addChild(elderModBadge);
            } else {
                auto modBadge = cocos2d::CCSprite::createWithSpriteFrameName("modBadge_01_001.png");
                modBadge->setPosition({ 30, 20 });
                modBadge->setScale(1.225f);
                this->m_mainLayer->addChild(modBadge);
            }
        } else if (id == "4") {
            if (name != "NULL") {
                auto discordSpr = CCSprite::create("discord_btn.png"_spr);
                discordSpr->setScale(2.f);
                auto discordBtn = CCMenuItemSpriteExtra::create(
                    discordSpr,
                    this,
            menu_selector(MoreLeaderboardsCell::callback_link)
            );
                auto discord_link = CCString::create(name);
                discordBtn->setUserObject(discord_link);
                mediaMenu->addChild(discordBtn);
            }
        } else if (id == "5") {
            if (name != "NULL") {
                auto twitchSpr = CCSprite::createWithSpriteFrameName("gj_twitchIcon_001.png");
                auto twitchBtn = CCMenuItemSpriteExtra::create(
                    twitchSpr,
                    this,
                    menu_selector(MoreLeaderboardsCell::callback_link)
                );
                auto twitch_link = CCString::create(name);
                twitchBtn->setUserObject(twitch_link);
                mediaMenu->addChild(twitchBtn);
            }
        } else if (id == "6") {
            if (name != "NULL") {
                auto youtubeSpr = CCSprite::createWithSpriteFrameName("gj_ytIcon_001.png");
                auto youtubeBtn = CCMenuItemSpriteExtra::create(
                    youtubeSpr,
                    this,
                    menu_selector(MoreLeaderboardsCell::callback_link)
                );
                auto youtube_link = CCString::create(name);
                youtubeBtn->setUserObject(youtube_link);
                mediaMenu->addChild(youtubeBtn);
            }
        }

        data.erase(data.begin());
        data.erase(data.begin());
    }

    auto username = cocos2d::CCLabelBMFont::create(usernameTemp.c_str(), "goldFont.fnt");
    username->setPosition({ 57, 20 });
    username->setScale(.725f);

    auto dictData = CCDictionary::create();
    dictData->setObject(CCString::create(data2[3]), "accountID");
    dictData->setObject(CCString::create(data2[13]), "playerID");
    dictData->setObject(CCString::create(usernameTemp.c_str()), "username");
    
    auto usernameBtn = CCMenuItemSpriteExtra::create(
        username,
        this,
        menu_selector(MoreLeaderboardsCell::callback_user)
    );
    usernameBtn->setPosition({0, 0});
    usernameBtn->setAnchorPoint({0, 0.5});
    usernameBtn->setUserObject(dictData);

    auto menu = CCMenu::create();
    menu->addChild(usernameBtn);
    menu->setPosition({ 55, 20 });

    this->m_mainLayer->addChild(menu);

    mediaMenu->alignItemsHorizontallyWithPadding(5);
    mediaMenu->setAnchorPoint({1, 0.5});
    mediaMenu->setPosition({ 320, 20 });
    this->m_mainLayer->addChild(mediaMenu);

    this->m_mainLayer->setVisible(true);
    this->m_backgroundLayer->setOpacity(255);
};

void MoreLeaderboardsCell::draw() {
    reinterpret_cast<StatsCell*>(this)->StatsCell::draw();
}

MoreLeaderboardsCell::MoreLeaderboardsCell(const char* name, CCSize size) :
    GenericListCell(name, size) {}

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
    GameLevelManager* glm = GameLevelManager::sharedState();

    auto data = static_cast<CCDictionary*>(static_cast<CCNode*>(pSender)->getUserObject());
    auto accountID = data->valueForKey("accountID")->getCString();
    auto playerID = data->valueForKey("playerID")->getCString();
    auto username = data->valueForKey("username")->getCString();

    glm->storeUserName(std::stoi(playerID), std::stoi(accountID), username);

    ProfilePage::create(std::stoi(accountID), true)->show();
}

void MoreLeaderboardsCell::callback_link(CCObject* pSender) {
    auto data = static_cast<CCString*>(static_cast<CCNode*>(pSender)->getUserObject());
    web::openLinkInBrowser(data->getCString());
}
