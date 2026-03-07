#pragma once 
#include <Geode/Geode.hpp>
#include "RateEvent.hpp"
enum EventType {
    Rate,
    Daily,
    Weekly,
    Event,
    smallChest,
    largeChest,
    List,
    Announcement,
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
    std::string getTitle() const { return title; };
    EventType getEventType() const { return type; };
    std::string getLevelName() const { return level_name; };
    int getLevelID() const { return level_id; };
    int typeToInt() const {
        switch (type) {
            case EventType::NA:
                return -1;
            case EventType::Rate: // Rate
                return 0;
            case EventType::Daily: // Daily
                return 1;
            case EventType::Weekly: // Weekly
                return 2;
            case EventType::smallChest: // Small chest
                return 3;
            case EventType::largeChest: // Large chest
                return 4;
            case EventType::List: // List
                return 5;
            case EventType::Event: // Event
                return 6;
            case EventType::Announcement: // Announcement 
                return 7;
        }
    }
    EventType intToType(int intType) const {
        switch (intType) {
            default:
                return EventType::NA;
            case 0: // Rate
                return EventType::Rate;
            case 1: // Daily
                return EventType::Daily;
            case 2: // Weekly
                return EventType::Weekly;
            case 3: // Small chest
                return EventType::smallChest;
            case 4: // Large chest
                return EventType::largeChest;
            case 5: // List
                return EventType::List;
            case 6: // Event
                return EventType::Event;
            case 7: // Announcement
                return EventType::Announcement;
        }
    }
};

template<>
struct matjson::Serialize<EventData> {
    static geode::Result<EventData> fromJson(matjson::Value const& value) {
        EventData data;
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
            case 7: // Announcement
                data.type = EventType::Announcement;
                break;
        }
        GEODE_UNWRAP_INTO(data.title, value["title"].asString());
        if (value.contains("sprite")) {
            GEODE_UNWRAP_INTO(data.sprite, value["sprite"].asString());
        }
        if (data.type != EventType::Announcement) {
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
            GEODE_UNWRAP_INTO(data.level_name, value["level_name"].asString());
            GEODE_UNWRAP_INTO(data.level_creator, value["level_creator"].asString());
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
        }
        return geode::Ok(data);
    }
    static matjson::Value toJson(EventData const& value) {
        auto val = Value::object();
        val.set("demon", value.demon);
        val.set("starsum", value.starsum);
        val.set("starsum", value.starsum);
        val.set("stars", value.stars);
        val.set("rate", value.rate);
        val.set("type", value.typeToInt());
        val.set("title", value.title);
        val.set("level_name", value.level_name);
        val.set("level_creator", value.level_creator);
        val.set("coins", value.coins);
        val.set("verified_coins", value.verified_coins);
        val.set("platformer", value.platformer);
        val.set("level_id", value.level_id);
        val.set("levels_list", value.levels_list);
        val.set("maxToCompleteList", value.maxToCompleteList);
        return val;
    }
};
