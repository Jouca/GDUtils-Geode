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
    GDUtils::Events::OnRate().listen([](EventData const& e) {
        log::info("A rate event with the title {}", e.getTitle());
        return ListenerResult::Propagate;
    });

    GDUtils::Events::OnRate(EventType::Daily).listen([](EventData const& e) {
        log::info("A daily event with the level name {}", e.getLevelName());
        return ListenerResult::Propagate;
    });

    GDUtils::Events::OnServerConnect().listen([](bool const& connected) {
        Loader::get()->queueInMainThread([connected]() {
            if (connected) {
                log::info("I am connected to the notification servers!");
            }
        });
        return ListenerResult::Propagate;
    });
}
