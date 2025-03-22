#include "NewProfilePage.h"
#include "../includes.h"
#include "Geode/utils/general.hpp"
#include <Geode/modify/ProfilePage.hpp>
#include <Geode/utils/web.hpp>

void NewProfilePage::onBadgePressed(CCObject* pSender) {
    geode::log::info("Object: {}", static_cast<CCNode*>(pSender)->getUserObject());
    CCInteger* score = static_cast<CCInteger*>(static_cast<CCNode*>(pSender)->getUserObject());
    geode::log::info("Badge pressed: {}", score->getValue());
    switch (score->getValue()) {
        case 1:
            #ifndef GEODE_IS_ANDROID
                FLAlertLayer::create(
                    nullptr,
                    "Geometry Dash Moderator",
                    R"text(
    A <cl>Geometry Dash Moderator</c> is a user that has been chosen by <cj>RobTop</c> and other <cy>Moderators</c> on the team by suggestions.

    Their first task is to suggest levels to be rated to <cj>RobTop</c>, the current developer of the game and the maintainer of the <co>level rating system</c>, he is the only one that can rate levels.

    <cr>How to become a Geometry Dash Moderator?</c>

    Many community members want to become a <cl>Moderator</c> to be able to suggest levels for a rating. Similar to rating, Moderator promotions are solely done by <cj>RobTop</c> himself on a case-by-case basis, so there are no clear steps to getting it. Here are some general criteria:
    <cg>- Be nice.</c>
    <cg>- Be active in the community.</c>
    <cg>- Be trustworthy.</c>
    <cg>- Be able to judge levels well.</c>

    If you display these qualities over time, there is a chance that <cy>other Moderators</c> may suggest you to <cj>RobTop</c>. Of course, showing that you are qualified may be difficult if you do not share circles with pre-existing Moderators. If you are interested, you can apply to join the team using our application form. Make sure you read the information in it clearly before submitting. Here is the link:

    <cl>https://forms.gle/XyRGtFzD3hshDUQV7</c>
                    )text",
                    "OK",
                    nullptr,
                    400.0f,
                    true,
                    100.0f,
                    1.0f
                )->show();
            #else
                FLAlertLayer::create(
                    nullptr,
                    "Geometry Dash Moderator",
                    R"text(
    A <cl>Geometry Dash Moderator</c> is a user that has been chosen by <cj>RobTop</c> and other <cy>Moderators</c> on the team by suggestions.

    Their first task is to suggest levels to be rated to <cj>RobTop</c>, the current developer of the game and the maintainer of the <co>level rating system</c>, he is the only one that can rate levels.
                    )text",
                    "OK",
                    nullptr,
                    400.0f,
                    true,
                    100.0f,
                    1.0f
                )->show();

            #endif
            break;
        case 2:
            #ifndef GEODE_IS_ANDROID // android height broken
                FLAlertLayer::create(
                    nullptr,
                    "Geometry Dash Elder Moderator",
                    R"text(
    A <cg>Geometry Dash Elder Moderator</c> is a user that has been chosen by <cj>RobTop</c> and other <cy>Moderators</c> on the team by suggestions and was already a <cy>Geometry Dash Moderator</c> before his promotion. They are the <cp>pillars</c> of the moderation team.

    Their first task is to suggest levels to be rated to <cj>RobTop</c>, the current developer of the game and the maintainer of the <co>level rating system</c>, which he is the only one that can rate levels. But they also have the power to <cg>moderate comments</c>, <cg>whitelists Newgrounds artists</c> and <cg>unlocks updates for old rated levels</c>.

    <cr>How to become a Geometry Dash Moderator?</c>

    Many community members want to become a <cl>Moderator</c> to be able to suggest levels for a rating. Similar to rating, Moderator promotions are solely done by <cj>RobTop</c> himself on a case-by-case basis, so there are no clear steps to getting it. Here are some general criteria:
    <cg>- Be nice.</c>
    <cg>- Be active in the community.</c>
    <cg>- Be trustworthy.</c>
    <cg>- Be able to judge levels well.</c>

    If you display these qualities over time, there is a chance that <cy>other Moderators</c> may suggest you to <cj>RobTop</c>. Of course, showing that you are qualified may be difficult if you do not share circles with pre-existing Moderators. If you are interested, you can apply to join the team using our application form. Make sure you read the information in it clearly before submitting. Here is the link:

    <cl>https://forms.gle/XyRGtFzD3hshDUQV7</c>
                    )text",
                    "OK",
                    nullptr,
                    400.0f,
                    true,
                    100.0f,
                    1.0f
                )->show();
            #else 
                FLAlertLayer::create(
                    nullptr,
                    "Geometry Dash Elder Moderator",
                    R"text(
    A <cg>Geometry Dash Elder Moderator</c> is a user that has been chosen by <cj>RobTop</c> and other <cy>Moderators</c> on the team by suggestions and was already a <cy>Geometry Dash Moderator</c> before his promotion. They are the <cp>pillars</c> of the moderation team.

    Their first task is to suggest levels to be rated to <cj>RobTop</c>, the current developer of the game and the maintainer of the <co>level rating system</c>, which he is the only one that can rate levels. But they also have the power to <cg>moderate comments</c>, <cg>whitelists Newgrounds artists</c> and <cg>unlocks updates for old rated levels</c>.
                    )text",
                    "OK",
                    nullptr,
                    400.0f,
                    true,
                    100.0f,
                    1.0f
                )->show();
            #endif
            break;
        case 3:
            FLAlertLayer::create(
                nullptr,
                "Geometry Dash Leaderboard Moderator",
                R"text(
A <cb>Geometry Dash Leaderboard Moderator</c> is a user that has been chosen by <cj>RobTop</c> and other <cy>Leaderboard Moderators</c> on the team by suggestions.

Their first task is to moderate the <co>Top 100 / Global leaderboard</c> (as well as <cy>level-specific ones</c>) and <cm>ban cheaters</c> to keep the leaderboards clean.

<cr>How to become a Geometry Dash Leaderboard Moderator?</c>

If you are interested, you can apply to join the team using our application form. Make sure you read the information in it clearly before submitting. Here is the link:

<cl>https://docs.google.com/forms/d/e/1FAIpQLSfrJyIVM7KzPyTgzGZ0CSuGBPv40160W9I6bPiMgLRawjJJBg/viewform</c>
                )text",
                "OK",
                nullptr,
                500.0f,
                true,
                40.0f,
                1.0f
            )->show();
            break;
    }
}

