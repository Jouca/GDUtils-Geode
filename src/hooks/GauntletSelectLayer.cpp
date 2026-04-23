#include <defs/geode.hpp>
#include <Geode/modify/GauntletSelectLayer.hpp>

struct GauntletParticles {
    std::string particle;
    CCPoint pos;
    CCPoint scale = CCPoint(1.0f, 1.0f);
    float rotation = 0.0f;
    int zOrder = 1;
    bool useGauntletPos = true;
};

struct NewGauntlet {
    std::vector<GauntletParticles> particles;
    std::string background;
    float bgOffsetY = 0.0f;
    float scaleBg = 1.0f;
    float scaleGauntlet = 0.0f;
    CCPoint gauntletPos;
    float gauntletRotation = 0.0f;
    bool gauntletFlipX = false;
    int gauntletZOrder = 2;
    bool reverseGauntlet = false;
    int itemZOrder = 0;
};

template<>
struct matjson::Serialize<NewGauntlet> {
    static geode::Result<NewGauntlet> fromJson(matjson::Value const& value) {
        NewGauntlet data;
        GEODE_UNWRAP_INTO(data.background, value["background"]["sprite"].asString());
        GEODE_UNWRAP_INTO(data.scaleBg, value["background"]["scale"].asDouble());

        if (value["background"].contains("offsetY")) {
            data.bgOffsetY = static_cast<float>(value["background"]["offsetY"].asDouble().unwrapOrDefault());
        }
        if (value.contains("reverseGauntlet")) {
            data.reverseGauntlet = value["reverseGauntlet"].asBool().unwrapOrDefault();
        }
        if (value.contains("itemZOrder")) {
            data.itemZOrder = value["itemZOrder"].asInt().unwrapOrDefault();
        }
        if (value.contains("gauntlet")) {
            auto const& g = value["gauntlet"];
            if (g.contains("position")) {
                GEODE_UNWRAP_INTO(auto gPos, g["position"].asArray());
                data.gauntletPos = CCPoint(
                    static_cast<float>(gPos.at(0).asDouble().unwrapOrDefault()),
                    static_cast<float>(gPos.at(1).asDouble().unwrapOrDefault())
                );
            }
            GEODE_UNWRAP_INTO(data.scaleGauntlet, g["scale"].asDouble());
            if (g.contains("zOrder")) {
                data.gauntletZOrder = g["zOrder"].asInt().unwrapOrDefault();
            }
            if (g.contains("rotation")) {
                data.gauntletRotation = static_cast<float>(g["rotation"].asDouble().unwrapOrDefault());
            }
            if (g.contains("flipX")) {
                data.gauntletFlipX = g["flipX"].asBool().unwrapOrDefault();
            }
        }
        if (value.contains("particles")) {
            GEODE_UNWRAP_INTO(auto particles, value["particles"].asArray());
            for (auto const& p : particles) {
                GauntletParticles pd;
                GEODE_UNWRAP_INTO(pd.particle, p["string"].asString());

                auto posOpt = p["position"].asArray();
                if (posOpt.isOk()) {
                    auto const& pos = posOpt.unwrap();
                    pd.pos = CCPoint(
                        static_cast<float>(pos.at(0).asDouble().unwrapOrDefault()),
                        static_cast<float>(pos.at(1).asDouble().unwrapOrDefault())
                    );
                    pd.useGauntletPos = false;
                }
                if (p.contains("scale")) {
                    GEODE_UNWRAP_INTO(auto sc, p["scale"].asArray());
                    pd.scale = CCPoint(
                        static_cast<float>(sc.at(0).asDouble().unwrapOrDefault()),
                        static_cast<float>(sc.at(1).asDouble().unwrapOrDefault())
                    );
                }
                if (p.contains("zOrder")) {
                    pd.zOrder = p["zOrder"].asInt().unwrapOrDefault();
                }
                if (p.contains("rotation")) {
                    pd.rotation = static_cast<float>(p["rotation"].asDouble().unwrapOrDefault());
                }
                data.particles.push_back(std::move(pd));
            }
        }
        return geode::Ok(data);
    }
};

