#include "Geode/utils/async.hpp"
#include "Geode/utils/string.hpp"
#include <defs/geode.hpp>
#include <Geode/modify/ProfilePage.hpp>
#include <Geode/modify/CommentCell.hpp>
#include <ui/EventInfoPopup.hpp>

std::string s_cachedBadgesResponse = "";

// Primary dispatch: unique int tag -> callback.
// Each call to makeBadgeButton gets a fresh tag stored directly on the button.
// Tags are never copied by BadgesAPI (its CCMenuItemSpriteExtra::create starts with kCCNodeTagInvalid = -1).
static int s_nextBadgeTag = 0;
static std::unordered_map<int, std::function<void()>> s_tagToCallback;

// Fallback dispatch: CCSpriteFrame* -> tag, used when BadgesAPI copies our button.
// BadgesAPI does CCSprite::createWithSpriteFrame(originalFrame), so the new sprite
// shares the same CCSpriteFrame object and the lookup resolves to the last registered tag.
static std::unordered_map<CCSpriteFrame*, int> s_frameToTag;

// Singleton target for all badge selectors.
// IMPORTANT: BadgesAPI copies our buttons as:
//   CCMenuItemSpriteExtra::create(child->getNormalImage(), this/*BadgesAPI*/, child->m_pfnSelector)
// so `this` inside onBadgeClick may be the BadgesAPI object — NEVER dereference it.
// Only touch `sender` and global/static state.
class GDUtilsBadgeHandler : public CCObject {
public:
    static GDUtilsBadgeHandler* get() {
        static auto* inst = [] {
            auto* h = new GDUtilsBadgeHandler();
            h->retain();
            return h;
        }();
        return inst;
    }

    void onBadgeClick(CCObject* sender) {
        auto* btn = typeinfo_cast<CCMenuItemSpriteExtra*>(sender);
        if (!btn) return;

        // Primary: tag set directly on this button instance (original button click).
        int tag = btn->getTag();
        if (auto it = s_tagToCallback.find(tag); it != s_tagToCallback.end()) {
            it->second();
            return;
        }

        // Fallback: sprite frame lookup (BadgesAPI popup copy, tag is -1).
        if (auto* spr = typeinfo_cast<CCSprite*>(btn->getNormalImage())) {
            if (auto fit = s_frameToTag.find(spr->displayFrame()); fit != s_frameToTag.end()) {
                if (auto it = s_tagToCallback.find(fit->second); it != s_tagToCallback.end())
                    it->second();
            }
        }
    }
};

static CCMenuItemSpriteExtra* makeBadgeButton(CCNode* normalSprite, std::string const& id, std::function<void()> callback) {
    int tag = ++s_nextBadgeTag;
    s_tagToCallback[tag] = std::move(callback);
    if (auto* spr = typeinfo_cast<CCSprite*>(normalSprite))
        s_frameToTag[spr->displayFrame()] = tag;
    auto* btn = CCMenuItemSpriteExtra::create(normalSprite, GDUtilsBadgeHandler::get(), menu_selector(GDUtilsBadgeHandler::onBadgeClick));
    btn->setID(id);
    btn->setTag(tag);
    return btn;
}

static void requestBadges(async::TaskHolder<web::WebResponse> listener, std::function<void(std::string const&)> callback) {
    if (!s_cachedBadgesResponse.empty()) {
        callback(s_cachedBadgesResponse);
    } else {
        listener.spawn(web::WebRequest().get("https://clarifygdps.com/gdutils/gdutils_roles.php?version=1.0"), [callback](web::WebResponse value) {
            if (value.ok()) {
                auto res = value.string().unwrapOrDefault();
                callback(std::move(res));
            } else {
                log::error("Failed to get GDUtils badges: {}", value.error());
            }
        });
    }
}

