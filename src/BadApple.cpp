// SPOILERSSS
#include "BadApple.h"
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
