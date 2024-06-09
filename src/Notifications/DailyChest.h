#pragma once

#include <Geode/Geode.hpp>
using namespace geode::prelude;

class DailyChest : public CCNode, public GJRewardDelegate {
    public:
        void getRewards(unsigned int type);
        void rewardsStatusFinished(int p0) override;
        void rewardsStatusFailed() override;
};
