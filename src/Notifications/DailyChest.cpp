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

    sio::message::ptr msg = sio::object_message::create();
    msg->get_map()["sprite"] = sio::string_message::create("GJ_square04.png");
    msg->get_map()["level_name"] = sio::string_message::create(" ");
    msg->get_map()["level_creator"] = sio::string_message::create(" ");
    msg->get_map()["demon"] = sio::int_message::create(0);
    msg->get_map()["starsum"] = sio::int_message::create(0);
    msg->get_map()["stars"] = sio::int_message::create(0);
    msg->get_map()["rate"] = sio::int_message::create(0);
    msg->get_map()["coins"] = sio::int_message::create(0);
    msg->get_map()["verified_coins"] = sio::int_message::create(0);
    msg->get_map()["platformer"] = sio::int_message::create(0);

    if (time1 != nullptr) {
        if (time1->m_timeRemaining == 0) {
            msg->get_map()["type"] = sio::int_message::create(3);
            msg->get_map()["title"] = sio::string_message::create("Small Daily Chest available!");
            EventsPush::pushRateLevel(CCDirector::sharedDirector()->getRunningScene(), msg);
        }
    }

    if (time2 != nullptr) {
        if (time2->m_timeRemaining == 0) {
            msg->get_map()["type"] = sio::int_message::create(4);
            msg->get_map()["title"] = sio::string_message::create("Big Daily Chest available!");
            EventsPush::pushRateLevel(CCDirector::sharedDirector()->getRunningScene(), msg);
        }
    }
};

void DailyChest::rewardsStatusFailed() {
    log::error("Failed to get rewards");
};
