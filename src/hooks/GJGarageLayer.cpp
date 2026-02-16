#include <defs/geode.hpp>
#include <Geode/modify/ItemInfoPopup.hpp>
class $modify(ItemInfoPopup) {
    bool init(int id, UnlockType type) {
        if (!ItemInfoPopup::init(id, type)) return false;
        if (auto scene = CCScene::get()) {
            if (scene->getChildByID("GJGarageLayer")) {
                GameManager* gm = GameManager::sharedState();
                GJItemIcon* node = m_mainLayer->getChildByType<GJItemIcon*>(0);
                if (node && node->m_isIcon) {
                    auto player = static_cast<SimplePlayer*>(node->m_player);
                    player->setColor(gm->colorForIdx(gm->getPlayerColor()));
                    player->setSecondColor(gm->colorForIdx(gm->getPlayerColor2()));
                    player->m_glowColor = gm->colorForIdx(gm->getPlayerGlowColor());

                    Build<CCSprite>::createSpriteName("GJ_checkOff_001.png").scale(.8f).child(
                        Build<CCLabelBMFont>::create("P1", "bigFont.fnt").scale(.525f).pos(15, 17)
                    ).intoMenuItem([player]() {
                        GameManager* gm = GameManager::sharedState();
                        player->setColor(gm->colorForIdx(gm->getPlayerColor()));
                        player->setSecondColor(gm->colorForIdx(gm->getPlayerColor2()));
                    }).pos(-128, -40).id("p1-button"_spr).parent(m_buttonMenu);

                    Build<CCSprite>::createSpriteName("GJ_checkOff_001.png").scale(.8f).child(
                        Build<CCLabelBMFont>::create("P2", "bigFont.fnt").scale(.525f).pos(15, 17)
                    ).intoMenuItem([player]() {
                        GameManager* gm = GameManager::sharedState();
                        player->setColor(gm->colorForIdx(gm->getPlayerColor2()));
                        player->setSecondColor(gm->colorForIdx(gm->getPlayerColor()));
                    }).pos(-99, -40).id("p2-button"_spr).parent(m_buttonMenu);

                    Build<CCSprite>::createSpriteName("GJ_checkOff_001.png").scale(.8f).child(
                        Build<CCLabelBMFont>::create("G", "bigFont.fnt").scale(.525f).pos(15, 17)
                    ).intoMenuItem([player]() {
                        GameManager* gm = GameManager::sharedState();
                        if (!player->m_hasGlowOutline) {
                            player->setGlowOutline(gm->colorForIdx(gm->getPlayerGlowColor()));
                        } else {
                            player->disableGlowOutline();
                        }
                    }).pos(-69, -40).id("glow-button"_spr).parent(m_buttonMenu);
                }
                
            }
        }

        return true;
    }
};

static void toggleHook(bool value) {
    for (auto& hook : geode::Mod::get()->getHooks()) {
        if (hook->getDisplayName().starts_with("ItemInfoPopup")) {
            if (value) {
                (void)hook->enable();
            } else {
                (void)hook->disable();
            }
        }
    }
}

$execute {
    listenForSettingChanges<bool>("previewIcons", [](bool value) {
        log::info("Preview Icons changed to {}", value);
        toggleHook(value);
    });
}

$on_mod(Loaded) {
    if (!Mod::get()->getSettingValue<bool>("previewIcons")) {
        toggleHook(false);
    }
}
