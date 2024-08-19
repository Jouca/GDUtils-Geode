#pragma once
#include <iostream>
#include <Geode/Geode.hpp>
#include "../MoreLeaderboards/MoreLeaderboards.h"

using namespace geode::prelude;

class NewProfilePage : public CCLayer {
    public:
        void onBadgePressed(CCObject* pSender);
        void onGDUtilsDevBadgePressed(CCObject* pSender);
        void onGDUtilsContributorBadgePressed(CCObject* pSender);
        void onGDUtilsArtistBadgePressed(CCObject* pSender);
        void onGDUtilsGDAwards2023BadgePressed(CCObject* pSender);
        void onGDUtilsSupporterBadgePressed(CCObject* pSender);
        void onULDevBadgePressed(CCObject* pSender);
        void onULOfficerBadgePressed(CCObject* pSender);
        void onULHelperBadgePressed(CCObject* pSender);
        void onRobTopBadgePressed(CCObject* pSender);
};
