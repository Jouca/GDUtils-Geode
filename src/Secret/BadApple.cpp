// SPOILERSSS
#include "BadApple.h"
#ifndef GEODE_IS_MACOS
#include "../Utils/DownloadManager.h"
#else // mac
#include <ctime>
#include <cxxabi.h>
#endif
#include <filesystem>
BadApple* BadApple::create() {
    auto pRet = new BadApple();
    if (pRet && pRet->BadApple::init()) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
};

bool BadApple::init() {
    auto menu = CCMenu::create();
    //#ifndef GEODE_IS_MACOS
    #ifdef GEODE_IS_WINDOWS
    videoplayer::VideoPlayer* player = videoplayer::VideoPlayer::create("./gdutils/bad_apple.mpg", true);
    CCDirector* director = CCDirector::sharedDirector();
    CCSize size = director->getWinSize();
    player->setPosition(size.width / 2, size.height / 2);
    player->setContentSize({ size.width - 40, size.height - 40 });
    player->setZOrder(-1);
    //reinterpret_cast<CCNode*>(layer->getChildren()->objectAtIndex(0))->setVisible(false);
    this->addChild(player);
    #endif
    setKeypadEnabled(true);

    return true;
}

void BadApple::keyBackClicked() {
    setKeypadEnabled(false);

    #ifdef GEODE_IS_UNKNOWN
    GameSoundManager::sharedManager()->playBackgroundMusic("./Resources/secretLoop02.mp3", true, false);
    #endif
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

// touhou time
//#ifdef GEODE_IS_WINDOWS
#include "BadApple.h"
std::string text_input = "";
bool bad_apple = false;

#include <Geode/modify/SecretLayer2.hpp>

class $modify(SecretVault, SecretLayer2) {
    bool init() {
        if (!SecretLayer2::init()) return false;
        bad_apple = false;
        return true;
    }
    void onSubmit(CCObject* obj) {
        SecretLayer2::onSubmit(obj);
        CCLabelBMFont* vault_text = typeinfo_cast<cocos2d::CCLabelBMFont*>(this->getChildren()->objectAtIndex(3));
        CCMenu* menu = nullptr;
        if (this->getChildrenCount() > 8) {
            menu = typeinfo_cast<cocos2d::CCMenu*>(this->getChildren()->objectAtIndex(8)); //6 before
        } else {
            menu = typeinfo_cast<cocos2d::CCMenu*>(this->getChildren()->objectAtIndex(6));
        }
        if (!strcmp(text_input.c_str(), "bad apple")) {
            if (!bad_apple) {
                ghc::filesystem::create_directory("gdutils");
                bad_apple = true;

                vault_text->setString("It's time... Touhou Fan");
                vault_text->setColor({ 255, 0, 0 });
                auto badappleSpr = CCSprite::createWithSpriteFrameName("GJ_musicOnBtn_001.png");
                auto badappleBtn = CCMenuItemSpriteExtra::create(
                    badappleSpr,
                    this,
                    menu_selector(SecretVault::onLaunchBadApple)
                );
                badappleBtn->setPosition(17, -89);
                menu->addChild(badappleBtn);

                auto badappleLabel = CCLabelBMFont::create("Bad Apple", "bigFont.fnt");
                badappleLabel->setScale(.4f);
                badappleLabel->setPosition(44, 237);
                this->addChild(badappleLabel);
            }
        }
    }
    void launchBadApple(CCObject* pSender) {
        //GameSoundManager::sharedManager()->stopBackgroundMusic();
        auto scene = cocos2d::CCScene::create();
        auto layer = BadApple::create();
        scene->addChild(layer);
        CCDirector::sharedDirector()->pushScene(scene); // cocos2d::CCTransitionFade::create(0.5f, scene)
    }

    void onLaunchBadApple(CCObject* pSender) {
        if (ghc::filesystem::exists("gdutils/bad_apple.mpg")) {
            auto node = CCNode::create();
            node->setTag(10);
            SecretVault::launchBadApple(node);
        } else {
            log::info("wait for bad apple");
            DownloadManager::create("https://clarifygdps.com/videos/bad_apple.mpg", "gdutils/bad_apple.mpg", menu_selector(SecretVault::launchBadApple))->show();
        }
    }

    void updateSearchLabel(const char* text) {
        if (strlen(text) > 0) {
            text_input = text;
        }
        SecretLayer2::updateSearchLabel(text);
    }
};
// touhou time
