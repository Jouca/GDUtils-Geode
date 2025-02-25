#include "NewGJCommentList.h"
#include "NewProfilePage.h"
#include "../MoreLeaderboards/MoreLeaderboards.h"
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
                    if (!cell->getChildByIDRecursive("gdutils-dev-badge")) {
                        auto badgeGDUtil = CCSprite::createWithSpriteFrameName("gdutils_badge.png"_spr);
                        badgeGDUtil->setScale(0.70f);
                        auto badgeGDUtilBtn = CCMenuItemSpriteExtra::create(
                            badgeGDUtil,
                            cell,
                            menu_selector(NewProfilePage::onGDUtilsDevBadgePressed)
                        );

                        ccColor3B color = { 218, 181, 35 };
                        if (auto commentText = cell->getChildByIDRecursive("comment-text-label")) {
                            as<CCLabelBMFont*>(commentText)->setColor(color);
                            cell->m_comment->m_color = color;
                        }
                        if (auto commentText = cell->getChildByIDRecursive("comment-text-area")) {
                            TextArea* textArea = as<TextArea*>(commentText);
                            MultilineBitmapFont* bmFont = as<MultilineBitmapFont*>(textArea->getChildren()->objectAtIndex(0));
                            CCArray* children = bmFont->getChildren();
                            for (int i = 0; i < children->count(); i++) {
                                auto child = as<CCLabelBMFont*>(children->objectAtIndex(i));
                                child->setColor(color);
                            }
                        }
                        
                        badgeGDUtilBtn->setID("gdutils-dev-badge");
                        if (cell->getChildByIDRecursive("percentage-label")) {
                            username_menu->insertBefore(badgeGDUtilBtn, cell->getChildByIDRecursive("percentage-label"));
                        } else {
                            username_menu->addChild(badgeGDUtilBtn);
                        }
                        username_menu->updateLayout();
                    }
                } else if (badge == 2) {
                    if (!cell->getChildByIDRecursive("gdutils-contributor-badge")) {
                        auto badgeGDUtil = CCSprite::createWithSpriteFrameName("contributorBadge.png"_spr);
                        badgeGDUtil->setScale(0.70f);
                        auto badgeGDUtilBtn = CCMenuItemSpriteExtra::create(
                            badgeGDUtil,
                            cell,
                            menu_selector(NewProfilePage::onGDUtilsContributorBadgePressed)
                        );

                        badgeGDUtilBtn->setID("gdutils-contributor-badge");
                        if (cell->getChildByIDRecursive("percentage-label")) {
                            username_menu->insertBefore(badgeGDUtilBtn, cell->getChildByIDRecursive("percentage-label"));
                        } else {
                            username_menu->addChild(badgeGDUtilBtn);
                        }
                        username_menu->updateLayout();
                    }
                } else if (badge == 3) {
                    if (!cell->getChildByIDRecursive("gdutils-artist-badge")) {
                        auto badgeGDUtil = CCSprite::createWithSpriteFrameName("artistBadge.png"_spr);
                        badgeGDUtil->setScale(0.70f);
                        auto badgeGDUtilBtn = CCMenuItemSpriteExtra::create(
                            badgeGDUtil,
                            cell,
                            menu_selector(NewProfilePage::onGDUtilsArtistBadgePressed)
                        );

                        badgeGDUtilBtn->setID("gdutils-artist-badge");
                        if (cell->getChildByIDRecursive("percentage-label")) {
                            username_menu->insertBefore(badgeGDUtilBtn, cell->getChildByIDRecursive("percentage-label"));
                        } else {
                            username_menu->addChild(badgeGDUtilBtn);
                        }
                        username_menu->updateLayout();
                    }
                } else if (badge == 4) {
                    if (!cell->getChildByIDRecursive("gdutils-gdawards2023-badge")) {
                        auto badgeGDUtil = CCSprite::createWithSpriteFrameName("gdAwards2023Badge.png"_spr);
                        badgeGDUtil->setScale(0.70f);
                        auto badgeGDUtilBtn = CCMenuItemSpriteExtra::create(
                            badgeGDUtil,
                            cell,
                            menu_selector(NewProfilePage::onGDUtilsGDAwards2023BadgePressed)
                        );

                        badgeGDUtilBtn->setID("gdutils-gdawards2023-badge");
                        if (cell->getChildByIDRecursive("percentage-label")) {
                            username_menu->insertBefore(badgeGDUtilBtn, cell->getChildByIDRecursive("percentage-label"));
                        } else {
                            username_menu->addChild(badgeGDUtilBtn);
                        }
                        username_menu->updateLayout();
                    }
                } else if (badge == 5) {
                    if (!cell->getChildByIDRecursive("gdutils-ul-developer-badge")) {
                        auto badgeGDUtil = CCSprite::createWithSpriteFrameName("ul_dev_badge.png"_spr);
                        badgeGDUtil->setScale(0.70f);
                        auto badgeGDUtilBtn = CCMenuItemSpriteExtra::create(
                            badgeGDUtil,
                            cell,
                            menu_selector(NewProfilePage::onULDevBadgePressed)
                        );

                        badgeGDUtilBtn->setID("gdutils-ul-developer-badge");
                        if (cell->getChildByIDRecursive("percentage-label")) {
                            username_menu->insertBefore(badgeGDUtilBtn, cell->getChildByIDRecursive("percentage-label"));
                        } else {
                            username_menu->addChild(badgeGDUtilBtn);
                        }
                        username_menu->updateLayout();
                    }
                } else if (badge == 6) {
                    if (!cell->getChildByIDRecursive("gdutils-ul-officer-badge")) {
                        auto badgeGDUtil = CCSprite::createWithSpriteFrameName("ul_officer_badge.png"_spr);
                        badgeGDUtil->setScale(0.70f);
                        auto badgeGDUtilBtn = CCMenuItemSpriteExtra::create(
                            badgeGDUtil,
                            cell,
                            menu_selector(NewProfilePage::onULOfficerBadgePressed)
                        );

                        badgeGDUtilBtn->setID("gdutils-ul-officer-badge");
                        if (cell->getChildByIDRecursive("percentage-label")) {
                            username_menu->insertBefore(badgeGDUtilBtn, cell->getChildByIDRecursive("percentage-label"));
                        } else {
                            username_menu->addChild(badgeGDUtilBtn);
                        }
                        username_menu->updateLayout();
                    }
                } else if (badge == 7) {
                    if (!cell->getChildByIDRecursive("gdutils-ul-helper-badge")) {
                        auto badgeGDUtil = CCSprite::createWithSpriteFrameName("ul_helper_badge.png"_spr);
                        badgeGDUtil->setScale(0.70f);
                        auto badgeGDUtilBtn = CCMenuItemSpriteExtra::create(
                            badgeGDUtil,
                            cell,
                            menu_selector(NewProfilePage::onULHelperBadgePressed)
                        );

                        badgeGDUtilBtn->setID("gdutils-ul-helper-badge");
                        if (cell->getChildByIDRecursive("percentage-label")) {
                            username_menu->insertBefore(badgeGDUtilBtn, cell->getChildByIDRecursive("percentage-label"));
                        } else {
                            username_menu->addChild(badgeGDUtilBtn);
                        }
                        username_menu->updateLayout();
                    }
                } else if (badge == 8) {
                    if (!cell->getChildByIDRecursive("gdutils-supporter-badge")) {
                        auto badgeGDUtil = CCSprite::createWithSpriteFrameName("supporter_badge.png"_spr);
                        badgeGDUtil->setScale(0.70f);
                        auto badgeGDUtilBtn = CCMenuItemSpriteExtra::create(
                            badgeGDUtil,
                            cell,
                            menu_selector(NewProfilePage::onGDUtilsSupporterBadgePressed)
                        );

                        ccColor3B color = { 218, 35, 212 };
                        if (auto commentText = cell->getChildByIDRecursive("comment-text-label")) {
                            as<CCLabelBMFont*>(commentText)->setColor(color);
                            cell->m_comment->m_color = color;
                        }
                        if (auto commentText = cell->getChildByIDRecursive("comment-text-area")) {
                            TextArea* textArea = as<TextArea*>(commentText);
                            MultilineBitmapFont* bmFont = as<MultilineBitmapFont*>(textArea->getChildren()->objectAtIndex(0));
                            CCArray* children = bmFont->getChildren();
                            for (int i = 0; i < children->count(); i++) {
                                auto child = as<CCLabelBMFont*>(children->objectAtIndex(i));
                                child->setColor(color);
                            }
                        }

                        badgeGDUtilBtn->setID("gdutils-supporter-badge");
                        if (cell->getChildByIDRecursive("percentage-label")) {
                            username_menu->insertBefore(badgeGDUtilBtn, cell->getChildByIDRecursive("percentage-label"));
                        } else {
                            username_menu->addChild(badgeGDUtilBtn);
                        }
                        username_menu->updateLayout();
                    }
                } else if (badge == 9) {
                    if (!cell->getChildByIDRecursive("gdutils-gdawards2024-badge")) {
                        auto badgeGDUtil = CCSprite::createWithSpriteFrameName("gdAwards2024Badge.png"_spr);
                        badgeGDUtil->setScale(0.70f);
                        auto badgeGDUtilBtn = CCMenuItemSpriteExtra::create(
                            badgeGDUtil,
                            cell,
                            menu_selector(NewProfilePage::onGDUtilsGDAwards2024BadgePressed)
                        );

                        badgeGDUtilBtn->setID("gdutils-gdawards2024-badge");
                        if (cell->getChildByIDRecursive("percentage-label")) {
                            username_menu->insertBefore(badgeGDUtilBtn, cell->getChildByIDRecursive("percentage-label"));
                        } else {
                            username_menu->addChild(badgeGDUtilBtn);
                        }
                        username_menu->updateLayout();
                    }
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
                badgeBtn = CCMenuItemSpriteExtra::create(
                    modbadge,
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

        if (result_global.empty()) {
            m_fields->m_listener.bind([] (web::WebTask::Event* e) {
                if (web::WebResponse* res = e->getValue()) {
                    if (res->ok()) {
                        result_global = res->string().unwrapOrDefault();
                    }
                }
            });

            auto req = web::WebRequest();
            m_fields->m_listener.setFilter(req.get("https://clarifygdps.com/gdutils/gdutils_roles.php"));
        }

        badgeHandle(result_global, cell, cell->m_comment->m_commentID);
    }
};
