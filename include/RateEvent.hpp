#pragma once 

#include <Geode/loader/Event.hpp>
#include "Types.hpp"

namespace GDUtils::Events {
    class RateEvent : public geode::Event<RateEvent, bool(EventData&)> {
    public:
        using Event::Event;
        static void emit(EventData data) {
            RateEvent().send(data);
        };
    };
    struct OnRate final : geode::GlobalEvent<OnRate, bool(EventData&), bool(EventData&), EventType> {
        using GlobalEvent::GlobalEvent;
    };
    struct OnServerConnect final : geode::ThreadSafeEvent<OnServerConnect, bool(bool)> {
        using ThreadSafeEvent::ThreadSafeEvent;
    };
}
