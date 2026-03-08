#include <Geode/modify/LeaderboardsLayer.hpp>
#include <defs/geode.hpp>
//#include <ui/moreleaderboards/MoreLeaderboards.hpp>

class $modify(LeaderboardsLayer) {
    bool init(LeaderboardType type, LeaderboardStat stat) {
        if (!LeaderboardsLayer::init(type, stat)) return false;
        Build<CCMenu>::create().with([](auto node) {
            Build<CCSprite>::createSpriteName("GJ_plusBtn_001.png").scale(.8f).intoMenuItem([]() {
                //CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5, MoreLeaderboards::scene()));
                FLAlertLayer::create("Notice", "This currently has not been <cy>implemented yet!</c>\nPlease wait until the <cy>next major update</c> for this feature to be added.", "OK")->show();
            }).pos(-239,20).id("more-leaderboards-btn"_spr).parent(node);
            Build<CCSprite>::createSpriteName("modBadge_01_001.png").scale(1.5f).intoMenuItem([]() {
                FLAlertLayer::create("Notice", "This currently has not been <cy>implemented yet!</c>\nPlease wait until the <cy>next major update</c> for this feature to be added.", "OK")->show();
            }).pos(-239,-24).id("mods-list-btn"_spr).parent(node);
        }).id("gdutils-menu"_spr).parent(this);
        return true;
    }
};
