#include <defs/geode.hpp>
#include <Geode/modify/CCSprite.hpp>
#include <Geode/modify/CCScale9Sprite.hpp>
// Backgrounds
// i dont like ios
#ifndef GEODE_IS_IOS
class $modify(UtilCCSprite, cocos2d::CCSprite) {
    static CCSprite* create(char const* name) {
        auto ret = CCSprite::create(name);
        if (ret == nullptr) return ret;
        // better code, thanks camila!
        if (!strcmp(name, "GJ_gradientBG.png")) {
            ret->retain();
            Loader::get()->queueInMainThread([=] {
                if (ret != nullptr) {
                    if (ret->getColor() == ccc3(0, 102, 255)) {
                        auto scene = CCDirector::sharedDirector()->getRunningScene();
                        if (scene->getChildrenCount() == 0) return;
                        auto layer = scene->getChildren()->objectAtIndex(0);
                        ret->setColor(Mod::get()->getSettingValue<ccColor3B>("background"));
                    }
                    ret->release();
                }
            });
        }
        return ret;
    }
};

// Child background
class $modify(UtilCCScale9, cocos2d::extension::CCScale9Sprite) {
    static cocos2d::extension::CCScale9Sprite* create(char const* name, CCRect rect) {
        auto ret = cocos2d::extension::CCScale9Sprite::create(name, rect);
        if (ret == nullptr) return ret;
        // better code, thanks camila!
        if ((!strcmp(name, "square02b_001.png") || !strcmp(name, "square02b_small.png"))) {
            ret->retain();
            Loader::get()->queueInMainThread([=] {
                if (ret != nullptr) {
                    // jouca waht
                    if (ret->getColor() == ccc3(0, 56, 141)) {
                        ccColor3B new_color = Mod::get()->getSettingValue<ccColor3B>("child-background");
                        if (Mod::get()->getSettingValue<bool>("syncBGcolor")) {
                            new_color = Mod::get()->getSettingValue<ccColor3B>("background");
                        }
                        if (Mod::get()->getSettingValue<bool>("addChildBDGradient")) {
                            new_color = ccc3(new_color.r * 0.79, new_color.g * 0.79, new_color.b * 0.79);
                        }
                        if (Mod::get()->getSettingValue<bool>("bgchild-transparency")) {
                            int opacity = Mod::get()->getSettingValue<int64_t>("bgchild-transparency-scroll");
                            ret->setOpacity(opacity);
                        }
                        ret->setColor(new_color);
                    } else if (ret->getColor() == ccc3(0, 39, 98)) {
                        ccColor3B new_color = Mod::get()->getSettingValue<ccColor3B>("child-background");
                        if (Mod::get()->getSettingValue<bool>("syncBGcolor")) {
                            new_color = Mod::get()->getSettingValue<ccColor3B>("background");
                        }
                        if (Mod::get()->getSettingValue<bool>("addChildBDGradient")) {
                            new_color = ccc3(new_color.r * 0.71, new_color.g * 0.71, new_color.b * 0.71);
                        }
                        if (Mod::get()->getSettingValue<bool>("bgchild-transparency")) {
                            int opacity = Mod::get()->getSettingValue<int64_t>("bgchild-transparency-scroll");
                            ret->setOpacity(opacity);
                        }
                        ret->setColor(new_color);
                    } else if (ret->getColor() == ccc3(0, 46, 117)) {
                        ccColor3B new_color = Mod::get()->getSettingValue<ccColor3B>("child-background");
                        if (Mod::get()->getSettingValue<bool>("syncBGcolor")) {
                            new_color = Mod::get()->getSettingValue<ccColor3B>("background");
                        }
                        if (Mod::get()->getSettingValue<bool>("addChildBDGradient")) {
                            new_color = ccc3(new_color.r * 0.74, new_color.g * 0.74, new_color.b * 0.74);
                        }
                        if (Mod::get()->getSettingValue<bool>("bgchild-transparency")) {
                            int opacity = Mod::get()->getSettingValue<int64_t>("bgchild-transparency-scroll");
                            ret->setOpacity(opacity);
                        }
                        ret->setColor(new_color);
                    } else if (ret->getColor() == ccc3(0, 36, 91)) {
                        ccColor3B new_color = Mod::get()->getSettingValue<ccColor3B>("child-background");
                        if (Mod::get()->getSettingValue<bool>("syncBGcolor")) {
                            new_color = Mod::get()->getSettingValue<ccColor3B>("background");
                        }
                        if (Mod::get()->getSettingValue<bool>("addChildBDGradient")) {
                            new_color = ccc3(new_color.r * 0.70, new_color.g * 0.70, new_color.b * 0.70);
                        }
                        if (Mod::get()->getSettingValue<bool>("bgchild-transparency")) {
                            int opacity = Mod::get()->getSettingValue<int64_t>("bgchild-transparency-scroll");
                            ret->setOpacity(opacity);
                        }
                        ret->setColor(new_color);
                    } else if (ret->getColor() == ccc3(0, 31, 79)) {
                        ccColor3B new_color = Mod::get()->getSettingValue<ccColor3B>("child-background");
                        if (Mod::get()->getSettingValue<bool>("syncBGcolor")) {
                            new_color = Mod::get()->getSettingValue<ccColor3B>("background");
                        }
                        if (Mod::get()->getSettingValue<bool>("addChildBDGradient")) {
                            new_color = ccc3(new_color.r * 0.67, new_color.g * 0.67, new_color.b * 0.67);
                        }
                        if (Mod::get()->getSettingValue<bool>("bgchild-transparency")) {
                            int opacity = Mod::get()->getSettingValue<int64_t>("bgchild-transparency-scroll");
                            ret->setOpacity(opacity);
                        }
                        ret->setColor(new_color);
                    }
                    ret->release();
                }
            });
        }
        return ret;
    }
};

static void toggleHook(bool value) {
    for (auto& hook : geode::Mod::get()->getHooks()) {
        if (hook->getDisplayName().starts_with("cocos2d::")) {
            if (value) {
                (void)hook->enable();
            } else {
                (void)hook->disable();
            }
        }
    }
}

$execute {
    listenForSettingChanges<bool>("activate-background", [](bool value) {
        log::info("Background changed to {}", value);
        toggleHook(value);
    });
}

$on_mod(Loaded) {
    if (!Mod::get()->getSettingValue<bool>("activate-background")) {
        toggleHook(false);
    }
}

#endif
