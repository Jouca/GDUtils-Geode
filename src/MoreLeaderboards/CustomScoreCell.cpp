#include "../includes.h"
#include <Geode/modify/GJScoreCell.hpp>
#include <Geode/modify/Field.hpp>
#include "../includes.h"
#include "MoreLeaderboards.h"
#include "SelectRegion.h"


class $modify(CustomScoreCell, GJScoreCell) {
    GJUserScore* score_registered = nullptr;

    void loadFromScore(GJUserScore* score) {
        GJScoreCell::loadFromScore(score);

        if (score_registered) return;
        score_registered = score;

        auto scene = CCDirector::sharedDirector()->getRunningScene();
        auto layer = scene->getChildren()->objectAtIndex(0);
        auto layerName = misc::getNodeName(layer);

        if (layerName == "MoreLeaderboards") {
            CCLayer* layer = as<CCLayer*>(this->getChildren()->objectAtIndex(1));
            if (layer) {
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
                    AxisLayoutOptions::create()->setAutoScale(true)->setMinScale(0)->setMaxScale(0.5f)->setScalePriority(1)
                );
                auto starsIcon = as<CCSprite*>(layer->getChildByIDRecursive("stars-icon"));
                starsIcon->setLayoutOptions(
                    AxisLayoutOptions::create()->setAutoScale(true)->setMinScale(0)->setMaxScale(0.5f)->setScalePriority(1)
                );
                auto moonsLabel = as<CCLabelBMFont*>(layer->getChildByIDRecursive("moons-label"));
                moonsLabel->setLayoutOptions(
                    AxisLayoutOptions::create()->setAutoScale(true)->setMinScale(0)->setMaxScale(0.5f)->setScalePriority(1)
                );
                auto moonsIcon = as<CCSprite*>(layer->getChildByIDRecursive("moons-icon"));
                moonsIcon->setLayoutOptions(
                    AxisLayoutOptions::create()->setAutoScale(true)->setMinScale(0)->setMaxScale(0.5f)->setScalePriority(1)
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
                    AxisLayoutOptions::create()->setAutoScale(true)->setMinScale(0.5f)->setMaxScale(0.8f)->setScalePriority(1)
                );

                // Search for flag
                std::string data_flag = MoreLeaderboards::data_response_moreLB;
                std::vector<std::string> dataString = MoreLeaderboards::getWords(data_flag, "#");
                std::vector<std::string> users = MoreLeaderboards::getWords(dataString[0], "|");

                while (users.size() > 0) {
                    std::string user = users[0];

                    std::vector<std::string> data_info = MoreLeaderboards::getWords(user, ":");
                    while (data_info.size() > 0) {
                        std::string id = data_info[0];
                        std::string info = data_info[1];

                        if (id == "1") {
                            if (info != playerName) break;
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

                layer->addChild(menu);
                layer->updateLayout();
            }
        }
    }
};