#include "../includes.h"
#include <string>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/utils/web.hpp>


// demon list
std::unordered_map<int, int> demonListCache; // Will clear after game exit, or if user deletes level

// love url encoded characters :D
// also for some reason this is required on mac because Geode's web requests doesnt automatically append this for some reason.
// SURELY THIS WONT AFFECT LEVELS WITH SOME OTHER BREAKING SYMBOLS.
std::string url_encode(const std::string& value) {
    std::string encoded;
    encoded.reserve(value.size());
    for (char c : value) {
        if (c == ' ') {
            encoded += "%20";
        } else {
            encoded += c;
        }
    }
    return encoded;
}

class $modify(LevelInfoLayer) {
    // chat jippity
    void set(int key, int value) {
        demonListCache[key] = value;
    }

    // Function to check if a key exists in the cache.
    bool exists(int key) {
        return demonListCache.find(key) != demonListCache.end();
    }

    // Function to get a value from the cache.
    int get(int key) {
        auto it = demonListCache.find(key);
        if (it != demonListCache.end()) {
            return it->second;
        }
        // Return -1 or some other default value to indicate the key wasn't found.
        return -1;
    }

    bool remove(int key) {
        return demonListCache.erase(key) == 1;
    }
    float getScaleBasedPos(int pos) {
        if (pos > 0 && pos < 10) return 0.5F;
        if (pos > 10 && pos < 100) return 0.4F;
        if (pos > 100 && pos < 1000) return 0.3F;
        return 0.1F;
    }
    bool init(GJGameLevel* level, bool p1) { // inspiration le gdbrowser, what does p1 do? idk
        if (!LevelInfoLayer::init(level, p1)) return false;
        if (!Mod::get()->getSettingValue<bool>("demonListPlacement")) return true;
        
        if (level->m_demon.value() == 0 || level->m_stars.value() != 10) return true;
        if (level->m_demonDifficulty != 6) return true;
        
        auto director = CCDirector::sharedDirector();
        auto winSize = director->getWinSize();
        auto demonSpr = CCSprite::createWithSpriteFrameName("diffIcon_06_btn_001.png");
        //auto demonSpr = CCSprite::createWithSpriteFrameName("difficulty_06_btn_001.png");
        demonSpr->setPosition({(winSize.width / 2) - 85, (winSize.height / 2) + 18});
        demonSpr->setScale(0.5F);
        auto positionLabel = CCLabelBMFont::create("N/A", "bigFont.fnt");
        positionLabel->setPosition({(winSize.width / 2) - 107, (winSize.height / 2) + 18}); // 178
        positionLabel->setScale(0.5F);
        positionLabel->setVisible(false);
        demonSpr->setVisible(false);
        this->addChild(demonSpr);
        this->addChild(positionLabel);
        if (level->m_coins > 0) {
            positionLabel->setPositionY(positionLabel->getPositionY() - 18);
            demonSpr->setPositionY(demonSpr->getPositionY() - 18);
        }
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
            this->retain();
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
            log::info("Sending a request to pointercrate...");
            web::AsyncWebRequest()
                .fetch(fmt::format("https://pointercrate.com/api/v2/demons/listed/?name={}", url_encode(level->m_levelName).c_str()))
                .json()
                .then([this, level, levelID, loading_circle, positionLabel, demonSpr, winSize](matjson::Value const& json) {
                    if (loading_circle != nullptr) {
                        loading_circle->fadeAndRemove();
                    }
                    auto scene = CCDirector::sharedDirector()->getRunningScene();
                    //auto layer = misc::findNode("LevelInfoLayer");
                    //if (layer == nullptr) return;
                    if (json.dump() == "[]") { //idk how to check size, doing .count crashes
                        log::info("Level not found in pointercrate.");
                        this->release();
                    } else {
                        auto info = json.get<matjson::Value>(0);
                        auto position = info.get<int>("position");
                        positionLabel->setString(fmt::format("#{}", position).c_str());
                        positionLabel->setScale(getScaleBasedPos(position));
                        positionLabel->setVisible(true);
                        demonSpr->setVisible(true);
                        set(levelID, position);
                        log::info("Level found in Pointercrate! {} at #{}", level->m_levelName.c_str(), position);
                        this->release();
                    }
                })
                .expect([this, loading_circle](std::string const& error) {
                    if (loading_circle != nullptr) {
                        loading_circle->fadeAndRemove();
                    }
                    log::error("Error while sending a request to Pointercrate: {}", error);
                    FLAlertLayer::create(nullptr,
                        "Error",
                        "Failed to make a request to <cy>Pointercrate</c>. Please either <cg>try again later</c>, look at the error logs to see what might have happened, or report this to the developers.",
                        "OK",
                        nullptr,
                        350.0F
                    )->show();
                    this->release();
                });
        }
        
        return true;
    }
};
// demon list