void NewProfilePage::onRobTopBadgePressed(CCObject* pSender) {
    FLAlertLayer::create(
        nullptr,
        "RobTop Games",
        R"text(
<cj>RobTop</c> is the creator and the main developer of <cl>Geometry Dash</c>. 

He is the <cy>only developer of the game</c> and is responsible for <cy>all the updates and content</c> that is added to the game.
        )text",
        "OK",
        nullptr,
        300.0f
    )->show();
}

void NewProfilePage::badgeFactoryAlert(CCObject* pSender) {
    CCArray* children = static_cast<CCArray*>(static_cast<CCNode*>(pSender)->getUserObject());

    std::string alertTitle = static_cast<CCString*>(children->objectAtIndex(0))->getCString();
    std::string alertDesc = static_cast<CCString*>(children->objectAtIndex(1))->getCString();
    float alertWidth = static_cast<CCFloat*>(children->objectAtIndex(2))->getValue();
    bool isKofi = strcmp(static_cast<CCString*>(children->objectAtIndex(3))->getCString(), "1") == 0;

    // Fix \n in alertDesc
    std::string::size_type n = 0;
    while ((n = alertDesc.find("\\n", n)) != std::string::npos) {
        alertDesc.replace(n, 2, "\n");
        n += 1;
    }

    if (isKofi) {
        geode::createQuickPopup(
            alertTitle.c_str(),
            alertDesc.c_str(),
            "OK", "Ko-Fi",
            [](auto, bool btn2) {
                if (btn2) {
                    geode::utils::web::openLinkInBrowser("https://ko-fi.com/gdutils");
                }
            },
            alertWidth
        );
    } else {
        FLAlertLayer::create(
            nullptr,
            alertTitle.c_str(),
            alertDesc.c_str(),
            "OK",
            nullptr,
            alertWidth
        )->show();
    }
}

// Mod badges descriptions & GDUtils dev badge
class $modify(ProfilePage) {
    struct Fields {
        EventListener<web::WebTask> m_listener;
    };

