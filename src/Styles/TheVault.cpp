#include "../includes.h"
#include <Geode/modify/SecretLayer.hpp>

class $modify(SecretLayer) {
    bool init() {
        if (!SecretLayer::init()) return false;

        if (!Mod::get()->template getSettingValue<bool>("secretLayerDesign")) return true;

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto bg = CCSprite::createWithSpriteFrameName("thevault.png"_spr);

        bg->setPosition(winSize / 2);
        bg->setZOrder(-1);
        bg->setScaleY(winSize.height / bg->getContentSize().height);
        bg->setScaleX(winSize.width / bg->getContentSize().width);
        this->addChild(bg);

        // Get vault name
        auto vaultName = this->getChildByID("vault-name");
        if (vaultName) {
            vaultName->setVisible(false);
        }

        return true;
    }
};