class $modify(CustomGauntletSelectLayer, GauntletSelectLayer) {
    void reverseGauntlet(GauntletNode* node) {
        if (!node->m_gauntletInfoNode) return;
        if (auto checkmark = node->getChildByID("checkmark-sprite")) {
            checkmark->setPositionY(78);
            if (auto shadow = node->getChildByID("checkmark-shadow-sprite")) shadow->setPositionY(78);
        }
        if (auto label_reward = node->m_gauntletInfoNode->getChildByID("reward-label")) {
            label_reward->setPositionY(50);
            if (auto s = node->m_gauntletInfoNode->getChildByID("reward-shadow-label")) s->setPositionY(50);
            if (auto chest = static_cast<CCSprite*>(node->m_gauntletInfoNode->getChildByID("chest-sprite"))) {
                chest->setPositionY(80);
                chest->setFlipY(true);
            }
            if (auto chest_s = static_cast<CCSprite*>(node->m_gauntletInfoNode->getChildByID("chest-shadow-sprite"))) {
                chest_s->setPositionY(80);
                chest_s->setFlipY(true);
            }
        }
        if (auto lp = node->m_gauntletInfoNode->getChildByID("gauntlet-progress-label")) lp->setPositionY(29);
        if (auto lps = node->m_gauntletInfoNode->getChildByID("gauntlet-progress-shadow-label")) lps->setPositionY(29);
    }

