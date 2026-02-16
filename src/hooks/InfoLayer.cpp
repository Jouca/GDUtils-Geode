#include <defs/geode.hpp>
#include <Geode/modify/InfoLayer.hpp>

// Copy button for sharing levels
class $modify(ProtoInfoLayer, InfoLayer) {
    bool init(GJGameLevel* level, GJUserScore* userscore, GJLevelList* levellist) {
        if (!InfoLayer::init(level, userscore, levellist)) return false;
        if (level == nullptr) return true;
        std::string levelID = std::to_string(level->m_levelID.value());
        if (auto menu = typeinfo_cast<CCMenu*>(this->getChildByIDRecursive("left-side-menu"))) {
            Build<CCSprite>::createSpriteName("GJ_redoBtn_001.png").scale(0.7f).intoMenuItem([levelID = std::move(levelID)]() {
                utils::clipboard::write(fmt::format("https://gdutils.com/{}", levelID));
                auto notification = Notification::create("Copied level URL to clipboard!", NotificationIcon::Success);
                notification->setTime(0.2f);
                notification->show();
            }).pos(14, -55).parent(menu);
        }
        return true;
    }
};

static void toggleHook(bool value) {
    for (auto& hook : geode::Mod::get()->getHooks()) {
        if (hook->getDisplayName() == "InfoLayer::init") {
            if (value) {
                (void)hook->enable();
            } else {
                (void)hook->disable();
            }
        }
    }
}

$execute {
    listenForSettingChanges<bool>("levelsProtocolButton", [](bool value) {
        log::info("Level Protocols Button changed to {}", value);
        toggleHook(value);
    });
}

$on_mod(Loaded) {
    if (!Mod::get()->getSettingValue<bool>("levelsProtocolButton")) {
        toggleHook(false);
    }
}