static void onRobTopBadgePressed() {
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
static void onModBadgePressed(int badgeID) {
    switch (badgeID) {
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

// Mod badges descriptions & GDUtils dev badge
class $modify(GDUtilsProfilePage, ProfilePage) {
    struct Fields {
        async::TaskHolder<web::WebResponse> m_listener;
    };

    void handleResponse(std::string result, int accountID) {
        auto menu = m_mainLayer->getChildByID("username-menu");
        if (!menu) return;
        if (result.empty()) return;
        s_cachedBadgesResponse = result;
        std::vector<std::string> users = geode::utils::string::split(result, "|");
        for (auto user : users) {
            std::vector<std::string> data = geode::utils::string::split(user, "?");
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
                if (!m_mainLayer->getChildByIDRecursive(badge_id)) {
                    auto* sprite = Build<CCSprite>::createSpriteName(fmt::format("{}"_spr, badge_sprite).c_str())
                        .scale(badge_scale)
                        .collect();
                    auto* btn = makeBadgeButton(sprite, badge_id, [title = alertTitle, desc = alertDesc, isKofi, width = alertWidth]() {
                        if (isKofi) {
                            geode::createQuickPopup(title.c_str(), geode::utils::string::replace(desc, "\\n", "\n").c_str(), "OK", "Ko-Fi",
                                [](auto, bool btn2) { if (btn2) geode::utils::web::openLinkInBrowser("https://ko-fi.com/gdutils"); }, width);
                        } else {
                            FLAlertLayer::create(nullptr, title.c_str(), desc.c_str(), "OK", nullptr, width)->show();
                        }
                    });
                    menu->addChild(btn);
                    menu->updateLayout();
                }
            }
        }
    }

    void loadPageFromUserInfo(GJUserScore* score) {
        ProfilePage::loadPageFromUserInfo(score);
        if (m_mainLayer) {
            auto menu = m_mainLayer->getChildByID("username-menu");
            if (!menu) return;
            if (auto modBadge = menu->getChildByID("mod-badge")) {
                modBadge->removeFromParentAndCleanup(true);
                if (score->m_userName == "RobTop" && Mod::get()->getSettingValue<bool>("customBadges")) {
                    auto* spr = Build<CCSprite>::createSpriteName("robtop_badge.png"_spr).collect();
                    menu->addChild(makeBadgeButton(spr, "mod-badge", []() { onRobTopBadgePressed(); }));
                } else {
                    int badgeID = score->m_modBadge;
                    auto* spr = Build<CCSprite>::createSpriteName(fmt::format("modBadge_0{}_001.png", badgeID).c_str()).collect();
                    menu->addChild(makeBadgeButton(spr, "mod-badge", [badgeID]() { onModBadgePressed(badgeID); }));
                }
            }
            if (Mod::get()->getSettingValue<bool>("customBadges")) {
                if (!s_cachedBadgesResponse.empty()) {
                    handleResponse(s_cachedBadgesResponse, score->m_accountID);
                } else {
                    m_fields->m_listener.spawn(web::WebRequest().get("https://clarifygdps.com/gdutils/gdutils_roles.php?version=1.0"), [this, accountID = std::move(score->m_accountID)](web::WebResponse value) {
                        if (value.ok()) {
                            auto res = value.string().unwrapOrDefault();
                            handleResponse(std::move(res), accountID);
                        } else {
                            log::error("Failed to get GDUtils badges: {}", value.error());
                        }
                    });
                }
            }
            menu->updateLayout();
        }
    }
};

// Comments
#include <Geode/utils/web.hpp>

std::string badgesDataComments = "";

int hexStringToInt(const std::string& hexStr) {
    int value;
    std::istringstream iss(hexStr);
    iss >> std::hex >> value;
    return value;
}

class $modify(CustomBadgesCommentCell, CommentCell) {
    struct Fields {
        async::TaskHolder<web::WebResponse> m_listener;
    };

    void handleResponse(std::string result, int commentID) {
        auto menu = m_mainLayer->getChildByIDRecursive("username-menu");
        if (!menu) return;
        if (this->m_accountComment) return;
        if (result.empty()) return;
        s_cachedBadgesResponse = result;
        std::vector<std::string> users = geode::utils::string::split(result, "|");

        CommentCell* cell = this;
        int accountID = cell->m_comment->m_accountID;
        bool colorSet = false;

        for (auto user : users) {
            std::vector<std::string> data = geode::utils::string::split(user, "?");
            int badge = 0;
            int accountID_data = 0;
            std::string badge_id = "";
            std::string badge_sprite = "";
            float badge_scale = 1.0f;
            std::string alertTitle = "";
            std::string alertDesc = "";
            float alertWidth = 300.0f;
            bool isKofi = false;
            ccColor3B color = { 255, 255, 255 };

            while (data.size() > 0) {
                std::string id = data[0];
                std::string name = data[1];
                if (id == "1") {
                    accountID_data = geode::utils::numFromString<int>(name).unwrapOrDefault();
                } else if (id == "3") {
                    badge = geode::utils::numFromString<int>(name).unwrapOrDefault();
                } else if (id == "4") {
                    badge_sprite = name;
                } else if (id == "5") {
                    badge_id = name;
                } else if (id == "7") {
                    badge_scale = geode::utils::numFromString<float>(name).unwrapOrDefault();
                } else if (id == "8") {
                    alertTitle = name;
                } else if (id == "9") {
                    alertDesc = name;
                } else if (id == "10") {
                    alertWidth = geode::utils::numFromString<float>(name).unwrapOrDefault();
                } else if (id == "11") {
                    isKofi = name == "1";
                } else if (id == "12") {
                    // Get color from HEX
                    color = ccc3(
                        hexStringToInt(name.substr(0, 2)),
                        hexStringToInt(name.substr(2, 2)),
                        hexStringToInt(name.substr(4, 2))
                    );
                }
                data.erase(data.begin());
                data.erase(data.begin());
            }

            if (accountID_data == accountID) {
                bool isCustomColor = (color.r != 255 || color.g != 255 || color.b != 255);
                if (isCustomColor && !colorSet) {
                    if (auto textArea = m_mainLayer->getChildByType<TextArea*>(0)) {
                        CCArrayExt<CCLabelBMFont*> children = textArea->m_label->getChildren();
                        for (auto label : children) label->setColor(color);
                        colorSet = true;
                    } else if (auto commentText = static_cast<CCLabelBMFont*>(m_mainLayer->getChildByID("comment-text-label"))) {
                        commentText->setColor(color);
                        cell->m_comment->m_color = color;
                        colorSet = true;
                    }
                }
                if (!m_mainLayer->getChildByIDRecursive(badge_id)) {
                    auto* sprite = Build<CCSprite>::createSpriteName(fmt::format("{}"_spr, badge_sprite).c_str())
                        .scale(badge_scale)
                        .collect();
                    auto* btn = makeBadgeButton(sprite, badge_id, [title = alertTitle, desc = alertDesc, isKofi, width = alertWidth]() {
                        if (isKofi) {
                            geode::createQuickPopup(title.c_str(), geode::utils::string::replace(desc, "\\n", "\n").c_str(), "OK", "Ko-Fi",
                                [](auto, bool btn2) { if (btn2) geode::utils::web::openLinkInBrowser("https://ko-fi.com/gdutils"); }, width);
                        } else {
                            FLAlertLayer::create(nullptr, title.c_str(), desc.c_str(), "OK", nullptr, width)->show();
                        }
                    });
                    if (cell->getChildByIDRecursive("percentage-label")) {
                        menu->insertBefore(btn, cell->getChildByIDRecursive("percentage-label"));
                    } else {
                        menu->addChild(btn);
                    }
                    menu->updateLayout();
                }
            }
        }
    }

    void loadFromComment(GJComment* comment) {
        CommentCell::loadFromComment(comment);
        if (comment->m_commentDeleted) return;
        auto menu = m_mainLayer->getChildByIDRecursive("username-menu");
        if (!menu) return;
        if (auto modBadge = menu->getChildByID("mod-badge")) {
            modBadge->removeFromParentAndCleanup(true);
            CCMenuItemSpriteExtra* badgeBtn;
            if (m_comment->m_userName == "RobTop" && Mod::get()->getSettingValue<bool>("customBadges")) {
                auto* spr = Build<CCSprite>::createSpriteName("robtop_badge.png"_spr).scale(0.75f).collect();
                badgeBtn = makeBadgeButton(spr, "mod-badge", []() { onRobTopBadgePressed(); });
            } else {
                int badgeID = comment->m_modBadge;
                auto* spr = Build<CCSprite>::createSpriteName(fmt::format("modBadge_0{}_001.png", badgeID).c_str()).scale(0.6f).collect();
                badgeBtn = makeBadgeButton(spr, "mod-badge", [badgeID]() { onModBadgePressed(badgeID); });
            }
            if (getChildByIDRecursive("percentage-label")) {
                menu->insertBefore(badgeBtn, getChildByIDRecursive("percentage-label"));
            } else {
                menu->addChild(badgeBtn);
            }
        }
        if (Mod::get()->getSettingValue<bool>("customBadges")) {
            if (!s_cachedBadgesResponse.empty()) {
                handleResponse(s_cachedBadgesResponse, m_comment->m_commentID);
            } else {
                m_fields->m_listener.spawn(web::WebRequest().get("https://clarifygdps.com/gdutils/gdutils_roles.php?version=1.0"), [this](web::WebResponse value) {
                    if (value.ok()) {
                        auto res = value.string().unwrapOrDefault();
                        handleResponse(std::move(res), m_comment->m_commentID);
                    } else {
                        log::error("Failed to get GDUtils badges: {}", value.error());
                    }
                });
            }
        }
        menu->updateLayout();
    }
};

static void toggleHook(bool value) {
    for (auto& hook : geode::Mod::get()->getHooks()) {
        if (hook->getDisplayName().starts_with("ProfilePage") || hook->getDisplayName().starts_with("CommentCell")) {
            (void)(value ? hook->enable() : hook->disable());
        }
    }
}
$execute {
    listenForSettingChanges<bool>("clickableBadges", [](bool value) {
        log::info("Clickable Badges changed to {}", value);
        toggleHook(value);
    });
}
$on_mod(Loaded) {
    if (!Mod::get()->getSettingValue<bool>("clickableBadges")) {
        toggleHook(false);
    }
}
