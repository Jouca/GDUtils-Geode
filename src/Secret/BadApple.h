// SPOILERSSS
#pragma once
#include "../includes.h"
#include "VideoPlayer.hpp"

class BadApple : public CCLayer {
    protected:
        virtual bool init();
        virtual void keyBackClicked();

    public:
        static BadApple* create();
};
