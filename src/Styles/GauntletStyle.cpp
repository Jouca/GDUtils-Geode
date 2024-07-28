#include "../includes.h"
#include <Geode/modify/GauntletSelectLayer.hpp>
#include <Geode/Geode.hpp>
#include "../Utils/ParticleManager.hpp"

class $modify(CustomGauntletSelectLayer, GauntletSelectLayer) {
    void reverseGauntlet(CCNode* node) {
        CCNode* gauntlet_node = node->getChildByID("gauntlet-info-node");

        CCSprite* checkmark = typeinfo_cast<CCSprite*>(node->getChildByID("checkmark-sprite"));
        if (checkmark) {
            checkmark->setPositionY(78);

            CCSprite* checkmark_shadow = typeinfo_cast<CCSprite*>(node->getChildByID("checkmark-shadow-sprite"));
            checkmark_shadow->setPositionY(78);
        }

        CCLabelBMFont* label_reward = typeinfo_cast<CCLabelBMFont*>(gauntlet_node->getChildByID("reward-label"));
        if (label_reward) {
            label_reward->setPositionY(50);
            CCLabelBMFont* label_reward_shadow = typeinfo_cast<CCLabelBMFont*>(gauntlet_node->getChildByID("reward-shadow-label"));
            label_reward_shadow->setPositionY(50);

            CCSprite* chest = typeinfo_cast<CCSprite*>(gauntlet_node->getChildByID("chest-sprite"));
            chest->setPositionY(80);
            chest->setFlipY(true);
            CCSprite* chest_shadow = typeinfo_cast<CCSprite*>(gauntlet_node->getChildByID("chest-shadow-sprite"));
            chest_shadow->setPositionY(80);
            chest_shadow->setFlipY(true);
        }

        CCLabelBMFont* label_progress = typeinfo_cast<CCLabelBMFont*>(gauntlet_node->getChildByID("gauntlet-progress-label"));
        label_progress->setPositionY(29);

        CCLabelBMFont* label_progress_shadow = typeinfo_cast<CCLabelBMFont*>(gauntlet_node->getChildByID("gauntlet-progress-shadow-label"));
        label_progress_shadow->setPositionY(29);
    }

