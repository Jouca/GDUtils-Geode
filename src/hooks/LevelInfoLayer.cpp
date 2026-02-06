#include <defs/geode.hpp>
#include <string>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/utils/web.hpp>

// demon list
std::unordered_map<int, int> demonListCache; // Will clear after game exit, or if user deletes level

class $modify(LevelInfoLayer) {
    struct Fields {
        async::TaskHolder<web::WebResponse> m_listener;
    };
    void set(int key, int value) {
        demonListCache[key] = value;
    }
    bool exists(int key) {
        return demonListCache.find(key) != demonListCache.end();
    }
    int get(int key) {
        auto it = demonListCache.find(key);
        if (it != demonListCache.end()) {
            return it->second;
        }
        return -1;
    }
    bool remove(int key) {
        return demonListCache.erase(key) == 1;
    }
    float getScaleBasedPos(int pos) {
        if (pos > 0 && pos < 10) return 0.5F;
        if (pos > 10 && pos < 100) return 0.4F;
        if (pos > 100 && pos < 1000) return 0.3F;
        if (pos > 1000 && pos < 10000) return 0.2F;
        return 0.1F;
    }
    bool init(GJGameLevel* level, bool p1) { // inspiration le gdbrowser, what does p1 do? idk
        if (!LevelInfoLayer::init(level, p1)) return false;
        if (level->m_demon.value() == 0 || level->m_stars.value() != 10) return true;
        if (level->m_demonDifficulty != 6) return true;
        
        auto director = CCDirector::sharedDirector();
        auto winSize = director->getWinSize();
        auto demonSpr = Build<CCSprite>::createSpriteName("diffIcon_06_btn_001.png")
            .pos({
                (winSize.width / 2) - 85,
                (level->m_coins > 0) ? (winSize.height / 2) : (winSize.height / 2) + 18
            })
            .scale(0.5F)
            .id("demon-spr"_spr)
            .visible(false)
            .parent(this)
            .collect();
        auto positionLabel = Build<CCLabelBMFont>::create("N/A", "bigFont.fnt")
            .pos({
                (winSize.width / 2) - 107,
                (level->m_coins > 0) ? (winSize.height / 2) : (winSize.height / 2) + 18
            })
            .scale(0.5F)
            .id("demon-pos"_spr)
            .visible(false)
            .parent(this)
            .collect();
        int levelID = level->m_levelID.value();
        if (exists(levelID)) {
            int position = get(levelID);
            if (position > 0) {
                positionLabel->setString(fmt::format("#{}", position).c_str());
                positionLabel->setScale(getScaleBasedPos(position));
                positionLabel->setVisible(true);
                demonSpr->setVisible(true);
            }
        } else {
            auto loading_circle = LoadingCircle::create();
            if (level->m_coins > 0) {
                loading_circle->setPosition({-100, -2});
                loading_circle->setScale(0.3F);
            } else {
                loading_circle->setPosition({-100, 8});
                loading_circle->setScale(0.4F);
            }
            loading_circle->setParentLayer(this);
            loading_circle->show();
            std::string listId = Mod::get()->getSettingValue<std::string>("demonListSelection");
            std::string url = "";
            if (listId == "Pointercrate") {
                url = fmt::format("https://pointercrate.com/api/v2/demons/listed/?level_id={}", levelID);
            } else {
                url = fmt::format("https://api.aredl.net/api/aredl/levels/{}", levelID);
            }
            geode::utils::web::WebRequest request = web::WebRequest();
            m_fields->m_listener.spawn("@loaderDemonListLevelInfo", request.get(url.c_str()), [this, loading_circle, positionLabel, demonSpr, level, levelID](web::WebResponse value) {
                if (value.ok()) {
                    matjson::Value json = value.json().unwrapOrDefault();

                    std::string listId = Mod::get()->template getSettingValue<std::string>("demonListSelection");

                    if (listId == "Pointercrate") {
                        if (loading_circle != nullptr) {
                            loading_circle->fadeAndRemove();
                        }
                        auto scene = CCDirector::sharedDirector()->getRunningScene();
                        if (json.dump() == "[]") { //idk how to check size, doing .count crashes
                            log::info("Level not found in Pointercrate.");
                        } else {
                            auto info = json.get(0);
                            if (info.isOk()) {
                                auto positionRes = info.unwrap().get("position");
                                if (positionRes.isOk()) {
                                    int position = positionRes.unwrap().asInt().unwrapOrDefault();
                                    positionLabel->setString(fmt::format("#{}", position).c_str());
                                    positionLabel->setScale(getScaleBasedPos(position));
                                    positionLabel->setVisible(true);
                                    demonSpr->setVisible(true);
                                    set(levelID, position);
                                    log::info("Level found in Pointercrate! {} at #{}", level->m_levelName.c_str(), position);
                                }
                            }

                        }
                    } else {
                        if (loading_circle != nullptr) {
                            loading_circle->fadeAndRemove();
                        }
                        auto scene = CCDirector::sharedDirector()->getRunningScene();
                        if (json.contains("code")) {
                        } else {
                            auto positionRes = json.get("position");
                            if (positionRes.isOk()) {
                                int position = positionRes.unwrap().asInt().unwrapOrDefault();
                                positionLabel->setString(fmt::format("#{}", position).c_str());
                                positionLabel->setScale(getScaleBasedPos(position));
                                positionLabel->setVisible(true);
                                demonSpr->setVisible(true);
                                set(levelID, position);
                                log::info("Level found in AREDL! {} at #{}", level->m_levelName.c_str(), position);
                            }
                        }
                    }
                } else {
                    if (loading_circle != nullptr) {
                        loading_circle->fadeAndRemove();
                    }
                    log::error("Error while sending a request to Demon List: {}", value.errorMessage());
                }
            });
        }
        return true;
    }
};
// demon list

void toggleHook(bool value) {
    for (auto& hook : geode::Mod::get()->getHooks()) {
        if (hook->getDisplayName() == "LevelInfoLayer::init") {
            if (value) {
                (void)hook->enable();
            } else {
                (void)hook->disable();
            }
        }
    }
}

$execute {
    listenForSettingChanges<bool>("demonListPlacement", [](bool value) {
        log::info("Demon List Placement changed to {}", value);
        toggleHook(value);
    });
}

$on_mod(Loaded) {
    if (!Mod::get()->getSettingValue<bool>("demonListPlacement")) {
        toggleHook(false);
    }
}
