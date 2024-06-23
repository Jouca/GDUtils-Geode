#pragma once
#include <iostream>
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class NewProfilePage : public CCLayer {
    public:
        void onBadgePressed(CCObject* pSender);
        void onGDUtilsBadgePressed(CCObject* pSender);
        void onRobTopBadgePressed(CCObject* pSender);
};
