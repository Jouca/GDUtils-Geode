#include "NewProfilePage.h"
#include "../includes.h"
#include <Geode/modify/ProfilePage.hpp>

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

void NewProfilePage::onGDUtilsBadgePressed(CCObject* pSender) {
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

// Mod badges descriptions & GDUtils dev badge
class $modify(ProfilePage) {
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

                badgeBtn = CCMenuItemSpriteExtra::create(
                    modbadge,
                    this,
                    menu_selector(NewProfilePage::onBadgePressed)
                );

                badgeBtn->setID("mod-badge");
                badgeBtn->setUserObject(a2);
                badgeBtn->setPosition(label->getPosition() + CCPoint { -5.f, -1.f });
                username_menu->addChild(badgeBtn);

                modbadge_bool = true;
            }

            std::vector<int> gdutils_accountID_devs = { 7026949, 6253758, 5509312 };
            if (std::find(gdutils_accountID_devs.begin(), gdutils_accountID_devs.end(), a2->m_accountID) != gdutils_accountID_devs.end()) {
                if (label != nullptr && !layer->getChildByIDRecursive("gdutils-badge")) {
                    auto badgeGDUtil = CCSprite::create(Mod::get()->expandSpriteName("gdutils_badge.png"));
                    badgeGDUtil->setScale(.3f);
                    badgeGDUtilBtn = CCMenuItemSpriteExtra::create(
                        badgeGDUtil,
                        this,
                        menu_selector(NewProfilePage::onGDUtilsBadgePressed)
                    );

                    badgeGDUtilBtn->setID("gdutils-badge");
                    if (modbadge_bool)
                        badgeGDUtilBtn->setPosition(badgeBtn->getPosition() + CCPoint { -5.f, -1.f });
                    else
                        badgeGDUtilBtn->setPosition(label->getPosition() + CCPoint { -5.f, -1.f });

                    username_menu->addChild(badgeGDUtilBtn);
                }
            }

            if (username_menu != nullptr) username_menu->updateLayout();
        }
    }
};
