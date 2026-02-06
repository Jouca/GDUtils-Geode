// SPOILERSSS
#include "BadApple.hpp"
#ifdef GEODE_IS_WINDOWS
#include "VideoPlayer.hpp"
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
    //#ifndef GEODE_IS_MACOS
    #ifdef GEODE_IS_WINDOWS
    videoplayer::VideoPlayer* player = videoplayer::VideoPlayer::create(Mod::get()->getSaveDir() / "bad_apple.mpg", true);
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
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}