    void onInfoBtn(CCObject* pSender) {
        FLAlertLayer::create(
            nullptr,
            "Gauntlets redesign",
            R"text(
    All <cl>new designs for Gauntlets</c> are made by <cy>Zidnes</c> and <cy>brift</c>.
    <cl>Particles</c> are made by <cy>GammaReXx</c>.

    Thanks for their contributions!
            )text",
            "OK",
            nullptr,
            300.0f
        )->show();
    }

    void setupGauntlets() {
        GauntletSelectLayer::setupGauntlets();

        if (!Mod::get()->getSettingValue<bool>("gauntletDesign")) return;

        CCSprite* infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        CCMenuItemSpriteExtra* infoBtn = CCMenuItemSpriteExtra::create(
            infoSpr,
            this,
            menu_selector(CustomGauntletSelectLayer::onInfoBtn)
        );

        CCSprite* bottom_corner_right = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("bottom-right-corner"));
        if (bottom_corner_right) {
            infoBtn->setPosition(bottom_corner_right->getPosition());
            infoBtn->setScale(.8f);
            infoBtn->setZOrder(10);
            infoBtn->setColor(cocos2d::ccc3(236, 149, 19));

            CCMenu* menu_btr = typeinfo_cast<CCMenu*>(this->getChildByIDRecursive("bottom-right-menu"));
            menu_btr->addChild(infoBtn);
            menu_btr->updateLayout();

            CCParticleSystemQuad* particle_info = GameToolbox::particleFromString("26a-1a1.66a0.3a30a90a180a14a0a1a1a0a7a0a0a0a0a2a1a0a0a1a0a0.568627a0a0.12549a0a1a0a1a1a0a0a1a0a0.501961a0a0a0a1a0a0a0a0.46a0a0a0a0a0a0a0a0a2a1a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
            infoBtn->addChild(particle_info);
            particle_info->setPosition(infoSpr->getPosition());
            particle_info->setZOrder(9);
        }

        auto list = this->getChildByIDRecursive("gauntlets-list");

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
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("world_gauntlet.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.270f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);

                        // Show gauntlets sprite again
                        gauntlet->setVisible(true);
                        gauntlet->setPosition({1, 16});
                        gauntlet->setScale(1);
                        gauntlet_node->setZOrder(3);

                        // Particle
                        CCParticleSystemQuad* particle = GameToolbox::particleFromString("22a-1a3a0a20a90a0a29a0a0a0a0a0a0a0a0a0a7a0a0a0a0.729412a0a0.352941a0a0.796078a0a1a0a7a0a0a0a0.729412a0a0.352941a0a0.796078a0a1a0a0.4a0a0.4a0a35a0a35a0a-50a0a1a2a1a0a0a0a159a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle);
                        particle->setPosition({10, 24});
                        particle->setZOrder(1);
                        particle->setScaleX(1.310f);
                        particle->setScaleY(2.610f);
                        CCParticleSystemQuad* particle2 = GameToolbox::particleFromString("22a-1a3a0a20a90a0a29a0a0a0a0a0a0a0a0a0a7a0a0a0a0.34902a0a0.466667a0a0.792157a0a1a0a7a0a0a0a0.34902a0a0.466667a0a0.792157a0a1a0a0.4a0a0.4a0a35a0a35a0a-50a0a1a2a1a0a0a0a159a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle2);
                        particle2->setPosition({6, 24});
                        particle2->setZOrder(1);
                        particle2->setScaleX(1.310f);
                        particle2->setScaleY(2.610f);
                        CCParticleSystemQuad* particle3 = GameToolbox::particleFromString("22a-1a3a0a20a90a0a29a0a0a0a0a0a0a0a0a0a7a0a0a0a0.360784a0a0.854902a0a0.368627a0a1a0a7a0a0a0a0.360784a0a0.854902a0a0.368627a0a1a0a0.4a0a0.4a0a35a0a35a0a-50a0a1a2a1a0a0a0a159a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle3);
                        particle3->setPosition({2, 24});
                        particle3->setZOrder(1);
                        particle3->setScaleX(1.310f);
                        particle3->setScaleY(2.610f);
                        CCParticleSystemQuad* particle4 = GameToolbox::particleFromString("22a-1a3a0a20a90a0a29a0a0a0a0a0a0a0a0a0a7a0a0a0a0.866667a0a0.839216a0a0.247059a0a1a0a7a0a0a0a0.866667a0a0.839216a0a0.247059a0a1a0a0.4a0a0.4a0a35a0a35a0a-50a0a1a2a1a0a0a0a159a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle4);
                        particle4->setPosition({-2, 24});
                        particle4->setZOrder(1);
                        particle4->setScaleX(1.310f);
                        particle4->setScaleY(2.610f);
                        CCParticleSystemQuad* particle5 = GameToolbox::particleFromString("22a-1a3a0a20a90a0a29a0a0a0a0a0a0a0a0a0a7a0a0a0a0.866667a0a0.294118a0a0.247059a0a1a0a7a0a0a0a0.866667a0a0.294118a0a0.247059a0a1a0a0.4a0a0.4a0a35a0a35a0a-50a0a1a2a1a0a0a0a159a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle5);
                        particle5->setPosition({-6, 24});
                        particle5->setZOrder(1);
                        particle5->setScaleX(1.310f);
                        particle5->setScaleY(2.610f);
                        CCParticleSystemQuad* particle6 = GameToolbox::particleFromString("22a-1a3a0a20a90a0a29a0a0a0a0a0a0a0a0a0a7a0a0a0a0.729412a0a0.352941a0a0.796078a0a1a0a7a0a0a0a0.729412a0a0.352941a0a0.796078a0a1a0a0.4a0a0.4a0a35a0a35a0a-50a0a1a2a1a0a0a0a159a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle6);
                        particle6->setPosition({-9, 24});
                        particle6->setZOrder(1);
                        particle6->setScaleX(-1.310f);
                        particle6->setScaleY(2.610f);
                        CCParticleSystemQuad* particle7 = GameToolbox::particleFromString("22a-1a3a0a20a90a0a29a0a0a0a0a0a0a0a0a0a7a0a0a0a0.34902a0a0.466667a0a0.792157a0a1a0a7a0a0a0a0.34902a0a0.466667a0a0.792157a0a1a0a0.4a0a0.4a0a35a0a35a0a-50a0a1a2a1a0a0a0a159a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle7);
                        particle7->setPosition({-5, 24});
                        particle7->setZOrder(1);
                        particle7->setScaleX(-1.310f);
                        particle7->setScaleY(2.610f);
                        CCParticleSystemQuad* particle8 = GameToolbox::particleFromString("22a-1a3a0a20a90a0a29a0a0a0a0a0a0a0a0a0a7a0a0a0a0.360784a0a0.854902a0a0.368627a0a1a0a7a0a0a0a0.360784a0a0.854902a0a0.368627a0a1a0a0.4a0a0.4a0a35a0a35a0a-50a0a1a2a1a0a0a0a159a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle8);
                        particle8->setPosition({-1, 24});
                        particle8->setZOrder(1);
                        particle8->setScaleX(-1.310f);
                        particle8->setScaleY(2.610f);
                        CCParticleSystemQuad* particle9 = GameToolbox::particleFromString("22a-1a3a0a20a90a0a29a0a0a0a0a0a0a0a0a0a7a0a0a0a0.866667a0a0.839216a0a0.247059a0a1a0a7a0a0a0a0.866667a0a0.839216a0a0.247059a0a1a0a0.4a0a0.4a0a35a0a35a0a-50a0a1a2a1a0a0a0a159a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle9);
                        particle9->setPosition({3, 24});
                        particle9->setZOrder(1);
                        particle9->setScaleX(-1.310f);
                        particle9->setScaleY(2.610f);
                        CCParticleSystemQuad* particle10 = GameToolbox::particleFromString("22a-1a3a0a20a90a0a29a0a0a0a0a0a0a0a0a0a7a0a0a0a0.866667a0a0.294118a0a0.247059a0a1a0a7a0a0a0a0.866667a0a0.294118a0a0.247059a0a1a0a0.4a0a0.4a0a35a0a35a0a-50a0a1a2a1a0a0a0a159a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle10);
                        particle10->setPosition({7, 24});
                        particle10->setZOrder(1);
                        particle10->setScaleX(-1.310f);
                        particle10->setScaleY(2.610f);
                        CCParticleSystemQuad* particle11 = GameToolbox::particleFromString("38a-1a1.5a0.3a21a90a90a0a0a31a69a0a0a0a0a0a0a2a1a65a57a1a0a1a0a1a0a1a0a5a1a0a0a1a0a1a0a1a0a1a0a0.2a0a0.4a0a0a0a0a0a0a0a0a2a1a0a0a0a169a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle11);
                        particle11->setPosition({1, 6});
                        particle11->setZOrder(1);
                        particle11->setScaleX(1.710f);
                        particle11->setScaleY(1.660f);
                        CCParticleSystemQuad* particle12 = GameToolbox::particleFromString("11a-1a1.5a0.3a6a90a90a0a0a31a69a0a0a0a0a0a0a4a1a0a0a1a0a1a0a1a0a1a0a7a1a52a55a1a0a1a0a1a0a1a0a0.2a0a0.4a0a0a0a0a0a0a0a0a2a1a0a0a0a167a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle12);
                        particle12->setPosition({1, 16});
                        particle12->setZOrder(1);
                        particle12->setScaleX(1.835f);
                        particle12->setScaleY(1.485f);
                    } else if (gauntletName == "Fantasy") {
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("fantasy_gauntlet.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.225f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);

                        // Show gauntlets sprite again
                        gauntlet->setVisible(true);
                        gauntlet->setPosition({-2, 14});
                        gauntlet->setScale(0.925f);
                        gauntlet_node->setZOrder(2);

                        // Particle
                        CCParticleSystemQuad* particle = GameToolbox::particleFromString("26a-1a5a0.3a4a-90a14a9a0a17a0a0a0a0a0a0a0a2a1a76a325a0.537255a0a0.898039a0a0.164706a0a1a0a2a1a148a149a0.537255a0a0.898039a0a0.164706a0a1a0a0a0a0.3a0a0a0a0a0a0a0a0a2a0a0a0a0a146a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle);
                        particle->setPosition({gauntlet->getPositionX(), 103});
                        particle->setZOrder(1);
                        particle->setScaleX(2.880f);
                        particle->setScaleY(4.603f);
                    } else if (gauntletName == "Portal") {
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("portal_gauntlet.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.225f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);

                        // Show gauntlets sprite again
                        gauntlet->setVisible(true);
                        gauntlet->setPosition({-2, 14});
                        gauntlet->setScale(0.925f);
                        gauntlet_node->setZOrder(2);

                        // Particle
                        CCParticleSystemQuad* particle = GameToolbox::particleFromString("13a-1a1a0.3a10a90a90a29a0a11a0a0a0a0a0a0a0a50a1a0a37a0.376471a0a0.109804a0a0.243137a0a1a0a32a1a142a96a0.376471a0a0.109804a0a0.243137a0a1a0a0.5a0a0.3a0a0a0a0a0a0a0a1a2a1a0a0a0a190a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle);
                        particle->setPosition({gauntlet->getPositionX(), 14});
                        particle->setZOrder(1);
                        particle->setScaleX(2.880f);
                        particle->setScaleY(3.978f);
                        CCParticleSystemQuad* particle2 = GameToolbox::particleFromString("13a-1a1a0.3a10a90a90a29a0a11a0a0a0a0a0a0a0a50a1a0a37a0.0705882a0a0.172549a0a0.203922a0a1a0a32a1a142a96a0.0705882a0a0.172549a0a0.203922a0a1a0a0.5a0a0.3a0a0a0a0a0a0a0a1a2a1a0a0a0a190a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle2);
                        particle2->setPosition({gauntlet->getPositionX(), 14});
                        particle2->setZOrder(1);
                        particle2->setScaleX(2.880f);
                        particle2->setScaleY(3.978f);
                        CCParticleSystemQuad* particle3 = GameToolbox::particleFromString("38a-1a2a0.3a16a90a180a29a0a11a0a0a0a0a0a0a0a6a1a0a0a0.760784a0a0.380392a0a0.835294a0a1a0a3a1a0a0a0.760784a0a0.380392a0a0.835294a0a1a0a0.18a0a0.49a0a43a0a33a0a-34a0a1a2a1a0a0a0a28a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle3);
                        particle3->setPosition({gauntlet->getPositionX(), 14});
                        particle3->setZOrder(1);
                        particle3->setScaleX(1.030f);
                        particle3->setScaleY(1.030f);
                        CCParticleSystemQuad* particle4 = GameToolbox::particleFromString("38a-1a2a0.3a16a90a180a29a0a11a0a0a0a0a0a0a0a6a1a0a0a0.376471a0a0.690196a0a0.831373a0a1a0a3a1a0a0a0.376471a0a0.690196a0a0.831373a0a1a0a0.18a0a0.49a0a43a0a33a0a-34a0a1a2a1a0a0a0a28a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle4);
                        particle4->setPosition({gauntlet->getPositionX(), 14});
                        particle4->setZOrder(1);
                        particle4->setScaleX(1.155f);
                        particle4->setScaleY(1.355f);
                        CCParticleSystemQuad* particle5 = GameToolbox::particleFromString("30a-1a1a0.3a30a90a180a29a0a11a0a0a0a0a0a0a0a4a1a0a0a0.760784a0a0.380392a0a0.835294a0a1a0a2a1a0a0a0.760784a0a0.380392a0a0.835294a0a1a0a0.18a0a0.49a0a43a0a33a0a-41a0a1a2a1a0a0a0a28a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle5);
                        particle5->setPosition({gauntlet->getPositionX(), 14});
                        particle5->setZOrder(1);
                        particle5->setScaleX(1.455f);
                        particle5->setScaleY(1.655f);
                        CCParticleSystemQuad* particle6 = GameToolbox::particleFromString("30a-1a1a0.3a30a90a180a29a0a11a0a0a0a0a0a0a0a4a1a0a0a0.376471a0a0.690196a0a0.831373a0a1a0a2a1a0a0a0.376471a0a0.690196a0a0.831373a0a1a0a0.18a0a0.49a0a43a0a33a0a-41a0a1a2a1a0a0a0a28a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle6);
                        particle6->setPosition({gauntlet->getPositionX(), 14});
                        particle6->setZOrder(1);
                        particle6->setScaleX(1.355f);
                        particle6->setScaleY(1.855f);
                    } else if (gauntletName == "Demon") {
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("demon_gauntlet.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.275f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);

                        // Show gauntlets sprite again
                        gauntlet->setVisible(true);
                        gauntlet->setPosition({-1, 22});
                        gauntlet->setScale(1.1f);
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
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("castle_gauntlet.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.275f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);
                    } else if (gauntletName == "Galaxy") {
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("galaxy_gauntlet.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.695f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);

                        // Show gauntlets sprite again
                        gauntlet->setVisible(true);
                        gauntlet->setPosition({0, 11});
                        gauntlet->setScale(0.975);
                        gauntlet_node->setZOrder(3);

                        // Particle
                        CCParticleSystemQuad* particle = GameToolbox::particleFromString("11a-1a1a0.3a8a90a90a0a0a0a0a0a0a0a0a0a0a50a3a0a0a0.627451a0a0.286275a0a0.470588a0a1a0a50a3a106a70a0.627451a0a0.286275a0a0.470588a0a1a0a0a0a0a0a0a0a0a0a0a0a0a2a1a0a0a0a176a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle);
                        particle->setPosition({0, 24});
                        particle->setZOrder(1);
                        particle->setScaleX(1.610f);
                        particle->setScaleY(1.610f);
                        CCParticleSystemQuad* particle2 = GameToolbox::particleFromString("14a-1a1.2a0.3a9a112a6a42a0a15a0a0a0a0a0a0a0a4a0a340a0a0.913725a0a0.72549a0a0.337255a0a1a0a4a0a340a0a0.913725a0a0.72549a0a0.337255a0a1a0a0.3a0a0.3a0a0a0a0a0a0a0a0a2a1a0a0a0a31a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle2);
                        particle2->setPosition({-25, 111});
                        particle2->setZOrder(1);
                        particle2->setScaleX(2.660f);
                        particle2->setScaleY(3.710f);
                        particle2->setRotation(180);
                        CCParticleSystemQuad* particle3 = GameToolbox::particleFromString("38a-1a1.5a0.3a21a90a90a0a0a31a69a0a0a0a0a0a0a2a1a65a57a0.886275a0a0.866667a0a0.454902a0a1a0a5a1a0a0a0.886275a0a0.866667a0a0.454902a0a1a0a0.2a0a0.4a0a0a0a0a0a0a0a0a2a1a0a0a0a169a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle3);
                        particle3->setPosition({2, 6});
                        particle3->setZOrder(1);
                        particle3->setScaleX(1.685f);
                        particle3->setScaleY(1.485f);
                        CCParticleSystemQuad* particle4 = GameToolbox::particleFromString("11a-1a1.5a0.3a6a90a90a0a0a31a69a0a0a0a0a0a0a4a1a0a0a0.890196a0a0.376471a0a0.65098a0a1a0a7a1a52a55a0.890196a0a0.376471a0a0.65098a0a1a0a0.2a0a0.4a0a0a0a0a0a0a0a0a2a1a0a0a0a167a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle4);
                        particle4->setPosition({2, -2});
                        particle4->setZOrder(1);
                        particle4->setScaleX(1.310f);
                        particle4->setScaleY(1.635f);
                        CCParticleSystemQuad* particle5 = GameToolbox::particleFromString("4a-1a1a0.3a3a90a90a0a0a0a0a0a0a0a0a0a0a50a3a0a0a0.862745a0a0.352941a0a0.623529a0a1a0a50a3a106a22a0.862745a0a0.352941a0a0.623529a0a1a0a0.3a0a0.3a0a0a0a0a0a0a0a0a2a1a0a0a0a164a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle5);
                        particle5->setPosition({0, 24});
                        particle5->setZOrder(1);
                        particle5->setScaleX(1.785f);
                        particle5->setScaleY(1.785f);
                    } else if (gauntletName == "Universe") {
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("universe_gauntlet.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.270f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);

                        // Show gauntlets sprite again
                        gauntlet->setVisible(true);
                        gauntlet->setPosition({0, 16});
                        gauntlet->setScale(0.975f);
                        gauntlet_node->setZOrder(3);

                        // Particle
                        CCParticleSystemQuad* particle = GameToolbox::particleFromString("23a-1a3a0.3a6a90a180a29a0a11a0a0a0a0a0a0a0a38a1a0a0a0.0627451a0a0.0980392a0a0.105882a0a1a0a50a1a0a0a0.0627451a0a0.0980392a0a0.105882a0a1a0a0.1a0a0.5a0a10a0a28a0a0a0a1a2a1a0a1a0a46a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle);
                        particle->setPosition({0, 11});
                        particle->setZOrder(1);
                        particle->setScaleX(1.485f);
                        particle->setScaleY(2.685f);
                        CCParticleSystemQuad* particle2 = GameToolbox::particleFromString("20a-1a0.5a0a15a90a0a44a0a11a0a0a0a0a0a0a0a4a1a0a0a0.760784a0a0.447059a0a0.94902a0a1a0a4a1a0a0a0.760784a0a0.447059a0a0.94902a0a1a0a0.1a0a0.1a0a0a0a0a0a0a0a0a2a1a0a0a0a33a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle2);
                        particle2->setPosition({32, -74});
                        particle2->setZOrder(1);
                        particle2->setScaleX(3.685f);
                        particle2->setScaleY(8.310f);
                        particle2->setRotation(-20);
                        CCParticleSystemQuad* particle3 = GameToolbox::particleFromString("19a-1a0.5a0a38a90a0a44a0a20a0a0a0a0a0a0a0a2a1a0a0a0.94902a0a0.67451a0a0.447059a0a1a0a2a1a0a0a0.94902a0a0.67451a0a0.447059a0a1a0a0.1a0a0.1a0a0a0a0a0a0a0a0a2a1a0a0a0a33a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle3);
                        particle3->setPosition({34, -96});
                        particle3->setZOrder(1);
                        particle3->setScaleX(2.135f);
                        particle3->setScaleY(8.935f);
                        particle3->setRotation(-20);
                        CCParticleSystemQuad* particle4 = GameToolbox::particleFromString("20a-1a1a0a15a90a0a36a0a11a0a0a0a0a0a0a0a4a1a0a0a0.447059a0a0.654902a0a0.94902a0a1a0a4a1a0a0a0.447059a0a0.654902a0a0.94902a0a1a0a0.2a0a0.2a0a0a0a0a0a0a0a0a2a1a0a0a0a28a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle4);
                        particle4->setPosition({41, -92});
                        particle4->setZOrder(1);
                        particle4->setScaleX(3.235f);
                        particle4->setScaleY(6.585f);
                        particle4->setRotation(-20);
                        CCParticleSystemQuad* particle5 = GameToolbox::particleFromString("11a-1a1a0.3a8a90a90a0a0a0a0a0a0a0a0a0a0a50a3a0a0a0.239216a0a0.403922a0a0.419608a0a1a0a50a3a106a70a0.239216a0a0.403922a0a0.419608a0a1a0a0a0a0a0a0a0a0a0a0a0a0a2a1a0a0a0a176a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle5);
                        particle5->setPosition({0, 24});
                        particle5->setZOrder(1);
                        particle5->setScaleX(1.710f);
                        particle5->setScaleY(1.710f);
                        CCParticleSystemQuad* particle6 = GameToolbox::particleFromString("4a-1a1a0.3a3a90a90a0a0a0a0a0a0a0a0a0a0a50a3a0a0a0.32549a0a0.537255a0a0.560784a0a1a0a50a3a106a22a0.32549a0a0.537255a0a0.560784a0a1a0a0.3a0a0.3a0a0a0a0a0a0a0a0a2a1a0a0a0a190a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle6);
                        particle6->setPosition({-2, 24});
                        particle6->setZOrder(1);
                        particle6->setScaleX(2.710f);
                        particle6->setScaleY(2.710f);
                    } else if (gauntletName == "Cursed") {
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("cursed_gauntlet.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.270f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);

                        // Show gauntlets sprite again
                        gauntlet->setVisible(true);
                        gauntlet->setPosition({1, 16});
                        gauntlet->setScale(1);
                        gauntlet_node->setZOrder(3);

                        // Particle
                        CCParticleSystemQuad* particle = GameToolbox::particleFromString("5a-1a1a0.3a3a90a90a29a0a11a0a0a0a0a0a0a0a50a1a24a45a0.105882a0a0.380392a0a0.105882a0a1a0a44a1a48a28a0.105882a0a0.380392a0a0.105882a0a1a0a0a0a0a0a0a0a0a0a0a0a1a2a1a0a0a0a175a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle);
                        particle->setPosition(gauntlet->getPosition());
                        particle->setZOrder(1);
                        particle->setScaleX(2.035f);
                        particle->setScaleY(2.035f);
                        CCParticleSystemQuad* particle2 = GameToolbox::particleFromString("5a-1a1a0.3a3a90a90a29a0a11a0a0a0a0a0a0a0a50a1a24a45a0.105882a0a0.380392a0a0.105882a0a1a0a44a1a48a28a0.105882a0a0.380392a0a0.105882a0a1a0a0a0a0a0a0a0a0a0a0a0a1a2a1a0a0a0a175a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle2);
                        particle2->setPosition(gauntlet->getPosition());
                        particle2->setZOrder(1);
                        particle2->setScaleX(2.035f);
                        particle2->setScaleY(2.035f);
                        CCParticleSystemQuad* particle3 = GameToolbox::particleFromString("93a-1a1a0.3a71a90a0a52a0a10a0a23a0a0a0a0a0a2a1a0a0a0.00392157a0a0.905882a0a0a0a1a0a2a1a30a0a0.00392157a0a0.905882a0a0a0a1a0a0.3a0a0.5a0a0a0a0a0a0a0a0a2a1a0a0a0a32a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle3);
                        particle3->setPosition({17, 94});
                        particle3->setZOrder(1);
                        particle3->setScaleX(4.510f);
                        particle3->setScaleY(3.335f);
                        particle3->setRotation(180);
                    } else if (gauntletName == "Doom") {
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("doom_gauntlet.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.270f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);

                        // Show gauntlets sprite again
                        gauntlet->setVisible(true);
                        gauntlet->setPosition({0, 19});
                        gauntlet->setScale(1.025f);
                        gauntlet_node->setZOrder(3);

                        // Particle
                        CCParticleSystemQuad* particle = GameToolbox::particleFromString("66a-1a3a0.3a20a90a27a11a0a43a83a0a0a0a0a0a0a5a1a0a0a0.733333a0a0.607843a0a0.12549a0a1a0a3a1a0a0a0.733333a0a0.470588a0a0.12549a0a1a0a0.5a0a1a0a0a0a0a0a0a0a0a2a1a0a0a0a28a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle);
                        particle->setPosition({0, -7});
                        particle->setZOrder(1);
                        particle->setScaleX(1.235f);
                        particle->setScaleY(1.235f);
                        CCParticleSystemQuad* particle2 = GameToolbox::particleFromString("44a-1a3a0.3a13a90a27a11a0a32a62a0a0a0a0a0a0a50a1a0a0a0.0666667a0a0.0509804a0a0.0470588a0a1a0a50a1a0a0a0.0666667a0a0.0509804a0a0.0470588a0a1a0a0.5a0a1a0a0a0a0a0a0a0a0a2a1a0a0a0a46a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle2);
                        particle2->setPosition({0, -13});
                        particle2->setZOrder(1);
                        particle2->setScaleX(1.310f);
                        particle2->setScaleY(1.435f);
                        CCParticleSystemQuad* particle3 = GameToolbox::particleFromString("6a-1a1.9a0a3a90a17a69a0a3a0a0a-102a0a0a0a0a4a1a57a105a1a0a1a0a1a0a1a0a4a1a0a0a1a0a1a0a1a0a1a0a0a0a0a0a0a0a0a0a0a0a0a2a1a0a0a0a145a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle3);
                        particle3->setPosition(gauntlet->getPosition());
                        particle3->setZOrder(1);
                        particle3->setScaleX(2.035f);
                        particle3->setScaleY(2.035f);
                    } else if (gauntletName == "Split") {
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("split_gauntlet.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.270f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);

                        // Show gauntlets sprite again
                        gauntlet->setVisible(true);
                        gauntlet->setPosition({-1, 20});
                        gauntlet->setScale(0.975f);
                        gauntlet_node->setZOrder(2);

                        // Particle
                        CCParticleSystemQuad* particle = GameToolbox::particleFromString("48a-1a2a0.3a20a90a2a19a0a10a20a0a0a0a0a0a0a4a1a0a0a0.666667a0a0.360784a0a0.603922a0a1a0a2a1a0a0a0.666667a0a0.360784a0a0.603922a0a1a0a0.5a0a0.5a0a0a0a0a0a0a0a0a2a1a0a0a0a31a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle);
                        particle->setPosition({-28, -65});
                        particle->setZOrder(1);
                        particle->setScaleX(2.180f);
                        particle->setScaleY(4.003f);
                        CCParticleSystemQuad* particle2 = GameToolbox::particleFromString("48a-1a2a0.3a20a90a2a19a0a10a20a0a0a0a0a0a0a4a1a0a0a0.454902a0a0.215686a0a0.403922a0a1a0a2a1a0a0a0.454902a0a0.215686a0a0.403922a0a1a0a0.5a0a0.5a0a0a0a0a0a0a0a0a2a1a0a0a0a28a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle2);
                        particle2->setPosition({-28, -65});
                        particle2->setZOrder(1);
                        particle2->setScaleX(2.180f);
                        particle2->setScaleY(4.003f);
                        CCParticleSystemQuad* particle3 = GameToolbox::particleFromString("48a-1a2a0.3a20a90a2a19a0a10a20a0a0a0a0a0a0a4a1a0a0a0.243137a0a0.329412a0a0.341176a0a1a0a2a1a0a0a0.243137a0a0.329412a0a0.341176a0a1a0a0.5a0a0.5a0a0a0a0a0a0a0a0a2a1a0a0a0a31a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle3);
                        particle3->setPosition({25, 65});
                        particle3->setZOrder(1);
                        particle3->setScaleX(2.180f);
                        particle3->setScaleY(4.003f);
                        particle3->setRotation(180);
                        CCParticleSystemQuad* particle4 = GameToolbox::particleFromString("48a-1a2a0.3a20a90a2a19a0a10a20a0a0a0a0a0a0a4a1a0a0a0.145098a0a0.203922a0a0.207843a0a1a0a2a1a0a0a0.145098a0a0.203922a0a0.207843a0a1a0a0.5a0a0.5a0a0a0a0a0a0a0a0a2a1a0a0a0a28a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle4);
                        particle4->setPosition({25, 65});
                        particle4->setZOrder(1);
                        particle4->setScaleX(2.180f);
                        particle4->setScaleY(4.003f);
                        particle4->setRotation(180);
                        CCParticleSystemQuad* particle5 = GameToolbox::particleFromString("30a-1a2a0.3a13a90a90a29a0a11a0a0a0a0a0a0a0a6a1a0a0a0.705882a0a0.34902a0a0.698039a0a1a0a2a1a0a0a0.584314a0a0.34902a0a0.705882a0a1a0a0a0a0.5a0a6a0a34a0a0a0a1a2a1a0a0a0a159a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle5);
                        particle5->setPosition({0, 29});
                        particle5->setZOrder(1);
                        particle5->setScaleX(1.955f);
                        particle5->setScaleY(1.955f);
                        particle5->setRotation(90);
                        CCParticleSystemQuad* particle6 = GameToolbox::particleFromString("30a-1a2a0.3a13a90a90a29a0a11a0a0a0a0a0a0a0a6a1a0a0a0.254902a0a0.333333a0a0.388235a0a1a0a2a1a0a0a0.254902a0a0.333333a0a0.388235a0a1a0a0a0a0.5a0a6a0a34a0a0a0a1a2a1a0a0a0a159a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle6);
                        particle6->setPosition({0, 29});
                        particle6->setZOrder(1);
                        particle6->setScaleX(1.955f);
                        particle6->setScaleY(1.955f);
                        particle6->setRotation(-90);
                        CCParticleSystemQuad* particle7 = GameToolbox::particleFromString("30a-1a2a0.3a13a90a90a29a0a11a0a0a0a0a0a0a0a6a1a0a0a0.705882a0a0.34902a0a0.698039a0a1a0a2a1a0a0a0.584314a0a0.34902a0a0.705882a0a1a0a0a0a0.5a0a6a0a34a0a0a0a1a2a1a0a0a0a28a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle7);
                        particle7->setPosition({0, 29});
                        particle7->setZOrder(1);
                        particle7->setScaleX(1.955f);
                        particle7->setScaleY(1.955f);
                        particle7->setRotation(90);
                        CCParticleSystemQuad* particle8 = GameToolbox::particleFromString("30a-1a2a0.3a13a90a90a29a0a11a0a0a0a0a0a0a0a6a1a0a0a0.254902a0a0.333333a0a0.388235a0a1a0a2a1a0a0a0.254902a0a0.333333a0a0.388235a0a1a0a0a0a0.5a0a6a0a34a0a0a0a1a2a1a0a0a0a28a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle8);
                        particle8->setPosition({0, 29});
                        particle8->setZOrder(1);
                        particle8->setScaleX(1.955f);
                        particle8->setScaleY(1.955f);
                        particle8->setRotation(-90);
                    } else if (gauntletName == "Monster") {
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("monster_gauntlet.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.270f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);

                        // Show gauntlets sprite again
                        gauntlet->setVisible(true);
                        gauntlet->setPosition({0, 22});
                        gauntlet->setScale(1.075f);
                        gauntlet_node->setZOrder(2);

                        // Particle
                        CCParticleSystemQuad* particle = GameToolbox::particleFromString("30a-1a3a0.3a9a90a9a11a0a17a40a0a0a0a0a0a0a4a1a0a0a0.0509804a0a0.929412a0a0a0a1a0a2a1a0a0a0.360784a0a0.486275a0a0.352941a0a1a0a0.1a0a0.5a0a0a0a0a0a0a0a0a2a1a0a0a0a165a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle);
                        particle->setPosition({gauntlet->getPositionX(), -37});
                        particle->setZOrder(1);
                        particle->setScaleX(2.510f);
                        particle->setScaleY(1.985f);
                        CCParticleSystemQuad* particle2 = GameToolbox::particleFromString("66a-1a3a0.3a20a90a27a11a0a37a38a0a0a0a0a0a0a5a1a0a0a0.0156863a0a0.866667a0a0.027451a0a1a0a3a1a0a0a0.164706a0a0.509804a0a0a0a1a0a0.5a0a1a0a0a0a0a0a0a0a0a2a1a0a0a0a28a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle2);
                        particle2->setPosition({gauntlet->getPositionX(), -38});
                        particle2->setZOrder(1);
                        particle2->setScaleX(1.072f);
                        particle2->setScaleY(2.475f);
                        CCParticleSystemQuad* particle3 = GameToolbox::particleFromString("18a-1a3a0.3a5a90a9a11a0a32a28a0a0a0a0a0a0a50a1a0a0a0.0352941a0a0.0901961a0a0.0313726a0a1a0a50a1a0a0a0.0352941a0a0.0901961a0a0.0313726a0a1a0a1a0a1a0a0a0a0a0a0a0a0a2a1a0a0a0a46a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle3);
                        particle3->setPosition({gauntlet->getPositionX(), -48});
                        particle3->setZOrder(1);
                        particle3->setScaleX(1.470f);
                        particle3->setScaleY(1.957f);
                    } else if (gauntletName == "Mystery") {
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("mystery_gauntlet.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.270f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);

                        // Show gauntlets sprite again
                        gauntlet->setVisible(true);
                        gauntlet->setPosition({3, 13});
                        gauntlet->setScale(0.975f);
                        gauntlet_node->setZOrder(3);

                        // Particle
                        CCParticleSystemQuad* particle = GameToolbox::particleFromString("30a-1a3a0.3a9a90a180a29a0a11a0a0a0a0a0a0a0a2a1a0a0a1a0a0.8a0a0.258824a0a1a0a8a1a0a0a1a0a0.8a0a0.258824a0a1a0a0.5a0a0.5a0a6a0a30a0a0a0a1a2a0a0a0a0a11a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle);
                        particle->setPosition(gauntlet->getPosition());
                        particle->setZOrder(2);
                        particle->setScaleX(2.035f);
                        particle->setScaleY(2.035f);
                        CCParticleSystemQuad* particle2 = GameToolbox::particleFromString("31a-1a3a0.3a9a90a180a0a0a29a82a0a0a0a0a0a0a4a1a0a0a0.388235a0a0.741176a0a1a0a1a0a15a1a0a0a0.388235a0a0.741176a0a1a0a1a0a1a0a1a0a6a0a30a0a0a0a0a2a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle2);
                        particle2->setPosition({0, 0});
                        particle2->setZOrder(1);
                        particle2->setScaleX(1.435f);
                        particle2->setScaleY(1.435f);
                    } else if (gauntletName == "Strange") {
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("strange_gauntlet.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.270f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);

                        reverseGauntlet(node);

                        // Show gauntlets sprite again
                        gauntlet->setVisible(true);
                        gauntlet->setPosition({2, -18});
                        gauntlet->setRotation(180);
                        gauntlet->setScale(0.975f);
                        gauntlet->setFlipX(true);
                        gauntlet_node->setZOrder(2);

                        // Particle
                        CCParticleSystemQuad* particle = GameToolbox::particleFromString("36a-1a3a0.3a6a-90a24a9a0a11a0a0a-7a0a0a0a0a2a1a76a325a0.592157a0a0.294118a0a0.658824a0a1a0a2a1a201a149a0.592157a0a0.294118a0a0.658824a0a1a0a0.3a0a0.3a0a0a0a0a0a0a0a0a2a0a0a0a0a96a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle);
                        particle->setPosition({2, -65});
                        particle->setZOrder(2);
                        particle->setScaleX(2.480f);
                        particle->setScaleY(2.728f);
                        particle->setRotation(180);
                        CCParticleSystemQuad* particle2 = GameToolbox::particleFromString("17a-1a4a0.3a3a180a0a8a0a47a34a0a0a0a0a0a0a50a1a0a0a0.243137a0a0.239216a0a0.239216a0a1a0a50a1a0a0a0.454902a0a0.447059a0a0.447059a0a1a0a1a0a1a0a0a0a0a0a0a0a0a2a1a0a0a0a46a0a0a0a0a0a0a0a0a0a0a0a0a0a0", NULL, false);
                        node->addChild(particle2);
                        particle2->setPosition({17, 60});
                        particle2->setZOrder(3);
                        particle2->setScaleX(0.805f);
                        particle2->setScaleY(1.155f);
                    } else if (gauntletName == "Water") {
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("water_gauntlet.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.270f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);
                    } else if (gauntletName == "Fire") {
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("fire_gauntlet.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.270f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);
                    } else if (gauntletName == "Ice") {
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("ice_gauntlet.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.270f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);
                    } else if (gauntletName == "Poison") {
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("poison_gauntlet.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.270f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);
                    } else if (gauntletName == "Time") {
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("time_gauntlet.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.220f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);
                    } else if (gauntletName == "Spike") {
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("spike_gauntlet.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.920f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);
                    } else if (gauntletName == "Shadow") {
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("shadow_gauntlet.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.745f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);
                    } else if (gauntletName == "NCS I") {
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("ncs_gauntlet_1.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.270f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);
                    } else if (gauntletName == "NCS II") {
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("ncs_gauntlet_2.png"_spr);
                        sprite->setPosition(bg->getPosition());
                        sprite->setScale(1.270f);
                        sprite->setZOrder(0);
                        node->addChild(sprite);
                    } else if (gauntletName == "Discord") {
                        CCSprite* sprite = CCSprite::createWithSpriteFrameName("discord_gauntlet.png"_spr);
                        sprite->setPosition({bg->getPositionX(), bg->getPositionY() + 3});
                        sprite->setScale(1.245f);
                        sprite->setZOrder(0);
                        item->setZOrder(1);
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