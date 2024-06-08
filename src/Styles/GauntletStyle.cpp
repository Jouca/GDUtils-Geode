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

                        // Show gauntlets sprite again
                        gauntlet->setVisible(true);
                        gauntlet->setPosition({-1, 22});
                        gauntlet->setScale(1.1f);
                        gauntlet_shadow->setVisible(true);
                        gauntlet_shadow->setPosition({-1, 22});
                        gauntlet_shadow->setScale(1.1f);
                        gauntlet_node->setZOrder(2);

                        // Particle
                        CCParticleSystemQuad* particle = GameToolbox::particleFromString("66a-1a3a0.3a20a90a27a11a0a37a38a0a0a0a0a0a0a5a1a0a0a0.694118a0a0.45098a0a0.054902a0a1a0a2a1a0a0a0.509804a0a0.172549a0a0a0a1a0a0.5a0a1a0a0a0a0a0a0a0a0a2a1a0a0a0a28a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle);
                        particle->setPosition(gauntlet->getPosition());
                        particle->setZOrder(1);
                        CCParticleSystemQuad* particle2 = GameToolbox::particleFromString("29a-1a3a0.3a8a90a27a11a0a37a38a0a0a0a0a0a0a50a1a0a0a0.0509804a0a0.0392157a0a0.0352941a0a1a0a50a1a0a0a0.0509804a0a0.0392157a0a0.0352941a0a1a0a0.5a0a1a0a0a0a0a0a0a0a0a2a1a0a0a0a46a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle2);
                        particle2->setPosition(gauntlet->getPosition());
                        particle2->setZOrder(1);
                        CCParticleSystemQuad* particle3 = GameToolbox::particleFromString("31a-1a1a0.3a23a90a90a29a0a11a0a0a0a0a0a0a0a33a1a89a149a0.454902a0a0.321569a0a0.152941a0a1a0a50a1a145a68a0.454902a0a0.321569a0a0.152941a0a1a0a0.5a0a0.5a0a0a0a0a0a0a0a1a2a1a0a0a0a192a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle3);
                        particle3->setPosition(gauntlet->getPosition());
                        particle3->setZOrder(1);
                        CCParticleSystemQuad* particle4 = GameToolbox::particleFromString("30a-1a2a0.3a13a90a8a21a0a8a0a0a45a0a0a0a0a40a1a89a149a0.396078a0a0.27451a0a0.14902a0a1a0a24a1a145a68a0.141176a0a0.0352941a0a0.0313726a0a1a0a0.5a0a0.5a0a0a0a0a0a0a0a0a2a1a0a0a0a46a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle4);
                        particle4->setPosition(gauntlet->getPosition());
                        particle4->setZOrder(1);
                        CCParticleSystemQuad* particle5 = GameToolbox::particleFromString("30a-1a1a0.3a13a90a8a21a0a0a0a0a45a0a0a0a0a21a1a89a149a0.894118a0a0.631373a0a0.345098a0a1a0a6a1a145a68a0.894118a0a0.631373a0a0.345098a0a1a0a0.5a0a0.5a0a0a0a0a0a0a0a0a2a1a0a0a0a183a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle5);
                        particle5->setPosition(gauntlet->getPosition());
                        particle5->setZOrder(1);
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
                    } else if (gauntletName == "Cursed") {
                        CCSprite* sprite = CCSprite::create(Mod::get()->expandSpriteName("cursed_gauntlet.png"));
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