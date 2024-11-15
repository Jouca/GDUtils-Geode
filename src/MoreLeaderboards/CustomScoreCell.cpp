#include <Geode/modify/GJScoreCell.hpp>
#include <Geode/modify/Field.hpp>
#include "MoreLeaderboards.h"
#include "SelectRegion.h"
#include "LevelHelper.cpp"

#include <Geode/Geode.hpp>
using namespace geode::prelude;


class $modify(CustomScoreCell, GJScoreCell) {
    void loadFromScore(GJUserScore* score) {
        GJScoreCell::loadFromScore(score);

        CCLayer* layer = as<CCLayer*>(this->getChildren()->objectAtIndex(1));
        if (layer) {
            // Search for flag
            std::string data_flag = MoreLeaderboards::data_response_moreLB;

            if (data_flag == "") return;

            auto position = layer->getPosition();

            RowLayout* layout = RowLayout::create();
            layout->setGap(3);
            layout->setAxisAlignment(AxisAlignment::Start);
            layout->setCrossAxisOverflow(true);

            CCMenu* menu = CCMenu::create();
            menu->setContentSize(CCSize(283, 21.580f));
            menu->setPosition(206, 42);

            // Move child to menu with layout
            auto starsLabel = as<CCLabelBMFont*>(layer->getChildByIDRecursive("stars-label"));
            starsLabel->setLayoutOptions(
                AxisLayoutOptions::create()->setAutoScale(true)->setScaleLimits(0, 0.5F)->setScalePriority(1)
            );
            auto starsIcon = as<CCSprite*>(layer->getChildByIDRecursive("stars-icon"));
            starsIcon->setLayoutOptions(
                AxisLayoutOptions::create()->setAutoScale(true)->setScaleLimits(0, 0.5F)->setScalePriority(1)
            );
            auto moonsLabel = as<CCLabelBMFont*>(layer->getChildByIDRecursive("moons-label"));
            moonsLabel->setLayoutOptions(
                AxisLayoutOptions::create()->setAutoScale(true)->setScaleLimits(0, 0.5F)->setScalePriority(1)
            );
            auto moonsIcon = as<CCSprite*>(layer->getChildByIDRecursive("moons-icon"));
            moonsIcon->setLayoutOptions(
                AxisLayoutOptions::create()->setAutoScale(true)->setScaleLimits(0, 0.5F)->setScalePriority(1)
            );

            // Username control
            std::string playerName = as<CCLabelBMFont*>(as<CCMenuItemSpriteExtra*>(layer->getChildByIDRecursive("player-name"))->getChildren()->objectAtIndex(0))->getString();
            CCMenu* menu_playername = CCMenu::create();
            menu_playername->setContentSize(as<CCMenuItemSpriteExtra*>(layer->getChildByIDRecursive("player-name"))->getContentSize());
            menu_playername->setLayout(
                RowLayout::create()->setGap(3)->setAxisAlignment(AxisAlignment::Start)
            );
            auto playerNameBM = CCLabelBMFont::create(playerName.c_str(), "goldFont.fnt");
            auto playerNameBtn = CCMenuItemSpriteExtra::create(
                playerNameBM,
                this,
                (SEL_MenuHandler)&CustomScoreCell::onViewProfile
            );
            playerNameBtn->setLayoutOptions(
                AxisLayoutOptions::create()->setAutoScale(true)->setScaleLimits(0.5F, 0.8F)->setScalePriority(1)
            );

            std::vector<std::string> dataString = MoreLeaderboards::getWords(data_flag, "#");
            std::vector<std::string> users = MoreLeaderboards::getWords(dataString[0], "|");

            std::string count_diff = "0";

            while (users.size() > 0) {
                std::string user = users[0];

                std::vector<std::string> data_info = MoreLeaderboards::getWords(user, ":");
                while (data_info.size() > 0) {
                    std::string id = data_info[0];
                    std::string info = data_info[1];

                    if (id == "1") {
                        if (info != playerName) break;
                    }
                    if (id == "9989") {
                        count_diff = info;
                    }
                    if (id == "9990") {
                        if (info == "XX") break;
                        auto flagSpr = CCSprite::createWithSpriteFrameName((""_spr+info+".png").c_str());
                        flagSpr->setPosition({ 20, 15 });
                        flagSpr->setScaleY(20 / flagSpr->getContentHeight() * 0.50);
                        flagSpr->setScaleX(std::min(1.0f, 30 / flagSpr->getContentWidth()));
                        flagSpr->setLayoutOptions(
                            AxisLayoutOptions::create()->setAutoScale(true)->setScalePriority(1)
                        );
                        menu_playername->addChild(flagSpr);
                    }

                    data_info.erase(data_info.begin());
                    data_info.erase(data_info.begin());
                }
                
                users.erase(users.begin());
            };

            menu_playername->addChild(playerNameBtn);
            menu_playername->updateLayout();
        
            OPT(layer->getChildByID("stars-label"))->removeFromParentAndCleanup(true);
            OPT(layer->getChildByID("stars-icon"))->removeFromParentAndCleanup(true);
            OPT(layer->getChildByID("moons-label"))->removeFromParentAndCleanup(true);
            OPT(layer->getChildByID("moons-icon"))->removeFromParentAndCleanup(true);
            OPT(layer->getChildByID("main-menu"))->removeFromParentAndCleanup(true);

            menu->addChild(menu_playername);
            menu->addChild(starsLabel);
            menu->addChild(starsIcon);
            menu->addChild(moonsLabel);
            menu->addChild(moonsIcon);

            menu->setLayout(layout);
            menu->updateLayout();

            // get main layer
            CCLayer* mainLayer = nullptr;
            CCObject* obj;
            CCARRAY_FOREACH(this->getChildren(), obj) {
                CCLayer* search = typeinfo_cast<CCLayer*>(obj);
                if (search != nullptr && search->getChildrenCount() > 2) {
                    mainLayer = search;
                    break;
                }
            }
            auto statsMenu = mainLayer->getChildByIDRecursive("stats-menu");

            // BetterProgression part
            Loader* loader = Loader::get();
            if (!loader->isModLoaded("itzkiba.better_progression") && MoreLeaderboards::g_tab == StatsListType::BetterProgression) {
                int currentEXP = LevelHelper::calculateTotalEXP(score);
                int currentLevel = LevelHelper::getLevelFromEXP(currentEXP);

                auto badgeSpr = SpriteHelper::createFullSprite(currentLevel);
                badgeSpr->setPosition({75, 17.5});
                badgeSpr->setScale(0.62);

                mainLayer->addChild(badgeSpr);

                statsMenu->setScale(0.83);
                statsMenu->setPositionX(statsMenu->getPositionX() + 13.f);
            }

            // Classic & Platformer Demons part
            auto classic_sprite = CCSprite::createWithSpriteFrameName("star_small01_001.png");
            classic_sprite->setPositionX(30);
            classic_sprite->setScale(1.2f);

            auto platformer_sprite = CCSprite::createWithSpriteFrameName("moon_small01_001.png");
            platformer_sprite->setPositionX(30);
            platformer_sprite->setScale(1.2f);

            auto pointercrate = CCSprite::createWithSpriteFrameName("diffIcon_06_btn_001.png");
            auto pointercrate_text = CCLabelBMFont::create("PC", "goldFont.fnt");
            pointercrate_text->setPosition(28, 0);
            pointercrate_text->setScale(0.5f);
            pointercrate->addChild(pointercrate_text);

            auto easydemon_sprite_classic = CCSprite::createWithSpriteFrameName("diffIcon_07_btn_001.png");
            easydemon_sprite_classic->addChild(classic_sprite);
            auto mediumdemon_sprite_classic = CCSprite::createWithSpriteFrameName("diffIcon_08_btn_001.png");
            mediumdemon_sprite_classic->addChild(classic_sprite);
            auto harddemon_sprite_classic = CCSprite::createWithSpriteFrameName("diffIcon_06_btn_001.png");
            harddemon_sprite_classic->addChild(classic_sprite);
            auto insane_sprite_classic = CCSprite::createWithSpriteFrameName("diffIcon_09_btn_001.png");
            insane_sprite_classic->addChild(classic_sprite);
            auto extreme_sprite_classic = CCSprite::createWithSpriteFrameName("diffIcon_10_btn_001.png");
            extreme_sprite_classic->addChild(classic_sprite);

            auto easydemon_sprite_platformer = CCSprite::createWithSpriteFrameName("diffIcon_07_btn_001.png");
            easydemon_sprite_platformer->addChild(platformer_sprite);
            auto mediumdemon_sprite_platformer = CCSprite::createWithSpriteFrameName("diffIcon_08_btn_001.png");
            mediumdemon_sprite_platformer->addChild(platformer_sprite);
            auto harddemon_sprite_platformer = CCSprite::createWithSpriteFrameName("diffIcon_06_btn_001.png");
            harddemon_sprite_platformer->addChild(platformer_sprite);
            auto insane_sprite_platformer = CCSprite::createWithSpriteFrameName("diffIcon_09_btn_001.png");
            insane_sprite_platformer->addChild(platformer_sprite);
            auto extreme_sprite_platformer = CCSprite::createWithSpriteFrameName("diffIcon_10_btn_001.png");
            extreme_sprite_platformer->addChild(platformer_sprite);

            // Change Z orders
            statsMenu->getChildByIDRecursive("diamonds-label")->setZOrder(0);
            statsMenu->getChildByIDRecursive("diamonds-icon")->setZOrder(1);
            statsMenu->getChildByIDRecursive("coins-label")->setZOrder(2);
            statsMenu->getChildByIDRecursive("coins-icon")->setZOrder(3);
            statsMenu->getChildByIDRecursive("user-coins-label")->setZOrder(4);
            statsMenu->getChildByIDRecursive("user-coins-icon")->setZOrder(5);
            statsMenu->getChildByIDRecursive("demons-label")->setZOrder(6);
            statsMenu->getChildByIDRecursive("demons-icon")->setZOrder(7);
            auto cp_label = statsMenu->getChildByIDRecursive("creator-points-label");
            if (cp_label) cp_label->setZOrder(8);
            auto cp_icon = statsMenu->getChildByIDRecursive("creator-points-icon");
            if (cp_icon) cp_icon->setZOrder(9);

            auto demon_icon = mainLayer->getChildByIDRecursive("demons-icon");
            auto demon_icon_pos = demon_icon->getPosition();
            auto demon_icon_scale = demon_icon->getScale();
            auto demon_icon_z = demon_icon->getZOrder();
            auto demon_icon_tag = demon_icon->getTag();
            auto demon_icon_layout = demon_icon->getLayoutOptions();
            auto demon_label = mainLayer->getChildByIDRecursive("demons-label");
            auto demon_label_pos = demon_label->getPosition();
            auto demon_label_scale = demon_label->getScale();
            auto demon_label_z = demon_label->getZOrder();
            auto demon_label_tag = demon_label->getTag();
            auto demon_label_layout = demon_label->getLayoutOptions();

            switch (MoreLeaderboards::g_tab) {
                case StatsListType::platformerDemonsEasy:
                    demon_icon->removeFromParent();
                    demon_icon = easydemon_sprite_platformer;
                    demon_icon->setPosition(demon_icon_pos);
                    demon_icon->setScale(demon_icon_scale);
                    demon_icon->setLayoutOptions(demon_icon_layout);
                    demon_icon->setTag(demon_icon_tag);
                    demon_icon->setZOrder(demon_icon_z);
                    statsMenu->addChild(easydemon_sprite_platformer);

                    demon_label->removeFromParent();
                    demon_label = CCLabelBMFont::create(count_diff.c_str(), "bigFont.fnt");
                    demon_label->setPosition(demon_label_pos);
                    demon_label->setScale(demon_label_scale);
                    demon_label->setZOrder(demon_label_z);
                    demon_label->setTag(demon_label_tag);
                    demon_label->setLayoutOptions(demon_label_layout);
                    statsMenu->addChild(demon_label);
                    break;
                case StatsListType::platformerDemonsMedium:
                    demon_icon->removeFromParent();
                    demon_icon = mediumdemon_sprite_platformer;
                    demon_icon->setPosition(demon_icon_pos);
                    demon_icon->setScale(demon_icon_scale);
                    demon_icon->setLayoutOptions(demon_icon_layout);
                    demon_icon->setTag(demon_icon_tag);
                    demon_icon->setZOrder(demon_icon_z);
                    statsMenu->addChild(mediumdemon_sprite_platformer);

                    demon_label->removeFromParent();
                    demon_label = CCLabelBMFont::create(count_diff.c_str(), "bigFont.fnt");
                    demon_label->setPosition(demon_label_pos);
                    demon_label->setScale(demon_label_scale);
                    demon_label->setZOrder(demon_label_z);
                    demon_label->setTag(demon_label_tag);
                    demon_label->setLayoutOptions(demon_label_layout);
                    statsMenu->addChild(demon_label);
                    break;
                case StatsListType::platformerDemonsHard:
                    demon_icon->removeFromParent();
                    demon_icon = harddemon_sprite_platformer;
                    demon_icon->setPosition(demon_icon_pos);
                    demon_icon->setScale(demon_icon_scale);
                    demon_icon->setLayoutOptions(demon_icon_layout);
                    demon_icon->setTag(demon_icon_tag);
                    demon_icon->setZOrder(demon_icon_z);
                    statsMenu->addChild(harddemon_sprite_platformer);

                    demon_label->removeFromParent();
                    demon_label = CCLabelBMFont::create(count_diff.c_str(), "bigFont.fnt");
                    demon_label->setPosition(demon_label_pos);
                    demon_label->setScale(demon_label_scale);
                    demon_label->setZOrder(demon_label_z);
                    demon_label->setTag(demon_label_tag);
                    demon_label->setLayoutOptions(demon_label_layout);
                    statsMenu->addChild(demon_label);
                    break;
                case StatsListType::platformerDemonsInsane:
                    demon_icon->removeFromParent();
                    demon_icon = insane_sprite_platformer;
                    demon_icon->setPosition(demon_icon_pos);
                    demon_icon->setScale(demon_icon_scale);
                    demon_icon->setLayoutOptions(demon_icon_layout);
                    demon_icon->setTag(demon_icon_tag);
                    demon_icon->setZOrder(demon_icon_z);
                    statsMenu->addChild(insane_sprite_platformer);

                    demon_label->removeFromParent();
                    demon_label = CCLabelBMFont::create(count_diff.c_str(), "bigFont.fnt");
                    demon_label->setPosition(demon_label_pos);
                    demon_label->setScale(demon_label_scale);
                    demon_label->setZOrder(demon_label_z);
                    demon_label->setTag(demon_label_tag);
                    demon_label->setLayoutOptions(demon_label_layout);
                    statsMenu->addChild(demon_label);
                    break;
                case StatsListType::platformerDemonsExtreme:
                    demon_icon->removeFromParent();
                    demon_icon = extreme_sprite_platformer;
                    demon_icon->setPosition(demon_icon_pos);
                    demon_icon->setScale(demon_icon_scale);
                    demon_icon->setLayoutOptions(demon_icon_layout);
                    demon_icon->setTag(demon_icon_tag);
                    demon_icon->setZOrder(demon_icon_z);
                    statsMenu->addChild(extreme_sprite_platformer);

                    demon_label->removeFromParent();
                    demon_label = CCLabelBMFont::create(count_diff.c_str(), "bigFont.fnt");
                    demon_label->setPosition(demon_label_pos);
                    demon_label->setScale(demon_label_scale);
                    demon_label->setZOrder(demon_label_z);
                    demon_label->setTag(demon_label_tag);
                    demon_label->setLayoutOptions(demon_label_layout);
                    statsMenu->addChild(demon_label);
                    break;
                case StatsListType::classicDemonsEasy:
                    demon_icon->removeFromParent();
                    demon_icon = easydemon_sprite_classic;
                    demon_icon->setPosition(demon_icon_pos);
                    demon_icon->setScale(demon_icon_scale);
                    demon_icon->setLayoutOptions(demon_icon_layout);
                    demon_icon->setTag(demon_icon_tag);
                    demon_icon->setZOrder(demon_icon_z);
                    statsMenu->addChild(easydemon_sprite_classic);

                    demon_label->removeFromParent();
                    demon_label = CCLabelBMFont::create(count_diff.c_str(), "bigFont.fnt");
                    demon_label->setPosition(demon_label_pos);
                    demon_label->setScale(demon_label_scale);
                    demon_label->setZOrder(demon_label_z);
                    demon_label->setTag(demon_label_tag);
                    demon_label->setLayoutOptions(demon_label_layout);
                    statsMenu->addChild(demon_label);
                    break;
                case StatsListType::classicDemonsMedium:
                    demon_icon->removeFromParent();
                    demon_icon = mediumdemon_sprite_classic;
                    demon_icon->setPosition(demon_icon_pos);
                    demon_icon->setScale(demon_icon_scale);
                    demon_icon->setLayoutOptions(demon_icon_layout);
                    demon_icon->setTag(demon_icon_tag);
                    demon_icon->setZOrder(demon_icon_z);
                    statsMenu->addChild(mediumdemon_sprite_classic);

                    demon_label->removeFromParent();
                    demon_label = CCLabelBMFont::create(count_diff.c_str(), "bigFont.fnt");
                    demon_label->setPosition(demon_label_pos);
                    demon_label->setScale(demon_label_scale);
                    demon_label->setZOrder(demon_label_z);
                    demon_label->setTag(demon_label_tag);
                    demon_label->setLayoutOptions(demon_label_layout);
                    statsMenu->addChild(demon_label);
                    break;
                case StatsListType::classicDemonsHard:
                    demon_icon->removeFromParent();
                    demon_icon = harddemon_sprite_classic;
                    demon_icon->setPosition(demon_icon_pos);
                    demon_icon->setScale(demon_icon_scale);
                    demon_icon->setLayoutOptions(demon_icon_layout);
                    demon_icon->setTag(demon_icon_tag);
                    demon_icon->setZOrder(demon_icon_z);
                    statsMenu->addChild(harddemon_sprite_classic);

                    demon_label->removeFromParent();
                    demon_label = CCLabelBMFont::create(count_diff.c_str(), "bigFont.fnt");
                    demon_label->setPosition(demon_label_pos);
                    demon_label->setScale(demon_label_scale);
                    demon_label->setZOrder(demon_label_z);
                    demon_label->setTag(demon_label_tag);
                    demon_label->setLayoutOptions(demon_label_layout);
                    statsMenu->addChild(demon_label);
                    break;
                case StatsListType::classicDemonsInsane:
                    demon_icon->removeFromParent();
                    demon_icon = insane_sprite_classic;
                    demon_icon->setPosition(demon_icon_pos);
                    demon_icon->setScale(demon_icon_scale);
                    demon_icon->setLayoutOptions(demon_icon_layout);
                    demon_icon->setTag(demon_icon_tag);
                    demon_icon->setZOrder(demon_icon_z);
                    statsMenu->addChild(insane_sprite_classic);

                    demon_label->removeFromParent();
                    demon_label = CCLabelBMFont::create(count_diff.c_str(), "bigFont.fnt");
                    demon_label->setPosition(demon_label_pos);
                    demon_label->setScale(demon_label_scale);
                    demon_label->setZOrder(demon_label_z);
                    demon_label->setTag(demon_label_tag);
                    demon_label->setLayoutOptions(demon_label_layout);
                    statsMenu->addChild(demon_label);
                    break;
                case StatsListType::classicDemonsExtreme:
                    demon_icon->removeFromParent();
                    demon_icon = extreme_sprite_classic;
                    demon_icon->setPosition(demon_icon_pos);
                    demon_icon->setScale(demon_icon_scale);
                    demon_icon->setLayoutOptions(demon_icon_layout);
                    demon_icon->setTag(demon_icon_tag);
                    demon_icon->setZOrder(demon_icon_z);
                    statsMenu->addChild(extreme_sprite_classic);

                    demon_label->removeFromParent();
                    demon_label = CCLabelBMFont::create(count_diff.c_str(), "bigFont.fnt");
                    demon_label->setPosition(demon_label_pos);
                    demon_label->setScale(demon_label_scale);
                    demon_label->setZOrder(demon_label_z);
                    demon_label->setTag(demon_label_tag);
                    demon_label->setLayoutOptions(demon_label_layout);
                    statsMenu->addChild(demon_label);
                    break;
                case StatsListType::Pointercrate:
                    demon_icon->removeFromParent();
                    demon_icon = pointercrate;
                    demon_icon->setPosition(demon_icon_pos);
                    demon_icon->setScale(demon_icon_scale);
                    demon_icon->setLayoutOptions(demon_icon_layout);
                    demon_icon->setTag(demon_icon_tag);
                    demon_icon->setZOrder(demon_icon_z);
                    statsMenu->addChild(pointercrate);

                    demon_label->removeFromParent();
                    demon_label = CCLabelBMFont::create(count_diff.c_str(), "bigFont.fnt");
                    demon_label->setPosition(demon_label_pos);
                    demon_label->setScale(demon_label_scale);
                    demon_label->setZOrder(demon_label_z);
                    demon_label->setTag(demon_label_tag);
                    demon_label->setLayoutOptions(demon_label_layout);
                    statsMenu->addChild(demon_label);
                    break;
            }

            statsMenu->updateLayout();

            layer->addChild(menu);
            layer->updateLayout();
        }
    }
};
