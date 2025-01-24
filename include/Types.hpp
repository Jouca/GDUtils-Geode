#pragma once 

#include <Geode/Geode.hpp>
enum EventType {
    Rate,
    Daily,
    Weekly,
    Event,
    smallChest,
    largeChest,
    List,
    NA
};

struct EventData {
    bool demon;
    uint8_t starsum;
    uint8_t stars;
    uint8_t rate;
    EventType type;
    std::string title;
    std::string sprite;
    std::string level_name;
    std::string level_creator;
    uint8_t coins;
    bool verified_coins;
    bool platformer;
    int level_id = -1;
};
