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
    bool demon = false;
    uint16_t starsum = 0;
    uint16_t stars = 0;
    uint16_t rate = 0;
    EventType type = EventType::Rate;
    std::string title;
    std::string sprite = "GJ_square01.png";
    std::string level_name;
    std::string level_creator;
    uint8_t coins = 0;
    uint8_t verified_coins = 0;
    bool platformer = false;
    int level_id = -1;
    std::string levels_list;
    int maxToCompleteList = 0;
};

template<>
struct matjson::Serialize<EventData> {
    static geode::Result<EventData> fromJson(matjson::Value const& value) {
        EventData data;
        if (value.contains("demon")) {
            if (value["demon"].asUInt().isErr()) {
                GEODE_UNWRAP_INTO(data.demon, value["demon"].asBool());
            } else {
                GEODE_UNWRAP_INTO(auto demon, value["demon"].asUInt());
                data.demon = demon == 1;
            }
        }
        GEODE_UNWRAP_INTO(data.starsum, value["starsum"].asUInt());
        GEODE_UNWRAP_INTO(data.stars, value["stars"].asUInt());
        GEODE_UNWRAP_INTO(data.rate, value["rate"].asUInt());
        GEODE_UNWRAP_INTO(auto type, value["type"].asUInt());
        // yeahhhh im getting linker errors ok
        switch (type) {
            default:
                data.type = EventType::NA;
                break;
            case 0: // Rate
                data.type = EventType::Rate;
                break;
            case 1: // Daily
                data.type = EventType::Daily;
                break;
            case 2: // Weekly
                data.type = EventType::Weekly;
                break;
            case 3: // Small chest
                data.type = EventType::smallChest;
                break;
            case 4: // Large chest
                data.type = EventType::largeChest;
                break;
            case 5: // List
                data.type = EventType::List;
                break;
            case 6: // Event
                data.type = EventType::Event;
                break;
        }
        GEODE_UNWRAP_INTO(data.title, value["title"].asString());
        GEODE_UNWRAP_INTO(data.sprite, value["sprite"].asString());
        GEODE_UNWRAP_INTO(data.level_name, value["level_name"].asString());
        GEODE_UNWRAP_INTO(data.level_creator, value["level_creator"].asString());
        GEODE_UNWRAP_INTO(data.rate, value["rate"].asUInt());
        GEODE_UNWRAP_INTO(data.coins, value["coins"].asUInt());
        GEODE_UNWRAP_INTO(data.verified_coins, value["verified_coins"].asUInt());
        if (value.contains("platformer")) {
            if (value["platformer"].asUInt().isErr()) {
                GEODE_UNWRAP_INTO(data.platformer, value["platformer"].asBool());
            } else {
                GEODE_UNWRAP_INTO(auto platformer, value["platformer"].asUInt());
                data.platformer = platformer == 1;
            }
        }
        if (value.contains("level_id")) {
            if (value["level_id"].asString().isErr()) {
                GEODE_UNWRAP_INTO(data.level_id, value["level_id"].asInt());
            } else {
                GEODE_UNWRAP_INTO(auto level_id, value["level_id"].asString());
                data.level_id = geode::utils::numFromString<int>(level_id).unwrapOrDefault();
            }
        }
        if (value.contains("levels_list")) {
            GEODE_UNWRAP_INTO(data.levels_list, value["levels_list"].asString());
        }
        if (value.contains("maxToCompleteList")) {
            GEODE_UNWRAP_INTO(data.maxToCompleteList, value["maxToCompleteList"].asInt());
        }
        return geode::Ok(data);
    }
};
