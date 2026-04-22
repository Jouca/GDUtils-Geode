#include <Geode/modify/LeaderboardsLayer.hpp>
#include <defs/geode.hpp>
#include <ui/moreleaderboards/MoreLeaderboards.hpp>

class $modify(LeaderboardsLayer) {
    bool init(LeaderboardType type, LeaderboardStat stat) {
        if (!LeaderboardsLayer::init(type, stat)) return false;

        auto menu = CCMenu::create();

        Build<CCSprite>::createSpriteName("GJ_plusBtn_001.png").scale(.8f)
            .intoMenuItem([]() {
                auto scene = CCScene::create();
                scene->addChild(MoreLeaderboards::create("more"));
                CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5, scene));
            }).pos(-239, 20).id("more-leaderboards-btn"_spr).parent(menu);

        Build<CCSprite>::createSpriteName("modBadge_01_001.png").scale(1.5f)
            .intoMenuItem([]() {
                auto scene = CCScene::create();
                scene->addChild(MoreLeaderboards::create("mods"));
                CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5, scene));
            }).pos(-239, -24).id("mods-list-btn"_spr).parent(menu);

        menu->setID("gdutils-menu"_spr);
        this->addChild(menu);
        return true;
    }
};
