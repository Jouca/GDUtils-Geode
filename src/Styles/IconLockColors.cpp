#include "../includes.h"
#include <Geode/modify/GJGarageLayer.hpp>
#include <Geode/Geode.hpp>
#ifndef GEODE_IS_IOS
class IconLockColors {
    public:
        void onP1(CCObject* pSender) {
            SimplePlayer* player = this->getPlayer(CCDirector::sharedDirector()->getRunningScene());

            GameManager* gm = GameManager::sharedState();
            player->setColor(gm->colorForIdx(gm->getPlayerColor()));
            player->setSecondColor(gm->colorForIdx(gm->getPlayerColor2()));
        };

        void onP2(CCObject* pSender) {
            SimplePlayer* player = this->getPlayer(CCDirector::sharedDirector()->getRunningScene());

            GameManager* gm = GameManager::sharedState();
            player->setColor(gm->colorForIdx(gm->getPlayerColor2()));
            player->setSecondColor(gm->colorForIdx(gm->getPlayerColor()));
        };

        void onGlow(CCObject* pSender) {
            SimplePlayer* player = this->getPlayer(CCDirector::sharedDirector()->getRunningScene());
            GameManager* gm = GameManager::sharedState();

            if (!player->m_hasGlowOutline) {
                player->setGlowOutline(gm->colorForIdx(gm->getPlayerGlowColor()));
            } else {
                player->disableGlowOutline();
            }
        };

        static SimplePlayer* getPlayer(CCScene* scene) {
            CCArray* children = scene->getChildren();

            for (int i = 0; i < children->count(); i++) {
                ItemInfoPopup* child = typeinfo_cast<ItemInfoPopup*>(children->objectAtIndex(i));

                if (child) {
                    CCArray* array_childrens = typeinfo_cast<CCNode*>(child->getChildren()->objectAtIndex(0))->getChildren();

                    for (int j = 0; j < array_childrens->count(); j++) {
                        GJItemIcon* node = typeinfo_cast<GJItemIcon*>(array_childrens->objectAtIndex(j));

                        if (node) {
                            SimplePlayer* player = typeinfo_cast<SimplePlayer*>(node->getChildren()->objectAtIndex(0));
                            return player;
                        }
                    }
                }
            }

            return nullptr;
        }

        static void createIconColor() {
            auto scene = CCDirector::sharedDirector()->getRunningScene();
            CCArray* children = scene->getChildren();

            for (int i = 0; i < children->count(); i++) {
                ItemInfoPopup* child = typeinfo_cast<ItemInfoPopup*>(children->objectAtIndex(i));

                if (child) {
                    CCArray* array_childrens = typeinfo_cast<CCNode*>(child->getChildren()->objectAtIndex(0))->getChildren();

                    SimplePlayer* player = getPlayer(CCDirector::sharedDirector()->getRunningScene());
                    GameManager* gm = GameManager::sharedState();
                    if (player) {
                        player->setColor(gm->colorForIdx(gm->getPlayerColor()));
                        player->setSecondColor(gm->colorForIdx(gm->getPlayerColor2()));
                        player->m_glowColor = gm->colorForIdx(gm->getPlayerGlowColor());

                        // Creating buttons
                        auto p1Spr = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
                        auto p1txt = CCLabelBMFont::create("P1", "bigFont.fnt");
                        p1txt->setScale(.525f);
                        p1txt->setPosition(15, 17);
                        p1Spr->addChild(p1txt);
                        p1Spr->setScale(.8f);
                        auto p1Btn = CCMenuItemSpriteExtra::create(
                            p1Spr,
                            child,
                            menu_selector(IconLockColors::onP1)
                        );
                        p1Btn->setPosition(-128, -40);
                        p1Btn->setID("p1-button"_spr);
                        child->m_buttonMenu->addChild(p1Btn);

                        auto p2Spr = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
                        auto p2txt = CCLabelBMFont::create("P2", "bigFont.fnt");
                        p2txt->setScale(.525f);
                        p2txt->setPosition(15, 17);
                        p2Spr->addChild(p2txt);
                        p2Spr->setScale(.8f);
                        auto p2Btn = CCMenuItemSpriteExtra::create(
                            p2Spr,
                            child,
                            menu_selector(IconLockColors::onP2)
                        );
                        p2Btn->setPosition(-99, -40);
                        p2Btn->setID("p2-button"_spr);
                        child->m_buttonMenu->addChild(p2Btn);

                        auto glowSpr = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
                        auto glowtxt = CCLabelBMFont::create("G", "bigFont.fnt");
                        glowtxt->setScale(.525f);
                        glowtxt->setPosition(15, 17);
                        glowSpr->addChild(glowtxt);
                        glowSpr->setScale(.8f);
                        auto glowBtn = CCMenuItemSpriteExtra::create(
                            glowSpr,
                            child,
                            menu_selector(IconLockColors::onGlow)
                        );
                        glowBtn->setPosition(-69, -40);
                        glowBtn->setID("glow-button"_spr);
                        child->m_buttonMenu->addChild(glowBtn);
                    }
                }
            }
        }
};

class $modify(GJGarageLayer) {
    void onSelect(cocos2d::CCObject* sender) {
        GJGarageLayer::onSelect(sender);

        if (!Mod::get()->template getSettingValue<bool>("previewIcons")) return;

        IconLockColors::createIconColor();
    }
};
#endif
