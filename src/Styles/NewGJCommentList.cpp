#include "NewGJCommentList.h"
#include "Geode/loader/Log.hpp"
#include "NewProfilePage.h"
#include "../MoreLeaderboards/MoreLeaderboards.h"
#include "ccTypes.h"
#include <Geode/modify/InfoLayer.hpp>
#include <Geode/modify/CommentCell.hpp>
#include <Geode/utils/web.hpp>

std::string result_global = "";

// Mod badges descriptions & GDUtils dev badge
class $modify(CommentCell) {
    struct Fields {
        EventListener<web::WebTask> m_listener;
    };

    void badgeHandle(std::string result, CCLayer* layer, int commentID) {
        if (this->m_accountComment) return;

        CommentCell* cell = this;
        int accountID = cell->m_comment->m_accountID;

        // Part for handling GDUtils badges
        if (!Mod::get()->template getSettingValue<bool>("customBadges")) return;

        if (result.empty()) return;

        std::string data = result;
        std::vector<std::string> data_user = MoreLeaderboards::getWords(data, "|");

        CCMenu* username_menu = typeinfo_cast<CCMenu*>(cell->getChildByIDRecursive("username-menu"));

        bool colorSet = false;

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
            ccColor3B color = { 255, 255, 255 };

            while (data.size() > 0) {
                std::string id = data[0];
                std::string name = data[1];

                if (id == "1") {
                    accountID_data = std::stoi(name);
                } else if (id == "3") {
                    badge = std::stoi(name);
                } else if (id == "4") {
                    badge_sprite = name;
                } else if (id == "5") {
                    badge_id = name;
                } else if (id == "7") {
                    badge_scale = std::stof(name);
                } else if (id == "8") {
                    alertTitle = name;
                } else if (id == "9") {
                    alertDesc = name;
                } else if (id == "10") {
                    alertWidth = std::stof(name);
                } else if (id == "11") {
                    isKofi = name == "1";
                } else if (id == "12") {
                    // Get color from HEX
                    color = ccc3(
                        std::stoi(name.substr(0, 2), nullptr, 16),
                        std::stoi(name.substr(2, 2), nullptr, 16),
                        std::stoi(name.substr(4, 2), nullptr, 16)
                    );
                }

                data.erase(data.begin());
                data.erase(data.begin());
            }

            if (accountID_data == accountID) {
                if (!cell->getChildByIDRecursive(badge_id)) {
                    auto badgeGDUtil = CCSprite::createWithSpriteFrameName(fmt::format("{}"_spr, badge_sprite).c_str());
                    badgeGDUtil->setScale(badge_scale);
                    auto badgeGDUtilBtn = CCMenuItemSpriteExtra::create(
                        badgeGDUtil,
                        cell,
                        menu_selector(NewProfilePage::badgeFactoryAlert)
                    );

                    if (auto commentText = cell->getChildByIDRecursive("comment-text-label")) {
                        if (color.r != 255 && color.g != 255 && color.b != 255 && !colorSet) {
                            as<CCLabelBMFont*>(commentText)->setColor(color);
                            cell->m_comment->m_color = color;
                            colorSet = true;
                        }
                    }
                    if (auto commentText = cell->getChildByIDRecursive("comment-text-area")) {
                        TextArea* textArea = as<TextArea*>(commentText);
                        MultilineBitmapFont* bmFont = as<MultilineBitmapFont*>(textArea->getChildren()->objectAtIndex(0));
                        CCArray* children = bmFont->getChildren();
                        for (int i = 0; i < children->count(); i++) {
                            auto child = as<CCLabelBMFont*>(children->objectAtIndex(i));
                            if (color.r != 255 && color.g != 255 && color.b != 255 && !colorSet) {
                                child->setColor(color);
                                colorSet = true;
                            }
                        }
                    }

                    badgeGDUtilBtn->setID(badge_id);

                    auto array = CCArray::create();
                    array->addObject(CCString::create(alertTitle));
                    array->addObject(CCString::create(alertDesc));
                    array->addObject(CCFloat::create(alertWidth));
                    array->addObject(CCString::create(isKofi ? "1" : "0"));
                    badgeGDUtilBtn->setUserObject(array);

                    if (cell->getChildByIDRecursive("percentage-label")) {
                        username_menu->insertBefore(badgeGDUtilBtn, cell->getChildByIDRecursive("percentage-label"));
                    } else {
                        username_menu->addChild(badgeGDUtilBtn);
                    }
                    username_menu->updateLayout();
                }
            }

            data_user.erase(data_user.begin());
        }
    }

    void loadFromComment(GJComment* comment) {
        CommentCell::loadFromComment(comment);

        if (comment->m_commentDeleted) return;

        CCMenuItemSpriteExtra* badgeBtn = nullptr;
        CommentCell* cell = this;
        CCSprite* modbadge = typeinfo_cast<CCSprite*>(cell->getChildByIDRecursive("mod-badge"));
        std::string username = cell->m_comment->m_userName;
        CCMenu* username_menu = typeinfo_cast<CCMenu*>(cell->getChildByIDRecursive("username-menu"));

        if (modbadge != nullptr) {
            modbadge->removeFromParentAndCleanup(true);

            if (username == "RobTop" && Mod::get()->template getSettingValue<bool>("customBadges")) {
                auto robtopSpr = CCSprite::createWithSpriteFrameName("robtop_badge.png"_spr);
                robtopSpr->setScale(0.75f);
                badgeBtn = CCMenuItemSpriteExtra::create(
                    robtopSpr,
                    this,
                    menu_selector(NewProfilePage::onRobTopBadgePressed)
                );
            } else {
                CCSprite* modSprite;
                if (comment->m_modBadge == 1) {
                    modSprite = CCSprite::createWithSpriteFrameName("modBadge_01_001.png");
                } else if (comment->m_modBadge == 2) {
                    modSprite = CCSprite::createWithSpriteFrameName("modBadge_02_001.png");
                } else if (comment->m_modBadge == 3) {
                    modSprite = CCSprite::createWithSpriteFrameName("modBadge_03_001.png");
                }
                modSprite->setScale(0.6f);

                badgeBtn = CCMenuItemSpriteExtra::create(
                    modSprite,
                    this,
                    menu_selector(NewProfilePage::onBadgePressed)
                );
            }

            badgeBtn->setID("mod-badge");
            badgeBtn->setUserObject(CCInteger::create(comment->m_modBadge));
            if (cell->getChildByIDRecursive("percentage-label")) {
                username_menu->insertBefore(badgeBtn, cell->getChildByIDRecursive("percentage-label"));
            } else {
                username_menu->addChild(badgeBtn);
            }
        }

        if (username_menu != nullptr) username_menu->updateLayout();

        const std::function<void(std::string const&)> then = [this, cell](std::string const& result) {
            badgeHandle(result, cell, cell->m_comment->m_commentID);
        };

        m_fields->m_listener.bind([then = std::move(then)] (web::WebTask::Event* e) {
            if (web::WebResponse* res = e->getValue()) {
                if (res->ok()) {
                    result_global = res->string().unwrapOrDefault();
                    then(result_global);
                }
            }
        });

        auto req = web::WebRequest();
        m_fields->m_listener.setFilter(req.get("https://clarifygdps.com/gdutils/gdutils_roles.php"));
    }
};