    void requestGDUtilsBadges(int accountID, CCLayer* layer) {
        const std::function<void(std::string const&)> then = [this, accountID, layer](std::string const& result) {
            std::vector<std::string> data_user = MoreLeaderboards::getWords(result, "|");

            while (data_user.size() > 0) {
                std::vector<std::string> data = MoreLeaderboards::getWords(data_user[0], "?");

                int badge = 0;
                int accountID_data = 0;
                std::string badge_id = "";
                std::string badge_sprite = "";
                float badge_scale = 1.0f;
                std::string alertTitle = "";
                std::string alertDesc = "";
                float alertWidth = 300.0f;
                bool isKofi = false;

                while (data.size() > 0) {
                    std::string id = data[0];
                    std::string name = data[1];

                    if (id == "1") {
                        accountID_data = numFromString<int>(name).unwrapOrDefault();
                    } else if (id == "3") {
                        badge = numFromString<int>(name).unwrapOrDefault();
                    } else if (id == "4") {
                        badge_sprite = name;
                    } else if (id == "5") {
                        badge_id = name;
                    } else if (id == "6") {
                        badge_scale = numFromString<float>(name).unwrapOrDefault();
                    } else if (id == "8") {
                        alertTitle = name;
                    } else if (id == "9") {
                        alertDesc = name;
                    } else if (id == "10") {
                        alertWidth = numFromString<float>(name).unwrapOrDefault();
                    } else if (id == "11") {
                        isKofi = name == "1";
                    }

                    data.erase(data.begin());
                    data.erase(data.begin());
                }

                if (accountID_data == accountID) {
                    if (!layer->getChildByIDRecursive(badge_id)) {
                        CCMenu* username_menu = typeinfo_cast<CCMenu*>(layer->getChildByIDRecursive("username-menu"));

                        auto badgeGDUtil = CCSprite::createWithSpriteFrameName(fmt::format("{}"_spr, badge_sprite).c_str());
                        badgeGDUtil->setScale(badge_scale);

                        auto badgeGDUtilBtn = CCMenuItemSpriteExtra::create(
                            badgeGDUtil,
                            layer,
                            menu_selector(NewProfilePage::badgeFactoryAlert)
                        );

                        badgeGDUtilBtn->setID(badge_id);

                        auto array = CCArray::create();
                        array->addObject(CCString::create(alertTitle));
                        array->addObject(CCString::create(alertDesc));
                        array->addObject(CCFloat::create(alertWidth));
                        array->addObject(CCString::create(isKofi ? "1" : "0"));
                        badgeGDUtilBtn->setUserObject(array);

                        username_menu->addChild(badgeGDUtilBtn);
                        username_menu->updateLayout();
                    }
                }

                data_user.erase(data_user.begin());
            }
        };
        const std::function<void(std::string const&)> expect = [this](std::string const& error) {
            log::error("Failed to get GDUtils badges: {}", error);
        };

        m_fields->m_listener.bind([expect = std::move(expect), then = std::move(then)] (web::WebTask::Event* e) {
            if (web::WebResponse* res = e->getValue()) {
                if (res->ok()) {
                    then(res->string().unwrapOrDefault());
                } else {
                    expect("Request failed");
                }
            }
        });

        auto req = web::WebRequest();
        m_fields->m_listener.setFilter(req.get("https://clarifygdps.com/gdutils/gdutils_roles.php"));
    }

    void loadPageFromUserInfo(GJUserScore* a2) {
        ProfilePage::loadPageFromUserInfo(a2);
        auto layer = m_mainLayer;
        CCMenuItemSpriteExtra* badgeBtn = nullptr;

        CCMenu* username_menu = typeinfo_cast<CCMenu*>(layer->getChildByIDRecursive("username-menu"));
        CCLabelBMFont* label = typeinfo_cast<CCLabelBMFont*>(layer->getChildByIDRecursive("username-label"));

        if (layer) {
            CCSprite* modbadge = typeinfo_cast<CCSprite*>(layer->getChildByIDRecursive("mod-badge"));
            if (modbadge != nullptr) {
                modbadge->removeFromParentAndCleanup(true);

                if (a2->m_userName == "RobTop" && Mod::get()->template getSettingValue<bool>("customBadges")) {
                    auto robtopSpr = CCSprite::createWithSpriteFrameName("robtop_badge.png"_spr);
                    badgeBtn = CCMenuItemSpriteExtra::create(
                        robtopSpr,
                        this,
                        menu_selector(NewProfilePage::onRobTopBadgePressed)
                    );
                } else {
                    CCSprite* modSprite;
                    if (a2->m_modBadge == 1) {
                        modSprite = CCSprite::createWithSpriteFrameName("modBadge_01_001.png");
                    } else if (a2->m_modBadge == 2) {
                        modSprite = CCSprite::createWithSpriteFrameName("modBadge_02_001.png");
                    } else if (a2->m_modBadge == 3) {
                        modSprite = CCSprite::createWithSpriteFrameName("modBadge_03_001.png");
                    }

                    badgeBtn = CCMenuItemSpriteExtra::create(
                        modSprite,
                        this,
                        menu_selector(NewProfilePage::onBadgePressed)
                    );
                }

                badgeBtn->setID("mod-badge");
                badgeBtn->setUserObject(CCInteger::create(a2->m_modBadge));
                username_menu->addChild(badgeBtn);
            }

            if (username_menu != nullptr) username_menu->updateLayout();
        }

        if (Mod::get()->template getSettingValue<bool>("customBadges")) requestGDUtilsBadges(a2->m_accountID, layer);
    }
};
