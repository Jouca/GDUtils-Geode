#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class DailyChest : public CCObject, public GJRewardDelegate {
    public:
        void getRewards();
        void rewardsStatusFinished(int p0) override;
        void rewardsStatusFailed() override;
        static void startGame();
};
