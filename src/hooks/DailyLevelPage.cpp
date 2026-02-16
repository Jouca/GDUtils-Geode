#include <defs/geode.hpp>
#include <Geode/modify/DailyLevelPage.hpp>
#include <ui/EventInfoPopup.hpp>

class $modify(EventPage, DailyLevelPage) {
    bool init(GJTimedLevelType type) {
        if (!DailyLevelPage::init(type)) return false;

        if (type != GJTimedLevelType::Event) return true;

        if (auto menu = this->m_mainLayer->getChildByIDRecursive("main-menu")) {
            Build<CCSprite>::createSpriteName("info_btn.png"_spr).intoMenuItem([]() {
                EventInfoPopup::create()->show();
            }).pos(14,-194).parent(menu);
        }
        return true;
    }
};

static void toggleHook(bool value) {
    for (auto& hook : geode::Mod::get()->getHooks()) {
        if (hook->getDisplayName().starts_with("DailyLevelPage")) {
            (void)(value ? hook->enable() : hook->disable());
        }
    }
}

$execute {
    listenForSettingChanges<bool>("eventLevelInfo", [](bool value) {
        log::info("Event Level Info changed to {}", value);
        toggleHook(value);
    });
}

$on_mod(Loaded) {
    if (!Mod::get()->getSettingValue<bool>("eventLevelInfo")) {
        toggleHook(false);
    }
}

