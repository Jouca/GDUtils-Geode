// SPOILERSSS
#pragma once
#include <defs/geode.hpp>

class COTE : public cocos2d::CCLayer {
    protected:
        virtual bool init();
        virtual void keyBackClicked();

    public:
        static COTE* create();
};
