#include <Geode/Geode.hpp>
using namespace geode::prelude;
#include <gdutilsdevs.gdutils/include/RateEvent.hpp>

#include <Geode/modify/MenuLayer.hpp>
class $modify(MenuLayer) {
    void onMoreGames(cocos2d::CCObject *sender) {
        MenuLayer::onMoreGames(sender);
        log::info("Emit event");
        EventData data = {
            true,
            2,
            6,
            1,
            EventType::Rate,
            "Custom Notification!",
            "GJ_square02.png",
            "Tidal Wave",
            "By OniLinkGD",
            0,
            false,
            false
        }; // Level ID is optional
        GDUtils::Events::RateEvent::emit(data);
    }
};

$execute {
    new EventListener<EventFilter<GDUtils::Events::OnRate>>(+[](GDUtils::Events::OnRate* e) {
        log::info("A rate event with the title {}", e->getTitle());
        return ListenerResult::Propagate;
    });
}
