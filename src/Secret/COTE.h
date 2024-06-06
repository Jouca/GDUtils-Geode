// SPOILERSSS
#pragma once
#include "../includes.h"
#include "VideoPlayer.hpp"

class COTE : public CCLayer {
    protected:
        virtual bool init();
        virtual void keyBackClicked();

    public:
        static COTE* create();
};