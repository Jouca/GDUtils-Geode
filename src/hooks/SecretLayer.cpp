#include <defs/geode.hpp>
#include <Geode/modify/SecretLayer.hpp>

class $modify(SecretLayer) {
    bool init() {
        if (!SecretLayer::init()) return false;
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto bg = CCSprite::createWithSpriteFrameName("thevault.png"_spr);
        bg->setPosition(winSize / 2);
        bg->setZOrder(-1);
        bg->setScaleY(winSize.height / bg->getContentSize().height);
        bg->setScaleX(winSize.width / bg->getContentSize().width);
        this->addChild(bg);
        if (auto vaultName = this->getChildByID("vault-name")) {
            vaultName->setVisible(false);
        }
        return true;
    }
};

static void toggleHook(bool value) {
    for (auto& hook : geode::Mod::get()->getHooks()) {
        if (hook->getDisplayName() == "SecretLayer::init") {
            if (value) {
                (void)hook->enable();
            } else {
                (void)hook->disable();
            }
        }
    }
}

$execute {
    listenForSettingChanges<bool>("secretLayerDesign", [](bool value) {
        log::info("SecretLayer Design changed to {}", value);
        toggleHook(value);
    });
}

$on_mod(Loaded) {
    if (!Mod::get()->getSettingValue<bool>("secretLayerDesign")) {
        toggleHook(false);
    }
}
