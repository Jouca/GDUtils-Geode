#pragma once 

#include <Geode/loader/Event.hpp>
#include "Types.hpp"

namespace GDUtils::Events {
    class RateEvent : public geode::SimpleEvent<RateEvent, EventData> {
    public:
        using SimpleEvent::SimpleEvent;
        static void emit(EventData data) {
            RateEvent().send(data);
        };
        static EventType intToType(int type) {
            switch (type) {
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
        static int typeToInt(EventType type) {
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
    };
    struct OnRate final : geode::GlobalEvent<OnRate, bool(EventData&), bool(EventData&), EventType> {
        using GlobalEvent::GlobalEvent;
    };
}
