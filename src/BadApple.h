// SPOILERSSS
#pragma once
#include <Geode/Geode.hpp>
#include "VideoPlayer.hpp"

using namespace cocos2d;
class BadApple : public CCLayer {
    protected:
        virtual bool init();
        virtual void keyBackClicked();

    public:
        static BadApple* create();
};
