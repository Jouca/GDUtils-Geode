#include "EventsPush.h"
#include "../Settings/CustomSettings.hpp"
#include "DailyChest.h"
#include "Geode/utils/general.hpp"
#include <queue>
#include <Geode/utils/web.hpp>
#include <Geode/modify/RewardsPage.hpp>

static std::unordered_map<std::string, web::WebTask> RUNNING_REQUESTS {};
static std::mutex lock_var;
const std::lock_guard<std::mutex> lock(lock_var);

std::vector<std::string> EventsPush::getWords(std::string s, std::string delim) {
    std::vector<std::string> res;
    std::string token = "";
    for (int i = 0; i < s.size(); i++) {
        bool flag = true;
        for (int j = 0; j < delim.size(); j++) {
            if (s[i + j] != delim[j]) flag = false;
        }
        if (flag) {
            if (token.size() > 0) {
                res.push_back(token);
                token = "";
                i += delim.size() - 1;
            }
        } else {
            token += s[i];
        }
    }
    res.push_back(token);
    return res;
}

std::queue<EventData> eventQueue;
bool processingEvents = false;
int zOrder = 200;
EventsPush* EventsPush::create(EventData data) {
    auto ret = new EventsPush();
    if (ret && ret->init(data)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

const char* getDemonDifficultyIcon(int stars) {
    switch(stars){
        case 3: 
            return "difficulty_07_btn_001.png";
        case 4: 
            return "difficulty_08_btn_001.png";
        case 5: 
            return "difficulty_09_btn_001.png";
        case 6: 
            return "difficulty_10_btn_001.png";
        default:
            return "difficulty_06_btn_001.png";
    }
}
 
const char* getDifficultyIcon(int stars) {
    switch(stars){
        case 10:
            return "difficulty_01_btn_001.png";
        case 20:
            return "difficulty_02_btn_001.png";
        case 30:
            return "difficulty_03_btn_001.png";
        case 40:
            return "difficulty_04_btn_001.png";
        case 50:
            return "difficulty_05_btn_001.png";
        case 60:
            return "difficulty_auto_btn_001.png";
        default:
            return "difficulty_00_btn_001.png";
    }
}

void EventsPush::destroySelf() {
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    auto layer = reinterpret_cast<CCLayer*>(scene->getChildByTag(1932));
    if (layer != nullptr) {
        scene->removeChildByTag(1932);
    }
    EventsPush::eventCompletedCallback(scene);
}
// stole from GDR mod lol 
GJDifficulty getDifficulty(int stars) {
    switch(stars){
        case 0:
            return GJDifficulty::NA;
        case 10:
            return GJDifficulty::Easy;
        case 20:
            return GJDifficulty::Normal;
        case 30:
            return GJDifficulty::Hard;
        case 40:
            return GJDifficulty::Harder;
        case 50:
            return GJDifficulty::Insane;
        case 60:
            return GJDifficulty::Auto;
        default:
            return GJDifficulty::NA;
    }
}

GJDifficulty getDemonDifficulty(int stars) {
    switch(stars){
        case 3: 
            return GJDifficulty::DemonEasy;
        case 4: 
            return GJDifficulty::DemonMedium;
        case 5: 
            return GJDifficulty::DemonInsane;
        case 6: 
            return GJDifficulty::DemonExtreme;
        default:
            return GJDifficulty::Demon;
    }
}

std::vector<std::string> split_str(std::string &string, char separator) {
    std::stringstream segmentstream(string);
    std::string segmented;
    std::vector<std::string> splitlist;

    while (std::getline(segmentstream, segmented, separator)) {
        splitlist.push_back(segmented);
    }

    return splitlist;
};

GJGameLevel* EventsPush::convertLevelToJSON(std::string& data) {
    // Robtop to JSON
    GJGameLevel* level = GJGameLevel::create();
#ifdef GEODE_IS_WINDOWS
    auto gamelevelmanager = GameLevelManager::sharedState();
    auto gamestatslevel = GameStatsManager::sharedState();

    std::string auto_level = "0";
    std::string demon = "0";
    auto datas_list = split_str(data, '#');
    std::string level_data = datas_list[0];
    std::string creator_data = datas_list[1];

    // Level data
    auto level_data_list = split_str(level_data, ':');
    if (level_data_list[21] != "") {
        demon = level_data_list[21]; //
    } else {
        demon = "0";
    }
    if (level_data_list[25] != "") {
        auto_level = level_data_list[25]; //
    } else {
        auto_level = "0";
    }

    level->m_levelID = numFromString<int>(level_data_list[1]).unwrapOrDefault();

    level->m_levelName = level_data_list[3];
    level->m_levelDesc = level_data_list[35];
    level->m_levelVersion = numFromString<int>(level_data_list[5]).unwrapOrDefault();
    level->m_userID = numFromString<int>(level_data_list[7]).unwrapOrDefault();

    level->m_ratings = numFromString<int>(level_data_list[9]).unwrapOrDefault();
    level->m_ratingsSum = numFromString<int>(level_data_list[11]).unwrapOrDefault();

    if (level_data_list[9] == "10") {
        if (demon == "1") {
            std::string levelDemonDifficultyStr = level_data_list[23];
            level->m_difficulty = getDemonDifficulty(numFromString<int>(levelDemonDifficultyStr).unwrapOrDefault());
        } else {
            if (auto_level == "1") {
                level->m_difficulty = getDifficulty(0);
            } else {
                level->m_difficulty = getDifficulty(numFromString<int>(level_data_list[11]).unwrapOrDefault());
            }
        }
    }
    level->m_downloads = numFromString<int>(level_data_list[13]).unwrapOrDefault();
    if (level_data_list[53] == "0") {
        level->m_audioTrack = numFromString<int>(level_data_list[15]).unwrapOrDefault();
    } else {
        level->m_songID = numFromString<int>(level_data_list[53]).unwrapOrDefault();
    }

    std::string levelGameVersionStr = level_data_list[17];
    level->m_gameVersion = numFromString<int>(levelGameVersionStr).unwrapOrDefault();
    int levelLikes = numFromString<int>(level_data_list[19]).unwrapOrDefault();
    if (levelLikes >= 0) {
        level->m_likes = levelLikes;
        level->m_dislikes = 0;
    } else {
        level->m_likes = 0;
        level->m_dislikes = 0 - levelLikes;
    }
    level->m_levelLength = numFromString<int>(level_data_list[37]).unwrapOrDefault();
    
    level->m_demon = numFromString<int>(demon).unwrapOrDefault();
    level->m_stars = numFromString<int>(level_data_list[27]).unwrapOrDefault();
    
    if (auto_level == "1") {
        level->m_autoLevel = true;
    } else {
        level->m_autoLevel = false;
    }
    
    level->m_originalLevel = numFromString<int>(level_data_list[39]).unwrapOrDefault();
    if (level_data_list[41] == "1") {
        level->m_twoPlayerMode = true;
    } else {
        level->m_twoPlayerMode = false;
    }

    level->m_coins = numFromString<int>(level_data_list[43]).unwrapOrDefault();
    level->m_coinsVerified = numFromString<int>(level_data_list[45]).unwrapOrDefault();
    level->m_starsRequested = numFromString<int>(level_data_list[47]).unwrapOrDefault();
    level->m_featured = numFromString<int>(level_data_list[29]).unwrapOrDefault();
    if (level_data_list[31] == "1") {
        level->m_isEpic = true;
    } else {
        level->m_isEpic = false;
    }
    level->m_demonDifficulty = numFromString<int>(level_data_list[23]).unwrapOrDefault();

    level->m_objectCount = numFromString<int>(level_data_list[33]).unwrapOrDefault();

    level->m_levelNotDownloaded = true;

    level->m_workingTime = numFromString<int>(level_data_list[49]).unwrapOrDefault();

    CCArray* levelids = gamelevelmanager->m_onlineLevels->allKeys();
    CCObject* obj;
    CCARRAY_FOREACH(levelids, obj) {
        std::string levelid = (static_cast<CCString*>(obj))->getCString();
        if (numFromString<int>(levelid).unwrapOrDefault() == level->m_levelID) {
            auto levelFromSaved = static_cast<GJGameLevel*>(gamelevelmanager->m_onlineLevels->objectForKey(levelid));
            level->m_normalPercent = levelFromSaved->m_normalPercent;
            level->m_newNormalPercent2 = levelFromSaved->m_newNormalPercent2;
            level->m_practicePercent = levelFromSaved->m_practicePercent;
            level->m_levelFavorited = levelFromSaved->m_levelFavorited;
            level->m_levelFolder = levelFromSaved->m_levelFolder;
            level->m_attempts = levelFromSaved->m_attempts;
            level->m_jumps = levelFromSaved->m_jumps;
            level->m_clicks = levelFromSaved->m_clicks;
            level->m_attemptTime = levelFromSaved->m_attemptTime;

            level->m_levelNotDownloaded = levelFromSaved->m_levelNotDownloaded;
            level->m_chk = levelFromSaved->m_chk;
            level->m_isChkValid = levelFromSaved->m_isChkValid;
            level->m_isCompletionLegitimate = levelFromSaved->m_isCompletionLegitimate;
            level->m_isVerified = levelFromSaved->m_isVerified;
            level->m_isUploaded = levelFromSaved->m_isUploaded;
        }
    }

    // Creator data
    auto creator_data_list = split_str(creator_data, ':');
    level->m_creatorName = creator_data_list[1];
    level->m_accountID = numFromString<int>(creator_data_list[2]).unwrapOrDefault();
#endif
    return level;
}

void EventsPush::onClickBtn(CCObject* ret) {
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    auto layer = scene->getChildren()->objectAtIndex(0);
    if (layer == nullptr) return;
    auto events_layer = reinterpret_cast<EventsPush*>(scene->getChildByTag(1932));
    if (events_layer == nullptr) return;
    std::string layerName = typeid(*layer).name() + 6;
    if (layerName != "PlayLayer" && layerName != "PauseLayer" && layerName != "LevelEditorLayer") { // redirect to level
        if (eventType == EventType::Rate) {
            if (events_layer->level->m_levelID == 0) return;
            
            std::string const& url = "http://www.boomlings.com/database/getGJLevels21.php";
            #ifndef GEODE_IS_MACOS
            int level_id = events_layer->level->m_levelID.value();
            #else // mac os 
            int level_id = events_layer->levelId;
            #endif
            std::string const& fields = "secret=Wmfd2893gb7&type=0&str=" + std::to_string(level_id);

            geode::utils::web::WebRequest request = web::WebRequest();
            RUNNING_REQUESTS.emplace(
                "@loaderEventRateNotification",
                request.bodyString(fields).post(url).map(
                    [](web::WebResponse* response) {
                        if (response->ok()) {
                            if (response->data().empty()) {
                                FLAlertLayer::create(nullptr,
                                    "Error",
                                    "An server error happened.",
                                    "OK",
                                    nullptr,
                                    180.0F
                                )->show();
                            } else {
                                auto data_result = response->string().unwrapOrDefault();
                                if (data_result != "-1") {
                                    auto scene = CCScene::create();
                                    auto layer = LevelInfoLayer::create(EventsPush::convertLevelToJSON(data_result), false);
                                    layer->downloadLevel();
                                    scene->addChild(layer);
                                    CCDirector::sharedDirector()->pushScene(cocos2d::CCTransitionFade::create(0.5f, scene));
                                } else {
                                    log::info("Level not found. (-1)");
                                }
                            }
                        } else {
                            FLAlertLayer::create(nullptr,
                                "Error",
                                "An server error happened.",
                                "OK",
                                nullptr,
                                180.0F
                            )->show();
                        }

                        RUNNING_REQUESTS.erase("@loaderEventRateNotification");
                        return *response;
                    }
                )
            );
        } else if (eventType == EventType::Daily) {
            DailyLevelPage::create(GJTimedLevelType::Daily)->show();
            return;
        } else if (eventType == EventType::Weekly) {
            DailyLevelPage::create(GJTimedLevelType::Weekly)->show();
            return;
        } else if (eventType == EventType::Event) {
            DailyLevelPage::create(GJTimedLevelType::Event)->show();
            return;
        } else if (eventType == EventType::List) {
            if (events_layer->level->m_levelID == 0) return;

            GJSearchObject* search_obj = GJSearchObject::create(SearchType::Search, std::to_string(events_layer->level->m_levelID));
            search_obj->m_searchMode = 1;
            LevelBrowserLayer::create(search_obj)->show();
            return;
        } /*else if (eventType == EventType::smallChest) {
            RewardsPage::create()->show();
            return;
        }*/
    } else { // copy to clipboard
        #ifndef GEODE_IS_MACOS
        clipboard::write(std::to_string(events_layer->level->m_levelID));
        #else
        clipboard::write(std::to_string(events_layer->levelId));
        #endif
    }
}

bool EventsPush::init(EventData data) {
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    // type
    // 0 = new rate
    // 1 = daily
    // 2 = weekly

    // rate
    // 0 = unrated / star rate
    // 1 = featured
    // 2 = epic
    // {'demon': 0, 'type': 0, 'starsum': 20, 'stars': 2, 'rate': 2, 'title': 'New Rated Level !', 'level_name': 'Pourquoi', 'level_creator': 'by Jouca', 'sprite': 'GJ_square01.png'}
    int starsum = data.starsum;
    int stars = data.stars;
    int rateType = data.rate;
    int coins = data.coins;
    int areCoinsVerified = data.verified_coins;
    int level_id = 0;
    if (data.level_id != -1) {
        level_id = data.level_id;
        level->m_levelName = data.level_name;
        level->m_coins = coins;
        level->m_coinsVerified = areCoinsVerified;
        level->m_levelID = level_id;
        #ifdef GEODE_IS_MACOS
        this->levelId = level_id;
        #endif
    }
    
    
    auto menu = CCMenu::create();

    bool newRate = Mod::get()->template getSettingValue<bool>("newRate");
    bool daily = Mod::get()->template getSettingValue<bool>("daily");
    bool weekly = Mod::get()->template getSettingValue<bool>("weekly");
    bool event = Mod::get()->template getSettingValue<bool>("event");
    bool smallChest = Mod::get()->template getSettingValue<bool>("smallChest");
    bool largeChest = Mod::get()->template getSettingValue<bool>("largeChest");
    bool list = Mod::get()->template getSettingValue<bool>("newListRate");
    bool announcement = Mod::get()->template getSettingValue<bool>("announcement");
    eventType = data.type;
    if (eventType == EventType::Rate && !newRate) {
        EventsPush::eventCompletedCallback(scene);
        return true;
    }
    if (eventType == EventType::Daily && !daily) {
        EventsPush::eventCompletedCallback(scene);
        return true;
    }
    if (eventType == EventType::Weekly && !weekly) {
        EventsPush::eventCompletedCallback(scene);
        return true;
    }
    if (eventType == EventType::smallChest && !smallChest) {
        EventsPush::eventCompletedCallback(scene);
        return true;
    }
    if (eventType == EventType::largeChest && !largeChest) {
        EventsPush::eventCompletedCallback(scene);
        return true;
    }
    if (eventType == EventType::List && !list) {
        EventsPush::eventCompletedCallback(scene);
        return true;
    }
    if (eventType == EventType::Event && !event) {
        EventsPush::eventCompletedCallback(scene);
        return true;
    }
    if (eventType == EventType::Announcement && !announcement) {
        EventsPush::eventCompletedCallback(scene);
        return true;
    }
    auto director = CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
    auto bg = cocos2d::extension::CCScale9Sprite::create(data.sprite.c_str(), { .0f, .0f, 80.0f, 80.0f, });
    auto bg_click_spr = cocos2d::extension::CCScale9Sprite::create(data.sprite.c_str(), { .0f, .0f, 80.0f, 80.0f, });
    float lrScale = (float)Mod::get()->template getSettingValue<double>("size");
    auto LrSize = CCSize{ 240, 70 }; // * lrScale for both
    bg->setScale(lrScale);
    this->setZOrder(zOrder);
    zOrder = zOrder + 10;
    bg->setContentSize(LrSize);
    bg_click_spr->setContentSize(LrSize);
    // 0 / 0 = bottom left
    // screen width / screen height = top right
    // screen width divided by 2 / screen height divided by 2 = middle

    // TODO : click to notifications to be redirected to specific content
    auto layer = scene->getChildren()->objectAtIndex(0);
    std::string layerName = typeid(*layer).name() + 6;
    if (layerName != "PlayLayer" && layerName != "PauseLayer" && layerName != "LevelEditorLayer") {
        auto bg_btn = CCMenuItemSpriteExtra::create(bg_click_spr, this, menu_selector(EventsPush::onClickBtn));
        menu->addChild(bg_btn);
    }
    
    menu->setPosition({ bg->getContentSize().width / 2, bg->getContentSize().height / 2 });
    bg->addChild(menu);
    this->addChild(bg);

    auto node = CCNode::create();

    if (eventType != EventType::smallChest && eventType != EventType::largeChest && eventType != EventType::Announcement) {
        CCSprite* diffFace;
        GJDifficultySprite* mythic = nullptr;
        if (!data.demon) {
            diffFace = cocos2d::CCSprite::createWithSpriteFrameName(getDifficultyIcon(starsum));
            mythic = GJDifficultySprite::create(static_cast<int>(getDifficulty(starsum)), static_cast<GJDifficultyName>(0));
        } else {
            diffFace = cocos2d::CCSprite::createWithSpriteFrameName(getDemonDifficultyIcon(starsum));
            mythic = GJDifficultySprite::create(static_cast<int>(getDemonDifficulty(starsum)), static_cast<GJDifficultyName>(0));
        }
        
        diffFace->setPosition({26.f, 43.f});
        diffFace->setScale(.8F);
        CCSprite* moon = cocos2d::CCSprite::createWithSpriteFrameName("moon_small01_001.png");
        CCSprite* featured = cocos2d::CCSprite::createWithSpriteFrameName("GJ_featuredCoin_001.png");
        CCSprite* epic = cocos2d::CCSprite::createWithSpriteFrameName("GJ_epicCoin_001.png");
        CCSprite* legendary = cocos2d::CCSprite::createWithSpriteFrameName("GJ_epicCoin2_001.png");
        mythic->updateFeatureState(static_cast<GJFeatureState>(4));

        std::string starcountstr = std::to_string(stars);
        auto starcount = cocos2d::CCLabelBMFont::create(starcountstr.c_str(), "bigFont.fnt");
        starcount->setPosition({25, 18});
        starcount->setAnchorPoint({1, 0.5});
        starcount->setScale(0.35f);
        bg->addChild(starcount);
        CCSprite* star;
        if (eventType == EventType::List) {
            star = cocos2d::CCSprite::createWithSpriteFrameName("diamond_small01_001.png");
        } else {
            star = cocos2d::CCSprite::createWithSpriteFrameName("star_small01_001.png");
        }
        star->setPosition({31, 18});
        star->setScale(.775F);
        moon->setPosition({31, 18});
        moon->setScale(.775F);

        if (data.platformer) bg->addChild(moon);
        else bg->addChild(star);

        featured->setPosition({26.f, 43.f});
        epic->setPosition({26.f, 43.f});
        legendary->setPosition({26.f, 43.f});
        mythic->setPosition({26.f, 43.f});
        featured->setScale(.8F);
        epic->setScale(.8F);
        legendary->setScale(.8F);
        mythic->setScale(.8F);

        switch (rateType) {
            default: // Rate
                bg->addChild(diffFace);
                break;
            case 1: // Featured
                bg->addChild(featured);
                bg->addChild(diffFace);
                break;
            case 2: // Epic
                bg->addChild(epic);
                bg->addChild(diffFace);
                break;
            case 3: // Legendary
                bg->addChild(legendary);
                bg->addChild(diffFace);

                if (Mod::get()->template getSettingValue<bool>("customDifficultyFaces")) {
                    CCSprite* legendaryFace = nullptr;
                    if (!data.demon) {
                        if (starsum >= 10) {
                            std::string diffStr = std::to_string(static_cast<int>(getDifficulty(starsum)));
                            auto name = "difficulty_0" + diffStr + "_legendaryIcon.png";
                            legendaryFace = CCSprite::create((""_spr+name).c_str());
                        }
                    } else {
                        if (starsum < 6) {
                            std::string diffStr = std::to_string(static_cast<int>(getDemonDifficulty(starsum)));
                            auto name = "difficulty_0" + diffStr + "_legendaryIcon.png";
                            legendaryFace = CCSprite::create((""_spr+name).c_str());
                        } else {
                            std::string diffStr = std::to_string(static_cast<int>(getDemonDifficulty(starsum)));
                            auto name = "difficulty_" + diffStr + "_legendaryIcon.png";
                            legendaryFace = CCSprite::create((""_spr+name).c_str());
                        }
                    }

                    if (legendaryFace != nullptr) {
                        legendaryFace->setPosition({26.f, 48.f});
                        legendaryFace->setScale(.8F);
                        bg->addChild(legendaryFace);
                    }
                }

                break;
            case 4: // Mythic
                bg->addChild(mythic);

                if (Mod::get()->template getSettingValue<bool>("customDifficultyFaces")) {
                    CCSprite* mythicFace = nullptr;
                    if (!data.demon) {
                        if (starsum >= 10) {
                            std::string diffStr = std::to_string(static_cast<int>(getDifficulty(starsum)));
                            auto name = "difficulty_0" + diffStr + "_mythicIcon.png";
                            mythicFace = CCSprite::create((""_spr+name).c_str());
                        }
                    } else {
                        if (starsum < 6) {
                            std::string diffStr = std::to_string(static_cast<int>(getDemonDifficulty(starsum)));
                            auto name = "difficulty_0" + diffStr + "_mythicIcon.png";
                            mythicFace = CCSprite::create((""_spr+name).c_str());
                        } else {
                            std::string diffStr = std::to_string(static_cast<int>(getDemonDifficulty(starsum)));
                            auto name = "difficulty_" + diffStr + "_mythicIcon.png";
                            mythicFace = CCSprite::create((""_spr+name).c_str());
                        }
                    }

                    if (mythicFace != nullptr) {
                        mythicFace->setPosition({26.f, 48.f});
                        mythicFace->setScale(.8F);
                        bg->addChild(mythicFace);
                    }
                }

                break;
        }

        auto verifiedCoinSpr1 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_coinsIcon2_001.png");
        verifiedCoinSpr1->setScale(0.35f);
        auto verifiedCoinSpr2 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_coinsIcon2_001.png");
        verifiedCoinSpr2->setScale(0.35f);
        auto verifiedCoinSpr3 = cocos2d::CCSprite::createWithSpriteFrameName("GJ_coinsIcon2_001.png");
        verifiedCoinSpr3->setScale(0.35f);

        if (areCoinsVerified == 0) {
            verifiedCoinSpr1->setColor({255, 175, 75});
            verifiedCoinSpr2->setColor({255, 175, 75});
            verifiedCoinSpr3->setColor({255, 175, 75});
        }

        if (coins == 1 || coins == 3) {
            verifiedCoinSpr2->setPosition({ -52, -22 });
            node->addChild(verifiedCoinSpr2);

            if (coins == 3) {
                verifiedCoinSpr1->setPosition({ -60, -22 });
                verifiedCoinSpr3->setPosition({ -44, -22 });

                node->addChild(verifiedCoinSpr1);
                node->addChild(verifiedCoinSpr3);
            }
        } else if (coins == 2) {
            verifiedCoinSpr1->setPosition({ -55, -22 });
            verifiedCoinSpr2->setPosition({ -48, -22 });

            node->addChild(verifiedCoinSpr1);
            node->addChild(verifiedCoinSpr2);
        }

        // Progress Bar for lists
        if (eventType == EventType::List) {
            auto barSpriteBack = CCSprite::create("GJ_progressBar_001.png");
            barSpriteBack->setScaleX(0.38f);
            barSpriteBack->setScaleY(0.45f);
            barSpriteBack->setPosition({ 38, 3 });
            barSpriteBack->setPositionY(3 - 19.f);
            barSpriteBack->setPositionX(38);
            barSpriteBack->setColor({0, 0, 0});
            barSpriteBack->setOpacity(100);
            barSpriteBack->setZOrder(10);
            barSpriteBack->setID("progress-bar-list"_spr);
            node->addChild(barSpriteBack);

            auto barSpriteTop = CCSprite::create("GJ_progressBar_001.png");
            barSpriteTop->setScaleY(0.4f);
            barSpriteTop->setPosition({ 2, 10 });
            barSpriteTop->setAnchorPoint({0, 0.5});
            barSpriteTop->setOpacity(255);
            barSpriteTop->setZOrder(11);

            CCPoint rectangle[4] = {
                CCPoint(0, 0),
                CCPoint(0, 20),
                CCPoint(barSpriteBack->getScaledContentSize().width * -2.5f, 20),
                CCPoint(barSpriteBack->getScaledContentSize().width * -2.5f, 0)
            };

            auto clippingNode = CCClippingNode::create();
            auto barMask = CCDrawNode::create();
            barMask->drawPolygon(rectangle, 4, ccc4FFromccc3B({0, 0, 0}), 0, ccc4FFromccc3B({0, 0, 0}));
            clippingNode->setStencil(barMask);
            clippingNode->addChild(barSpriteTop);
            clippingNode->setPositionX(330);
            barSpriteBack->addChild(clippingNode);
            barSpriteTop->setPositionX(-666);

            // Get datas for levels
            int maxToCompleteList = data.maxToCompleteList;
            std::vector<std::string> levels = split_str(data.levels_list, ',');

            // Get client data for levels
            GameLevelManager* gameLevelManager = GameLevelManager::sharedState();
            CCArray* levelsData = gameLevelManager->getCompletedLevels(false);

            int completedLevels = 0;
            int nbLevels = 0;

            while (levels.size() > 0) {
                std::string id = levels[0];
                nbLevels++;

                for (int i = 0; i < levelsData->count(); i++) {
                    auto level = static_cast<GJGameLevel*>(levelsData->objectAtIndex(i));
                    if (std::to_string(level->m_levelID) == id) {
                        if (level->m_normalPercent == 100) completedLevels++;
                    }
                }

                levels.erase(levels.begin());
            }

            // min -666 | max -337
            // Calculate position for Progress Bar (between these values based on percentage)
            float percentage;
            CCLabelBMFont* progressText;

            if (completedLevels >= maxToCompleteList) {
                percentage = (float)completedLevels / (float)nbLevels;
                progressText = CCLabelBMFont::create((std::to_string(completedLevels) + "/" + std::to_string(nbLevels)).c_str(), "bigFont.fnt");
                barSpriteTop->setColor({ 100, 255, 255 });
                starcount->setColor({ 100, 255, 255 });

                auto listCompleted = CCSprite::createWithSpriteFrameName("GJ_completesIcon_001.png");
                listCompleted->setPosition({ 94, -15 });
                listCompleted->setScale(0.55f);
                listCompleted->setZOrder(13);
                node->addChild(listCompleted);
            } else {
                percentage = (float)completedLevels / (float)maxToCompleteList;
                progressText = CCLabelBMFont::create((std::to_string(completedLevels) + "/" + std::to_string(maxToCompleteList)).c_str(), "bigFont.fnt");
                barSpriteTop->setColor({ 255, 84, 50 });

                auto listUncompleted = CCSprite::createWithSpriteFrameName("diamond_small01_001.png");
                listUncompleted->setPosition({ 94, -15 });
                listUncompleted->setScale(0.55f);
                listUncompleted->setZOrder(13);
                node->addChild(listUncompleted);
            }
            
            auto moveAction = CCEaseSineOut::create(CCMoveBy::create(0.f, { (percentage * 337), 0 }));
            barSpriteTop->runAction(moveAction);

            // Add text for Progress Bar
            progressText->setPosition({ 38, -15 });
            progressText->setScale(0.35f);
            progressText->setZOrder(13);
            node->addChild(progressText);
        }
    }
    
    auto title = cocos2d::CCLabelBMFont::create(data.title.c_str(), "goldFont.fnt");
    if (eventType == EventType::smallChest) {
        title->setPosition({ -65, 26 });
    } else if (eventType == EventType::largeChest) {
        title->setPosition({ -54, 26 });
    } else if (eventType == EventType::List) {
        title->setPosition({ -27, 27 });
    } else if (eventType == EventType::Announcement) {
        title->setString("Announcement");
        title->setPosition({ -65, 26 });
        auto subtitle = cocos2d::CCLabelBMFont::create(data.title.c_str(), "bigFont.fnt");
        subtitle->limitLabelWidth(200, 0.46f, 0.1f);
        subtitle->setAnchorPoint({ 0, 0.5 });
        subtitle->setPosition({ -65, 10 });
        node->addChild(subtitle);
    } else {
        title->setPosition({ -27, 23 });
    }
    title->setScale(.575F);
    title->setAnchorPoint({ 0, 0.5 });
    node->addChild(title);
    if (eventType != EventType::Announcement) {
        auto level_title = cocos2d::CCLabelBMFont::create(data.level_name.c_str(), "bigFont.fnt");
        if (eventType == EventType::List) {
            level_title->setPosition({ -27, 11 });
        } else {
            level_title->setPosition({ -27, 3 });
        }
        level_title->setScale(.46F);
        
        level_title->setAnchorPoint({ 0, 0.5 });

        auto level_by = cocos2d::CCLabelBMFont::create(data.level_creator.c_str(), "goldFont.fnt");
        if (eventType == EventType::List) {
            level_by->setPosition({ -27, -2 });
        } else {
            level_by->setPosition({ -27, -11 });
        }
        level_by->setScale(.46F);
        level_by->limitLabelWidth(120, 0.46f, 0.1f);
        level_by->setAnchorPoint({ 0, 0.5 });
        node->addChild(level_by);
        level_title->limitLabelWidth(120, 0.46f, 0.1f);
        node->addChild(level_title);
    }

    if (eventType == EventType::Daily || eventType == EventType::Weekly || eventType == EventType::Event) {
        CCSprite* crown;
        if (eventType == EventType::Daily) {
            crown = cocos2d::CCSprite::createWithSpriteFrameName("gj_dailyCrown_001.png");
        } else if (eventType == EventType::Event) {
            crown = cocos2d::CCSprite::createWithSpriteFrameName("gj_eventCrown_001.png");
        } else {
            crown = cocos2d::CCSprite::createWithSpriteFrameName("gj_weeklyCrown_001.png");
        }
        crown->setScale(.45F);
        crown->setPosition({ 45, 45 });
        node->addChild(crown);
    } else if (eventType == EventType::smallChest) {
        auto chest = cocos2d::CCSprite::createWithSpriteFrameName("chest_01_02_001.png");
        chest->setScale(.45F);
        chest->setPosition({ 45, -3 });
        node->addChild(chest);
    } else if (eventType == EventType::largeChest) {
        auto chest = cocos2d::CCSprite::createWithSpriteFrameName("chest_02_02_001.png");
        chest->setScale(.4F);
        chest->setPosition({ 45, -3 });
        node->addChild(chest);

    } else if (eventType == EventType::List) {
        auto list = cocos2d::CCSprite::createWithSpriteFrameName("GJ_listAddBtn_001.png");
        list->setPosition({ 139, -8 });
        node->addChild(list);
    }

    node->setAnchorPoint({ 0.5, 0.5 });
    node->setPosition({ 77, 30 });

    bg->addChild(node);

    this->setTag(1932);

    // Move action
    //CCDelayTime
    
    float delayTime = (float)Mod::get()->template getSettingValue<double>("time");

    auto cornerId = Mod::get()->getSettingValue<SettingPosEnum>("notificationPlacement");
    float moveX = .0F;
    switch (cornerId) {
        case SettingPosEnum::TopLeft: // top left
            bg->setPosition((-(bg->getContentSize().width / 2)) * lrScale, ((winSize.height - (bg->getContentSize().height / 2)) - (20 * lrScale)));
            moveX = (bg->getContentSize().width) * lrScale;
            break;
        case SettingPosEnum::TopRight: // top right
            bg->setPosition(winSize.width + ((bg->getContentSize().width / 2) * lrScale), ((winSize.height - (bg->getContentSize().height / 2)) - (20 * lrScale)));
            moveX = -(bg->getContentSize().width * lrScale);
            break;
        case SettingPosEnum::BottomLeft: // bottom left
            bg->setPosition((-(bg->getContentSize().width / 2)) * lrScale, (bg->getContentSize().height / 2) * lrScale);
            moveX = (bg->getContentSize().width) * lrScale;
            break;
        case SettingPosEnum::BottomRight: // bottom right
            bg->setPosition({ winSize.width + ((bg->getContentSize().width / 2) * lrScale), (bg->getContentSize().height / 2) * lrScale});
            moveX = -((bg->getContentSize().width) * lrScale);
            break;
    }
    
    bg->runAction(CCSequence::create(
        CCEaseOut::create(CCMoveBy::create(0.5f, { moveX, 0.0f }), 0.6f),
        CCDelayTime::create(delayTime),
        CCEaseIn::create(CCMoveBy::create(0.5f, { -moveX, 0.0f }), 0.6f),
        CCDelayTime::create(0.5F),
        CCCallFunc::create(this, callfunc_selector(EventsPush::destroySelf)),
        nullptr
    ));

    float volume = 0.0f;
    if (Mod::get()->template getSettingValue<bool>("sfx-link")) {
        volume = GameManager::get()->m_sfxVolume;
    } else {
        volume = Mod::get()->template getSettingValue<double>("sfx-volume");
    }
    
    if (Mod::get()->template getSettingValue<bool>("sfx") && (eventType != EventType::smallChest && eventType != EventType::largeChest)) FMODAudioEngine::sharedEngine()->playEffect("crystal01.ogg", 1, 1, volume);
    if (Mod::get()->template getSettingValue<bool>("sfx") && (eventType == EventType::smallChest || eventType == EventType::largeChest)) FMODAudioEngine::sharedEngine()->playEffect("reward01.ogg", 1, 1, volume);
    return true;
}

void EventsPush::stopNow(CCScene* scene) {
    EventsPush::eventCompletedCallback(scene);
}

void EventsPush::pushRateLevel(CCScene* self, std::string data) {
    EventData eventData = matjson::Serialize<EventData>::fromJson(matjson::parse(data).unwrapOrDefault()).unwrapOrDefault();
    /*log::info(
        "EventData [\ndemon = {};\nstarsum = {};\nstars = {};\nrate = {};\ntype = {};\ntitle = {};\nsprite = {};\nlevel_name = {};\nlevel_creator = {};\ncoins = {};\nverified_coins = {};\nplatformer = {};\nlevel_id = {};\nlevels_list = {};\nmaxToCompleteList = {};\n]",
        eventData.demon,
        eventData.starsum,
        eventData.stars,
        eventData.rate,
        GDUtils::Events::RateEvent::typeToInt(eventData.type),
        eventData.title,
        eventData.sprite,
        eventData.level_name,
        eventData.level_creator,
        eventData.coins,
        eventData.verified_coins,
        eventData.platformer,
        eventData.level_id,
        eventData.levels_list,
        eventData.maxToCompleteList
    );*/
    pushRateLevel(self, eventData);
}

void EventsPush::pushRateLevel(CCScene* self, EventData data) {
    // Enqueue the event
    eventQueue.push(data);
    
    // If no event is being processed, start processing events
    if (!processingEvents) {
        EventsPush::processNextEvent(self);
    }
}

void EventsPush::processNextEvent(CCScene* self) {
    if (!eventQueue.empty()) {
        auto data = eventQueue.front();
        eventQueue.pop();
        processingEvents = true;
        
        auto layer = EventsPush::create(data);
        GDUtils::Events::OnRate(data).post();
        // Set a callback function that will be called when the event is completed
        //layer->setEventCompletedCallback(std::bind(&EventsPush::eventCompletedCallback, this));
        if (layer && self) self->addChild(layer);
    }
}

void EventsPush::eventCompletedCallback(CCScene* self) {
    // Event is completed, set processingEvents to false
    processingEvents = false;
    
    // Process the next event (if any)
    EventsPush::processNextEvent(self);
}

$execute {
    new EventListener<EventFilter<GDUtils::Events::RateEvent>>(+[](GDUtils::Events::RateEvent* e) {
        if (auto scene = CCScene::get()) {
            EventsPush::pushRateLevel(scene, e->getData());
        }
        return ListenerResult::Stop;
    });
}
