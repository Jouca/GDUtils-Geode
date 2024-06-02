#include "../includes.h"
#include <Geode/modify/GauntletSelectLayer.hpp>
#include <Geode/Geode.hpp>
#include "../Utils/ParticleManager.hpp"

class $modify(GauntletSelectLayer) {
    void setupGauntlets() {
        GauntletSelectLayer::setupGauntlets();

        auto list = this->getChildByIDRecursive("gauntlets-list");

        log::info("Gauntlet list: {}", list);

        if (list) {
            CCArray* pages = typeinfo_cast<CCArray*>(list->getChildByID("gauntlet-pages")->getChildren());

            for (int i = 0; i < pages->count(); i++) {
                auto page = typeinfo_cast<CCMenu*>(typeinfo_cast<CCLayer*>(pages->objectAtIndex(i))->getChildren()->objectAtIndex(0))->getChildren();

                for (int j = 0; j < page->count(); j++) {
                    CCMenuItemSpriteExtra* item = typeinfo_cast<CCMenuItemSpriteExtra*>(page->objectAtIndex(j));

                    // Handle spaces
                    if (j == 0) {
                        item->setPositionX(item->getPositionX() - 25.0f);
                    } else if (j == 2) {
                        item->setPositionX(item->getPositionX() + 25.0f);
                    }

                    GauntletNode* node = typeinfo_cast<GauntletNode*>(item->getChildByIDRecursive("gauntlet-node"));

                    CCLabelBMFont* label = typeinfo_cast<CCLabelBMFont*>(node->getChildByID("gauntlet-name-label"));
                    CCLabelBMFont* label_shadow = typeinfo_cast<CCLabelBMFont*>(node->getChildByID("gauntlet-name-shadow-label"));
                    CCLabelBMFont* label_gauntlet = typeinfo_cast<CCLabelBMFont*>(node->getChildByID("gauntlet-label"));
                    CCLabelBMFont* label_shadow_gauntlet = typeinfo_cast<CCLabelBMFont*>(node->getChildByID("gauntlet-shadow-label"));
                    CCScale9Sprite* bg = typeinfo_cast<CCScale9Sprite*>(node->getChildByID("background"));

                    CCNode* gauntlet_node = node->getChildByID("gauntlet-info-node");
                    CCSprite* gauntlet = typeinfo_cast<CCSprite*>(gauntlet_node->getChildByID("gauntlet-sprite"));
                    CCSprite* gauntlet_shadow = typeinfo_cast<CCSprite*>(gauntlet_node->getChildByID("gauntlet-shadow-sprite"));

                    bg->setVisible(false);
                    label->setVisible(false);
                    label_shadow->setVisible(false);
                    label_gauntlet->setVisible(false);
                    label_shadow_gauntlet->setVisible(false);
                    gauntlet->setVisible(false);
                    gauntlet_shadow->setVisible(false);

                    std::string gauntletName = label->getString();
                    if (gauntletName == "World") {
                        CCSprite* sprite = CCSprite::create(Mod::get()->expandSpriteName("world_gauntlet.png"));
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.270f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);
                    } else if (gauntletName == "Fantasy") {
                        CCSprite* sprite = CCSprite::create(Mod::get()->expandSpriteName("fantasy_gauntlet.png"));
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.225f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);

                        // Particle
                        /*CCParticleSystem* particle = createParticles(20, ccc3(255, 255, 35), ccc3(0, 132, 0), "2", "-1", "1.21", "0.30", "116", "0", "20", "20", "0", "0", "0", "0", "8", "8", "1", "0", "0", "0", "0", "1", "0", "1", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "particle_118_001.png");
                        node->addChild(particle);
                        particle->runAction(cocos2d::CCAction::create());*/
                    } else if (gauntletName == "Portal") {
                        CCSprite* sprite = CCSprite::create(Mod::get()->expandSpriteName("portal_gauntlet.png"));
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.225f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);
                    } else if (gauntletName == "Demon") {
                        CCSprite* sprite = CCSprite::create(Mod::get()->expandSpriteName("demon_gauntlet.png"));
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.275f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);
                    } else if (gauntletName == "Castle") {
                        CCSprite* sprite = CCSprite::create(Mod::get()->expandSpriteName("castle_gauntlet.png"));
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.275f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);
                    } else if (gauntletName == "Galaxy") {
                        CCSprite* sprite = CCSprite::create(Mod::get()->expandSpriteName("galaxy_gauntlet.png"));
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.695f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);
                    } else if (gauntletName == "Universe") {
                        CCSprite* sprite = CCSprite::create(Mod::get()->expandSpriteName("universe_gauntlet.png"));
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.270f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);
                    } else {
                        bg->setVisible(true);
                        label->setVisible(true);
                        label_shadow->setVisible(true);
                        label_gauntlet->setVisible(true);
                        label_shadow_gauntlet->setVisible(true);
                        gauntlet->setVisible(true);
                        gauntlet_shadow->setVisible(true);
                    }
                }
            }
        }
    }
};