#include "DailyChest.h"
#include "EventsPush.h"

static GJRewardItem* time1 = nullptr;
static GJRewardItem* time2 = nullptr;

void DailyChest::getRewards(unsigned int type) {
    auto glm = GameLevelManager::sharedState();
    if (glm) {
        glm->m_GJRewardDelegate = this;
        glm->getGJRewards(0);
    }
};

void DailyChest::rewardsStatusFinished(int p0) {
    GameStatsManager* gsm;
    // Check if rewards are loaded
    gsm = GameStatsManager::sharedState();
    if (gsm->m_rewardItems->count() == 0) return;

    // Get the rewards
    int time1_value = 0x1;
    int time2_value = 0x2;
    time1 = typeinfo_cast<GJRewardItem*>(gsm->m_rewardItems->objectForKey(time1_value));
    time2 = typeinfo_cast<GJRewardItem*>(gsm->m_rewardItems->objectForKey(time2_value));

    EventData data;
    data.sprite = "GJ_square04.png";
    data.level_name = " ";
    data.level_creator = " ";
    if (time1 != nullptr) {
        if (time1->m_timeRemaining == 0) {
            data.type = EventType::smallChest;
            data.title = "Small Daily Chest available!";
            EventsPush::pushRateLevel(CCDirector::sharedDirector()->getRunningScene(), data);
        }
    }

    if (time2 != nullptr) {
        if (time2->m_timeRemaining == 0) {
            data.type = EventType::largeChest;
            data.title = "Big Daily Chest available!";
            EventsPush::pushRateLevel(CCDirector::sharedDirector()->getRunningScene(), data);
        }
    }
};

void DailyChest::rewardsStatusFailed() {
    log::error("Failed to get rewards");
};
