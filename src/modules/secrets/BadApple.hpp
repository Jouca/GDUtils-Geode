// SPOILERSSS
#pragma once
#include <defs/geode.hpp>

class BadApple : public cocos2d::CCLayer {
    protected:
        virtual bool init();
        virtual void keyBackClicked();

    public:
        static BadApple* create();
};
