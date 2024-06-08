#pragma once

#include <Geode/Geode.hpp>

class DailyChest : public CCNode, public GJRewardDelegate {
    public:
        void getRewards(unsigned int type);
        void rewardsStatusFinished(int p0) override;
        void rewardsStatusFailed() override;
};
