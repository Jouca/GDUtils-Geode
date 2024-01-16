#pragma once
#include <iostream>

using namespace cocos2d;
using namespace geode::prelude;

class NewProfilePage : public CCLayer {
    public:
        void onBadgePressed(CCObject* pSender);
        void onGDUtilsBadgePressed(CCObject* pSender);
};