// touhou time
#include <Geode/modify/SecretLayer2.hpp>
#include <defs/geode.hpp>
#include <modules/secrets/BadApple.hpp>
#include <modules/secrets/COTE.hpp>
#include <ui/DownloadPopup.hpp>

enum States {
    None,
    BadApple,
    COTE
};

class $modify(SecretVault, SecretLayer2) {
    struct Fields {
        CCMenu* m_menu = nullptr;
        States m_currentState = States::None;
    };

    bool init() {
        if (!SecretLayer2::init()) return false;
        return true;
    }

    void onSubmit(CCObject* obj) {
        gd::string input = m_searchInput->getString();
        SecretLayer2::onSubmit(obj);
        if (m_fields->m_menu == nullptr) {
            Build<CCMenu>::create().pos(50, 200).contentSize({100, 200}).layout(
                ColumnLayout::create()->setCrossAxisAlignment(AxisAlignment::Center)
            ).store(m_fields->m_menu).parent(this);
        }
        CCLabelBMFont* vault_text = typeinfo_cast<CCLabelBMFont*>(this->getChildByIDRecursive("vault-text"));
        if (!vault_text) {
            vault_text = this->getChildByType<CCLabelBMFont*>(1);
        }
        if (!strcmp(input.c_str(), "bad apple")) {
            if (m_fields->m_currentState != States::BadApple) {
                m_fields->m_currentState = States::BadApple;
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
        } else if (!strcmp(input.c_str(), "honami best girl")) {
            if (m_fields->m_currentState != States::COTE) {
                m_fields->m_currentState = States::COTE;
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
            m_fields->m_currentState = States::None;
        }
        m_fields->m_menu->updateLayout();
    }

    void launchBadApple() {
        //GameSoundManager::sharedManager()->stopBackgroundMusic();
        FMODAudioEngine::sharedEngine()->stopAllMusic(true);
        auto scene = cocos2d::CCScene::create();
        auto layer = BadApple::create();
        scene->addChild(layer);
        CCDirector::sharedDirector()->pushScene(scene); // cocos2d::CCTransitionFade::create(0.5f, scene)
    }

    void onLaunchBadApple(CCObject* pSender) {
        if (std::filesystem::exists(Mod::get()->getSaveDir() / "bad_apple.mpg")) {
            SecretVault::launchBadApple();
        } else {
            log::info("wait for bad apple");
        #ifdef GEODE_IS_WINDOWS
            DownloadPopup::create("https://clarifygdps.com/videos/bad_apple.mpg", Mod::get()->getSaveDir() / "bad_apple.mpg", [this]() {
                SecretVault::launchBadApple();
            })->show();
        #endif
        }
    }

    void launchCOTE() {
        //GameSoundManager::sharedManager()->stopBackgroundMusic();
        FMODAudioEngine::sharedEngine()->stopAllMusic(true);
        auto scene = cocos2d::CCScene::create();
        auto layer = COTE::create();
        scene->addChild(layer);
        CCDirector::sharedDirector()->pushScene(scene); // cocos2d::CCTransitionFade::create(0.5f, scene)
    }

    void onLaunchCOTE(CCObject* pSender) {
        if (std::filesystem::exists(Mod::get()->getSaveDir() / "cote.mpg")) {
            SecretVault::launchCOTE();
        } else {
            log::info("wait for cote");
        #ifdef GEODE_IS_WINDOWS
            DownloadPopup::create("https://clarifygdps.com/videos/cote.mpg", Mod::get()->getSaveDir() / "cote.mpg", [this]() {
                SecretVault::launchCOTE();
            })->show();
        #endif
        }
        
    }

    void updateSearchLabel(const char* text) {
        SecretLayer2::updateSearchLabel(text);
    }
};
