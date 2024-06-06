// SPOILERSSS
#include "COTE.h"
#ifdef GEODE_IS_WINDOWS
#include "../Utils/DownloadManager.h"
#else // mac
#include <ctime>
#include <cxxabi.h>
#endif

COTE* COTE::create() {
    auto pRet = new COTE();
    if (pRet && pRet->COTE::init()) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
};

bool COTE::init() {
    auto menu = CCMenu::create();
    //#ifndef GEODE_IS_MACOS
    #ifdef GEODE_IS_WINDOWS
    videoplayer::VideoPlayer* player = videoplayer::VideoPlayer::create("./gdutils/cote.mpg", true);
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

void COTE::keyBackClicked() {
    setKeypadEnabled(false);

    #ifdef GEODE_IS_UNKNOWN
    GameSoundManager::sharedManager()->playBackgroundMusic("./Resources/secretLoop02.mp3", true, false);
    #endif
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

// cote time