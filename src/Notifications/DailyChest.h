#pragma once

using namespace cocos2d;

class DailyChest : public cocos2d::CCNode, public GJRewardDelegate {
    public:
        void getRewards(unsigned int type);
        void rewardsStatusFinished(int p0) override;
        void rewardsStatusFailed() override {};
};