#pragma once
#include <iostream>
#include <Geode/Geode.hpp>
#include "../MoreLeaderboards/MoreLeaderboards.h"

using namespace geode::prelude;

class NewProfilePage : public CCLayer {
    public:
        void onBadgePressed(CCObject* pSender);
        void onRobTopBadgePressed(CCObject* pSender);
        void badgeFactoryAlert(CCObject* pSender);
};
