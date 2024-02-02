#include "DailyChest.h"
#include "EventsPush.h"

void DailyChest::getRewards(unsigned int type) {
    GameLevelManager* glm = GameLevelManager::sharedState();
    glm->m_GJRewardDelegate = this;
    glm->getGJRewards(0);
};

void DailyChest::rewardsStatusFinished(int p0) {
    int time1 = -1;
    int time2 = -1;
    time1 = GameStatsManager::sharedState()->getRewardItem(GJRewardType::Small);
    time2 = GameStatsManager::sharedState()->getRewardItem(GJRewardType::Large);

    sio::message::ptr msg = sio::object_message::create();
    msg->get_map()["sprite"] = sio::string_message::create("GJ_square04-uhd.png");
    msg->get_map()["level_name"] = sio::string_message::create("");
    msg->get_map()["level_creator"] = sio::string_message::create("None");
    msg->get_map()["demon"] = sio::int_message::create(0);
    msg->get_map()["starsum"] = sio::int_message::create(0);
    msg->get_map()["stars"] = sio::int_message::create(0);
    msg->get_map()["rate"] = sio::int_message::create(0);
    msg->get_map()["coins"] = sio::int_message::create(0);
    msg->get_map()["verified_coins"] = sio::int_message::create(0);
    msg->get_map()["platformer"] = sio::int_message::create(0);

    if (time1 == 0) {
        msg->get_map()["type"] = sio::int_message::create(3);
        msg->get_map()["title"] = sio::string_message::create("Small Daily Chest available!");
        EventsPush::pushRateLevel(CCDirector::sharedDirector()->getRunningScene(), msg);
    }

    if (time2 == 0) {
        msg->get_map()["type"] = sio::int_message::create(4);
        msg->get_map()["title"] = sio::string_message::create("Big Daily Chest available!");
        EventsPush::pushRateLevel(CCDirector::sharedDirector()->getRunningScene(), msg);
    }
};