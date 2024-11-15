// SPOILERSSS
#include "BadApple.h"
#ifdef GEODE_IS_WINDOWS
#include "../Utils/DownloadManager.h"
#else // mac
#include <ctime>
#include <cxxabi.h>
#endif
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
std::string text_input = "";
#include "BadApple.h"
bool bad_apple = false;
#include "COTE.h"
bool cote = false;

#include <Geode/modify/SecretLayer2.hpp>

class $modify(SecretVault, SecretLayer2) {
    struct Fields {
        CCMenu* m_menu = nullptr;
    };

    bool init() {
        if (!SecretLayer2::init()) return false;
        bad_apple = false;
        cote = false;
        return true;
    }

    void onSubmit(CCObject* obj) {
        SecretLayer2::onSubmit(obj);

        if (m_fields->m_menu == nullptr) {
            m_fields->m_menu = CCMenu::create();
            m_fields->m_menu->setPosition(50, 200);
            m_fields->m_menu->setContentSize({ 100, 200 });
            m_fields->m_menu->setLayout(
                ColumnLayout::create()
                    ->setCrossAxisAlignment(AxisAlignment::Center)
            );
        }

        this->addChild(m_fields->m_menu);
        m_fields->m_menu->removeAllChildren();

        CCLabelBMFont* vault_text = nullptr;
        if (this->getChildByIDRecursive("vault-text") == nullptr) {
            vault_text = typeinfo_cast<cocos2d::CCLabelBMFont*>(this->getChildren()->objectAtIndex(3));
        } else {
            vault_text = as<CCLabelBMFont*>(this->getChildByIDRecursive("vault-text"));
            vault_text->setID("vault-text");
        }

        if (!strcmp(text_input.c_str(), "bad apple")) {
            if (!bad_apple) {
                std::filesystem::create_directory("gdutils");
                bad_apple = true;
                cote = false;

                auto menu = CCMenu::create();
                menu->setLayout(
                    RowLayout::create()
                        ->setAxis(Axis::Column)
                        ->setAxisAlignment(AxisAlignment::Center)
                        ->setGap(3)
                );

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
                badappleLabel->setLayoutOptions(
                    AxisLayoutOptions::create()
                        ->setAutoScale(true)
                        ->setScaleLimits(0, 0.4F)
                        ->setScalePriority(1)
                );
                menu->addChild(badappleLabel);

                menu->updateLayout();
                m_fields->m_menu->addChild(menu);
            }
        } else if (!strcmp(text_input.c_str(), "honami best girl")) {
            if (!cote) {
                std::filesystem::create_directory("gdutils");
                cote = true;
                bad_apple = false;

                auto menu = CCMenu::create();
                menu->setLayout(
                    RowLayout::create()
                        ->setAxis(Axis::Column)
                        ->setAxisAlignment(AxisAlignment::Center)
                        ->setGap(3)
                );

                vault_text->setString("Well, you have good taste.");
                vault_text->setColor({ 245, 143, 221 });
                auto COTESpr = CCSprite::createWithSpriteFrameName("GJ_musicOnBtn_001.png");
                auto COTEBtn = CCMenuItemSpriteExtra::create(
                    COTESpr,
                    this,
                    menu_selector(SecretVault::onLaunchCOTE)
                );
                COTEBtn->setPosition(17, -89);
                menu->addChild(COTEBtn);

                auto COTELabel = CCLabelBMFont::create("Classroom of the Elite", "bigFont.fnt");
                COTELabel->setScale(.2f);
                COTELabel->setPosition(44, 237);
                COTELabel->setLayoutOptions(
                    AxisLayoutOptions::create()
                        ->setAutoScale(true)
                        ->setScaleLimits(0, 0.2F)
                        ->setScalePriority(1)
                );
                menu->addChild(COTELabel);

                menu->updateLayout();
                m_fields->m_menu->addChild(menu);
            }
        } else {
            cote = false;
            bad_apple = false;
        }

        m_fields->m_menu->updateLayout();
    }

    void launchBadApple(CCObject* pSender) {
        
        //GameSoundManager::sharedManager()->stopBackgroundMusic();
        FMODAudioEngine::sharedEngine()->stopAllMusic();
        auto scene = cocos2d::CCScene::create();
        auto layer = BadApple::create();
        scene->addChild(layer);
        CCDirector::sharedDirector()->pushScene(scene); // cocos2d::CCTransitionFade::create(0.5f, scene)
    }

    void onLaunchBadApple(CCObject* pSender) {
        if (std::filesystem::exists("gdutils/bad_apple.mpg")) {
            auto node = CCNode::create();
            node->setTag(10);
            SecretVault::launchBadApple(node);
        } else {
            log::info("wait for bad apple");
        #ifdef GEODE_IS_WINDOWS
            DownloadManager::create("https://clarifygdps.com/videos/bad_apple.mpg", "gdutils/bad_apple.mpg", menu_selector(SecretVault::launchBadApple))->show();
        #endif
        }
    }

    void launchCOTE(CCObject* pSender) {
        //GameSoundManager::sharedManager()->stopBackgroundMusic();
        FMODAudioEngine::sharedEngine()->stopAllMusic();
        auto scene = cocos2d::CCScene::create();
        auto layer = COTE::create();
        scene->addChild(layer);
        CCDirector::sharedDirector()->pushScene(scene); // cocos2d::CCTransitionFade::create(0.5f, scene)
    }

    void onLaunchCOTE(CCObject* pSender) {
        if (std::filesystem::exists("gdutils/cote.mpg")) {
            auto node = CCNode::create();
            node->setTag(10);
            SecretVault::launchCOTE(node);
        } else {
            log::info("wait for cote");
        #ifdef GEODE_IS_WINDOWS
            DownloadManager::create("https://clarifygdps.com/videos/cote.mpg", "gdutils/cote.mpg", menu_selector(SecretVault::launchCOTE))->show();
        #endif
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