    bool init(int p0) {
        if (!GauntletSelectLayer::init(p0)) return false;
        if (auto menu = this->getChildByIDRecursive("bottom-right-menu")) {
            Build<CCSprite>::createSpriteName("GJ_infoIcon_001.png").color(236, 149, 19).intoMenuItem([]() {
                FLAlertLayer::create(
                    nullptr,
                    "Gauntlets redesign",
                    R"text(
All <cl>new designs for Gauntlets</c> are made by <cy>xvia</c> and <cy>brift</c>.
<cl>Particles</c> are made by <cy>GammaReXx</c>.

Thanks for their contributions!
                    )text",
                    "OK",
                    nullptr,
                    300.0f
                )->show();
            }).id("particle-info-button"_spr).zOrder(10).with([](CCNode* node) {
                auto* p = GameToolbox::particleFromString(
                    "26a-1a1.66a0.3a30a90a180a14a0a1a1a0a7a0a0a0a0a2a1a0a0a1a0a0.568627a0a0.12549a0a1a0a1a1a0a0a1a0a0.501961a0a0a0a1a0a0a0a0.46a0a0a0a0a0a0a0a0a2a1a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0a0",
                    nullptr, false
                );
                node->addChildAtPosition(p, Anchor::Center);
                p->setZOrder(9);
            }).parent(menu);
            menu->updateLayout();
        }
        return true;
    }

    void setupGauntlets() {
        GauntletSelectLayer::setupGauntlets();
        if (!m_scrollLayer) return;

        auto res = file::readJson(Mod::get()->getResourcesDir() / "particles.json");
        if (res.isErr()) {
            log::error("Couldn't read gauntlets/particles.json: {}", res.unwrapErr());
            return;
        }
        auto const& json = res.unwrap();
        auto const scaleFactor = 4.f / CCDirector::get()->getContentScaleFactor();

        for (int i = 0; i < m_scrollLayer->m_pages->count(); i++) {
            auto* page = static_cast<CCNode*>(
                static_cast<CCNode*>(m_scrollLayer->m_pages->objectAtIndex(i))
                ->getChildren()->objectAtIndex(0)
            )->getChildren();

            for (int j = 0; j < page->count(); j++) {
                auto* item = static_cast<CCNode*>(page->objectAtIndex(j));

                if (j == 0 && page->count() > 1) {
                    item->setPositionX(item->getPositionX() - 25.0f);
                } else if (j == 1 && page->count() == 2) {
                    item->setPositionX(item->getPositionX() + 25.0f);
                } else if (j == 2) {
                    item->setPositionX(item->getPositionX() + 25.0f);
                }

                auto* node = static_cast<GauntletNode*>(item->getChildByIDRecursive("gauntlet-node"));
                auto* label = static_cast<CCLabelBMFont*>(node->getChildByID("gauntlet-name-label"));
                auto* label_shadow = node->getChildByID("gauntlet-name-shadow-label");
                auto* label_gauntlet = node->getChildByID("gauntlet-label");
                auto* label_shadow_gauntlet = node->getChildByID("gauntlet-shadow-label");
                auto* bg = node->getChildByID("background");
                auto* gauntlet_node = node->m_gauntletInfoNode;
                auto* gauntlet = static_cast<CCSprite*>(gauntlet_node->getChildByID("gauntlet-sprite"));
                auto* gauntlet_shadow = gauntlet_node->getChildByID("gauntlet-shadow-sprite");

                bg->setVisible(false);
                label->setVisible(false);
                label_shadow->setVisible(false);
                label_gauntlet->setVisible(false);
                label_shadow_gauntlet->setVisible(false);
                gauntlet->setVisible(false);
                gauntlet_shadow->setVisible(false);

                std::string gauntletName = label->getString();

                auto restoreVisibility = [&]() {
                    bg->setVisible(true);
                    label->setVisible(true);
                    label_shadow->setVisible(true);
                    label_gauntlet->setVisible(true);
                    label_shadow_gauntlet->setVisible(true);
                    gauntlet->setVisible(true);
                    gauntlet_shadow->setVisible(true);
                };

                if (!json.contains(gauntletName)) {
                    restoreVisibility();
                    continue;
                }

                auto gdOpt = matjson::Serialize<NewGauntlet>::fromJson(json[gauntletName]);
                if (gdOpt.isErr()) {
                    log::warn("Couldn't parse gauntlet '{}': {}", gauntletName, gdOpt.unwrapErr());
                    restoreVisibility();
                    continue;
                }
                auto const& gd = gdOpt.unwrap();

                auto bgPos = bg->getPosition();
                bgPos.y += gd.bgOffsetY;

                auto* bgSprite = CCSprite::create(fmt::format("{}"_spr, gd.background).c_str());
                bgSprite->setPosition(bgPos);
                bgSprite->setScale(gd.scaleBg / scaleFactor);
                bgSprite->setZOrder(0);
                bgSprite->setID("gauntlet-background"_spr);
                node->addChild(bgSprite);

                if (gd.itemZOrder != 0) item->setZOrder(gd.itemZOrder);
                if (gd.reverseGauntlet) reverseGauntlet(node);

                if (gd.scaleGauntlet != 0.f) {
                    gauntlet->setVisible(true);
                    gauntlet->setPosition(gd.gauntletPos);
                    gauntlet->setScale(gd.scaleGauntlet);
                    if (gd.gauntletRotation != 0.f) gauntlet->setRotation(gd.gauntletRotation);
                    if (gd.gauntletFlipX) gauntlet->setFlipX(true);
                    gauntlet_node->setZOrder(gd.gauntletZOrder);
                }

                for (size_t k = 0; k < gd.particles.size(); k++) {
                    auto const& pd = gd.particles[k];
                    auto* p = GameToolbox::particleFromString(pd.particle, nullptr, false);
                    node->addChild(p);
                    p->setZOrder(pd.zOrder);
                    p->setPosition(pd.useGauntletPos ? gauntlet->getPosition() : pd.pos);
                    p->setScaleX(pd.scale.x);
                    p->setScaleY(pd.scale.y);
                    if (pd.rotation != 0.f) p->setRotation(pd.rotation);
                    p->setID(fmt::format("gauntlet-particles-{}"_spr, k + 1));
                }
            }
        }
    }
};

static void toggleHook(bool value) {
    for (auto& hook : geode::Mod::get()->getHooks()) {
        if (hook->getDisplayName().starts_with("GauntletSelectLayer")) {
            if (value) {
                (void)hook->enable();
            } else {
                (void)hook->disable();
            }
        }
    }
}

$execute {
    listenForSettingChanges<bool>("gauntletDesign", [](bool value) {
        log::info("Gauntlet Design changed to {}", value);
        toggleHook(value);
    });
}

$on_mod(Loaded) {
    if (!Mod::get()->getSettingValue<bool>("gauntletDesign")) {
        toggleHook(false);
    }
}
