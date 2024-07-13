#include "NewProfilePage.h"
#include "../includes.h"
#include <Geode/modify/ProfilePage.hpp>
#include <Geode/utils/web.hpp>

static std::unordered_map<std::string, web::WebTask> RUNNING_REQUESTS {};
static std::mutex lock_var;

void NewProfilePage::onBadgePressed(CCObject* pSender) {
    GJUserScore* score = static_cast<GJUserScore*>(static_cast<CCNode*>(pSender)->getUserObject());
    switch (score->m_modBadge) {
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

There's currently <cp>no way</c> to become a <cb>Leaderboard Moderator</c> by applying. <cj>RobTop</c> and the <cy>Leaderboard Moderators</c> will choose users that they think are qualified to become a <cb>Leaderboard Moderator</c> and will suggest them to <cj>RobTop</c>.
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

void NewProfilePage::onGDUtilsDevBadgePressed(CCObject* pSender) {
    FLAlertLayer::create(
        nullptr,
        "GDUtils Developer",
        R"text(
You found a <co>GDUtils developer</c>! :O
        )text",
        "OK",
        nullptr,
        250.0f
    )->show();
}

void NewProfilePage::onGDUtilsContributorBadgePressed(CCObject* pSender) {
    FLAlertLayer::create(
        nullptr,
        "GDUtils Contributor",
        R"text(
<cg>GDUtils Contributor</c> is a user that has contributed to the <cl>GDUtils</c> project by <cy>writing code</c>, <cy>adding new features</c> on GitHub.

This badge is given to users that have contributed to the project by having merged <cy>pull requests</c> and have been recognized by the <cl>GDUtils</c> team.
        )text",
        "OK",
        nullptr,
        350.0f
    )->show();
}

void NewProfilePage::onULDevBadgePressed(CCObject* pSender) {
    FLAlertLayer::create(
        nullptr,
        "UL Developer",
        R"text(
<cg>Updated Leaderboard Developer</c> is a user that has contributed to the <cl>Updated Leaderboard</c> project by <cy>writing code</c> and <cy>managing the project</c>.

They are responsible for <cy>all the updates and content</c> that is added to the <cl>Updated Leaderboard</c> project.
        )text",
        "OK",
        nullptr,
        350.0f
    )->show();
}

void NewProfilePage::onULOfficerBadgePressed(CCObject* pSender) {
    FLAlertLayer::create(
        nullptr,
        "UL Manager",
        R"text(
<cg>Updated Leaderboard Manager</c> is a user that is handling the <cl>Updated Leaderboard</c> project by <cy>managing the project</c> with others <cy>UL Helpers</c>.

They are responsible of the <cy>management</c> and the <cy>Helper team</c> of the <cl>Updated Leaderboard</c> project and they have the same permissions as the <cy>UL Helpers</c>.
        )text",
        "OK",
        nullptr,
        350.0f
    )->show();
}

void NewProfilePage::onULHelperBadgePressed(CCObject* pSender) {
    FLAlertLayer::create(
        nullptr,
        "UL Helper",
        R"text(
<cg>Updated Leaderboard Helper</c> is a user that is helping the <cl>Updated Leaderboard</c> project by <cy>managing the project</c> with others <cy>UL Managers</c>.

They are responsible of the <cy>management</c> of the leaderboards by <cy>adding new users</cy>, <cy>checking for cheaters</c> and <cy>managing the leaderboards</c> with the <cy>UL Managers</c>.
        )text",
        "OK",
        nullptr,
        350.0f
    )->show();
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

// Mod badges descriptions & GDUtils dev badge
class $modify(ProfilePage) {
    void requestGDUtilsBadges(int accountID, CCLayer* layer) {
        const geode::utils::MiniFunction<void(std::string const&)> then = [this, accountID, layer](std::string const& result) {
            std::vector<std::string> data_user = MoreLeaderboards::getWords(result, "|");

            while (data_user.size() > 0) {
                std::vector<std::string> data = MoreLeaderboards::getWords(data_user[0], "?");

                int badge = 0;
                int accountID_data = 0;

                while (data.size() > 0) {
                    std::string id = data[0];
                    std::string name = data[1];

                    if (id == "1") {
                        accountID_data = std::stoi(name);
                    } else if (id == "3") {
                        badge = std::stoi(name);
                    }

                    data.erase(data.begin());
                    data.erase(data.begin());
                }

                if (accountID_data == accountID) {
                    if (badge == 1) {
                        if (!layer->getChildByIDRecursive("gdutils-dev-badge")) {
                            CCMenu* username_menu = typeinfo_cast<CCMenu*>(layer->getChildByIDRecursive("username-menu"));

                            auto badgeGDUtil = CCSprite::createWithSpriteFrameName("gdutils_badge.png"_spr);
                            badgeGDUtil->setScale(.3f);
                            auto badgeGDUtilBtn = CCMenuItemSpriteExtra::create(
                                badgeGDUtil,
                                layer,
                                menu_selector(NewProfilePage::onGDUtilsDevBadgePressed)
                            );

                            badgeGDUtilBtn->setID("gdutils-dev-badge");
                            username_menu->addChild(badgeGDUtilBtn);
                            username_menu->updateLayout();
                        }
                    } else if (badge == 2) {
                        if (!layer->getChildByIDRecursive("gdutils-contributor-badge")) {
                            CCMenu* username_menu = typeinfo_cast<CCMenu*>(layer->getChildByIDRecursive("username-menu"));

                            auto badgeGDUtil = CCSprite::createWithSpriteFrameName("contributorBadge.png"_spr);
                            badgeGDUtil->setScale(1.075f);
                            auto badgeGDUtilBtn = CCMenuItemSpriteExtra::create(
                                badgeGDUtil,
                                layer,
                                menu_selector(NewProfilePage::onGDUtilsContributorBadgePressed)
                            );

                            badgeGDUtilBtn->setID("gdutils-contributor-badge");
                            username_menu->addChild(badgeGDUtilBtn);
                            username_menu->updateLayout();
                        }
                    } else if (badge == 5) {
                        if (!layer->getChildByIDRecursive("gdutils-ul-developer-badge")) {
                            CCMenu* username_menu = typeinfo_cast<CCMenu*>(layer->getChildByIDRecursive("username-menu"));

                            auto badgeGDUtil = CCSprite::createWithSpriteFrameName("ul_dev_badge.png"_spr);
                            badgeGDUtil->setScale(.75f);
                            auto badgeGDUtilBtn = CCMenuItemSpriteExtra::create(
                                badgeGDUtil,
                                layer,
                                menu_selector(NewProfilePage::onULDevBadgePressed)
                            );

                            badgeGDUtilBtn->setID("gdutils-ul-developer-badge");
                            username_menu->addChild(badgeGDUtilBtn);
                            username_menu->updateLayout();
                        }
                    } else if (badge == 6) {
                        if (!layer->getChildByIDRecursive("gdutils-ul-officer-badge")) {
                            CCMenu* username_menu = typeinfo_cast<CCMenu*>(layer->getChildByIDRecursive("username-menu"));

                            auto badgeGDUtil = CCSprite::createWithSpriteFrameName("ul_officer_badge.png"_spr);
                            badgeGDUtil->setScale(.75f);
                            auto badgeGDUtilBtn = CCMenuItemSpriteExtra::create(
                                badgeGDUtil,
                                layer,
                                menu_selector(NewProfilePage::onULOfficerBadgePressed)
                            );

                            badgeGDUtilBtn->setID("gdutils-ul-officer-badge");
                            username_menu->addChild(badgeGDUtilBtn);
                            username_menu->updateLayout();
                        }
                    } else if (badge == 7) {
                        if (!layer->getChildByIDRecursive("gdutils-ul-helper-badge")) {
                            CCMenu* username_menu = typeinfo_cast<CCMenu*>(layer->getChildByIDRecursive("username-menu"));

                            auto badgeGDUtil = CCSprite::createWithSpriteFrameName("ul_helper_badge.png"_spr);
                            badgeGDUtil->setScale(.75f);
                            auto badgeGDUtilBtn = CCMenuItemSpriteExtra::create(
                                badgeGDUtil,
                                layer,
                                menu_selector(NewProfilePage::onULHelperBadgePressed)
                            );

                            badgeGDUtilBtn->setID("gdutils-ul-helper-badge");
                            username_menu->addChild(badgeGDUtilBtn);
                            username_menu->updateLayout();
                        }
                    }
                }

                data_user.erase(data_user.begin());
            }
        };
        const geode::utils::MiniFunction<void(std::string const&)> expect = [this](std::string const& error) {
            log::error("Failed to get GDUtils badges: {}", error);
        };

        geode::utils::web::WebRequest request = web::WebRequest();
        RUNNING_REQUESTS.emplace(
            "@loaderGDUtilsBadgesGet",
            request.get("https://clarifygdps.com/gdutils/gdutils_roles.php").map(
                [expect = std::move(expect), then = std::move(then)](web::WebResponse* response) {
                    if (response->ok()) {
                        then(response->string().value());
                    } else {
                        expect("Request failed");
                    }

                    RUNNING_REQUESTS.erase("@loaderGDUtilsBadgesGet");
                    return *response;
                }
            )
        );
    }

    void loadPageFromUserInfo(GJUserScore* a2) {
        auto layer = m_mainLayer;
        CCMenuItemSpriteExtra* badgeBtn = nullptr;
        CCMenuItemSpriteExtra* badgeGDUtilBtn = nullptr;

        bool modbadge_bool = false;

        ProfilePage::loadPageFromUserInfo(a2);

        CCMenu* username_menu = typeinfo_cast<CCMenu*>(layer->getChildByIDRecursive("username-menu"));
        CCLabelBMFont* label = typeinfo_cast<CCLabelBMFont*>(layer->getChildByIDRecursive("username-label"));

        // Keep 7976112 for contributors soon

        if (layer) {
            CCSprite* modbadge = typeinfo_cast<CCSprite*>(layer->getChildByIDRecursive("mod-badge"));
            if (modbadge != nullptr) {
                modbadge->removeFromParentAndCleanup(true);

                if (a2->m_userName == "RobTop") {
                    auto robtopSpr = CCSprite::createWithSpriteFrameName("robtop_badge.png"_spr);
                    robtopSpr->setScale(.04f);
                    badgeBtn = CCMenuItemSpriteExtra::create(
                        robtopSpr,
                        this,
                        menu_selector(NewProfilePage::onRobTopBadgePressed)
                    );
                } else {
                    badgeBtn = CCMenuItemSpriteExtra::create(
                        modbadge,
                        this,
                        menu_selector(NewProfilePage::onBadgePressed)
                    );
                }

                badgeBtn->setID("mod-badge");
                badgeBtn->setUserObject(a2);
                username_menu->addChild(badgeBtn);

                modbadge_bool = true;
            }

            if (username_menu != nullptr) username_menu->updateLayout();
        }

        requestGDUtilsBadges(a2->m_accountID, layer);
    }
};
