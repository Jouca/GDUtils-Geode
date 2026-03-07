#include "EventsPush.hpp"
#include <defs/misc.hpp>
#include <modules/settings/Position.hpp>
#include <queue>
#include <network/AMQClient.hpp>

std::queue<EventData> g_eventQueue;
bool g_processingEvents = false;
std::optional<arc::mpsc::Sender<bool>> g_finishTx;
//extern std::optional<arc::mpsc::Sender<std::string>> s_tx;

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

bool EventsPush::init(EventData data) {
    if (!CCLayer::init()) return false;
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    auto director = CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
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
    std::unordered_map<EventType, std::string_view> sKeys {
        { EventType::Rate, "newRate" },
        { EventType::Daily, "daily" },
        { EventType::Weekly, "weekly" },
        { EventType::smallChest, "event" },
        { EventType::largeChest, "smallChest" },
        { EventType::List, "largeChest" },
        { EventType::Event, "newListRate" },
        { EventType::Announcement, "announcement" },
    };
    eventType = data.type;
    auto it = sKeys.find(data.type);
    if (it != sKeys.end()) {
        if (!Mod::get()->getSettingValue<bool>(it->second)) {
            EventsPush::eventCompletedCallback(scene);
            return true;
        }
    }
    float lrScale = (float)Mod::get()->template getSettingValue<double>("size");
    auto bg = Build<NineSlice>::create(data.sprite.c_str()).scale(lrScale).contentSize(240, 70).collect();
    // 0 / 0 = bottom left
    // screen width / screen height = top right
    // screen width divided by 2 / screen height divided by 2 = middle
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

        mythic->setPosition({26.f, 43.f});
        mythic->setScale(.8F);

        switch (rateType) {
            default: // Rate
                bg->addChild(diffFace);
                break;
            case 1: // Featured
                Build<CCSprite>::createSpriteName("GJ_featuredCoin_001.png").pos(26,43).scale(.8f).parent(bg);
                bg->addChild(diffFace);
                break;
            case 2: // Epic
                Build<CCSprite>::createSpriteName("GJ_epicCoin_001.png").pos(26,43).scale(.8f).parent(bg);
                bg->addChild(diffFace);
                break;
            case 3: // Legendary
                Build<CCSprite>::createSpriteName("GJ_epicCoin2_001.png").pos(26,43).scale(.8f).parent(bg);
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

        if (data.verified_coins == 0) {
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
            std::vector<std::string> levels = geode::utils::string::split(data.levels_list, ",");

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
    
    std::string titleStr = data.title;
    CCPoint titlePos;
    geode::Anchor titleAnchor = Anchor::Top;
    switch (data.type) {
        case EventType::smallChest:
            titlePos = CCPoint(0, -14);
            Build<CCSprite>::createSpriteName("chest_01_02_001.png").scale(0.45f).parentAtPos(bg, Anchor::Center, {0,-8});
            break;
        case EventType::largeChest:
            titlePos = CCPoint(0, -14);
            Build<CCSprite>::createSpriteName("chest_02_02_001.png").scale(0.4f).parentAtPos(bg, Anchor::Center, {0,-8});
            break;
        case EventType::List: // TEST
            titlePos = CCPoint(-11, -15);
            Build<CCSprite>::createSpriteName("GJ_listAddBtn_001.png").anchorPoint(1,0).parentAtPos(bg, Anchor::BottomLeft, {7,7});
            break;
        case EventType::Announcement: // TEST
            titleStr = "Announcement";
            titlePos = CCPoint(-56, -14);
            Build<CCLabelBMFont>::create(data.title.c_str(), "bigFont.fnt").limitLabelWidth(200, 0.46f, 0.1f).anchorPoint(0, 0.5).parentAtPos(bg, Anchor::Left, {12,5});
            break;
        default:
            titlePos = CCPoint(-6, -17);
            break;
    }
    Build<CCLabelBMFont>::create(titleStr.c_str(), "goldFont.fnt").scale(0.575F).parentAtPos(bg, titleAnchor, titlePos);

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
    }

    node->setAnchorPoint({ 0.5, 0.5 });
    node->setPosition({ 77, 30 });

    bg->addChild(node);

    // Move action
    
    float delayTime = (float)Mod::get()->template getSettingValue<double>("time");

    float moveX = .0F;
    switch (Mod::get()->getSettingValue<PositionEnum>("notificationPlacement")) {
        case PositionEnum::TopLeft: // top left
            bg->setPosition((-(bg->getContentSize().width / 2)) * lrScale, ((winSize.height - (bg->getContentSize().height / 2)) - (20 * lrScale)));
            moveX = (bg->getContentSize().width) * lrScale;
            break;
        case PositionEnum::TopRight: // top right
            bg->setPosition(winSize.width + ((bg->getContentSize().width / 2) * lrScale), ((winSize.height - (bg->getContentSize().height / 2)) - (20 * lrScale)));
            moveX = -(bg->getContentSize().width * lrScale);
            break;
        case PositionEnum::BottomLeft: // bottom left
            bg->setPosition((-(bg->getContentSize().width / 2)) * lrScale, (bg->getContentSize().height / 2) * lrScale);
            moveX = (bg->getContentSize().width) * lrScale;
            break;
        case PositionEnum::BottomRight: // bottom right
            bg->setPosition({ winSize.width + ((bg->getContentSize().width / 2) * lrScale), (bg->getContentSize().height / 2) * lrScale});
            moveX = -((bg->getContentSize().width) * lrScale);
            break;
    }

    if (Mod::get()->getSettingValue<bool>("sfx")) {
        float volume = (Mod::get()->getSettingValue<bool>("sfx-link")) ? GameManager::get()->m_sfxVolume : Mod::get()->getSettingValue<double>("sfx-volume");
        if (data.type != EventType::smallChest && data.type != EventType::largeChest) {
            FMODAudioEngine::get()->playEffect("crystal01.ogg", 1, 1, volume);
        } else {
            FMODAudioEngine::get()->playEffect("reward01.ogg", 1, 1, volume);
        }
    }

    bg->runAction(CCSequence::create(
        CCEaseOut::create(CCMoveBy::create(0.5f, { moveX, 0.0f }), 0.6f),
        CCDelayTime::create(delayTime),
        CCEaseIn::create(CCMoveBy::create(0.5f, { -moveX, 0.0f }), 0.6f),
        CCDelayTime::create(0.5F),
        CCCallFunc::create(this, callfunc_selector(EventsPush::destroySelf)),
        nullptr
    ));

    this->setZOrder(scene->getHighestChildZ() + 10);
    this->setID("EventsPush"_spr);
    return true;
}

bool EventsPush::initold(EventData data) {
    if (!CCLayer::init()) return false;
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    auto director = CCDirector::sharedDirector();
    auto winSize = director->getWinSize();
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
    int coins = data.coins;
    int areCoinsVerified = data.verified_coins;
    std::unordered_map<EventType, std::string_view> sKeys {
        { EventType::Rate, "newRate" },
        { EventType::Daily, "daily" },
        { EventType::Weekly, "weekly" },
        { EventType::smallChest, "event" },
        { EventType::largeChest, "smallChest" },
        { EventType::List, "largeChest" },
        { EventType::Event, "newListRate" },
        { EventType::Announcement, "announcement" },
    };
    eventType = data.type;
    auto it = sKeys.find(data.type);
    if (it != sKeys.end()) {
        if (!Mod::get()->getSettingValue<bool>(it->second)) {
            EventsPush::eventCompletedCallback(scene);
            return true;
        }
    }
    float lrScale = (float)Mod::get()->getSettingValue<double>("size");

    auto node = Build<CCNode>::create().anchorPoint(0.5, 0.5).pos(77, 30).collect();
    auto bg = Build<CCScale9Sprite>::create(data.sprite.c_str()).contentSize(240, 70).scale(lrScale).child(node).parent(this).collect();
    float moveX = 0.f;
    switch (Mod::get()->getSettingValue<PositionEnum>("notificationPlacement")) {
        case PositionEnum::TopLeft: // top left
            bg->setPosition((-(bg->getContentSize().width / 2)) * lrScale, ((winSize.height - (bg->getContentSize().height / 2)) - (20 * lrScale)));
            moveX = (bg->getContentSize().width) * lrScale;
            break;
        case PositionEnum::TopRight: // top right
            bg->setPosition(winSize.width + ((bg->getContentSize().width / 2) * lrScale), ((winSize.height - (bg->getContentSize().height / 2)) - (20 * lrScale)));
            moveX = -(bg->getContentSize().width * lrScale);
            break;
        case PositionEnum::BottomLeft: // bottom left
            bg->setPosition((-(bg->getContentSize().width / 2)) * lrScale, (bg->getContentSize().height / 2) * lrScale);
            moveX = (bg->getContentSize().width) * lrScale;
            break;
        case PositionEnum::BottomRight: // bottom right
            bg->setPosition({ winSize.width + ((bg->getContentSize().width / 2) * lrScale), (bg->getContentSize().height / 2) * lrScale});
            moveX = -((bg->getContentSize().width) * lrScale);
            break;
    }
    std::string titleStr = data.title;
    CCPoint titlePos;
    switch (data.type) {
        case EventType::smallChest:
            titlePos = CCPoint(-65, 26);
            Build<CCSprite>::createSpriteName("chest_01_02_001.png").scale(0.45f).pos(45, -3).parent(node);
            break;
        case EventType::largeChest:
            titlePos = CCPoint(-54, 26);
            Build<CCSprite>::createSpriteName("chest_02_02_001.png").scale(0.4f).pos(45, -3).parent(node);
            break;
        case EventType::List:
            titlePos = CCPoint(-27, 27);
            Build<CCSprite>::createSpriteName("GJ_listAddBtn_001.png").pos(139, -8).parent(node);
            break;
        case EventType::Announcement:
            titleStr = "Announcement";
            titlePos = CCPoint(-65, 26);
            Build<CCLabelBMFont>::create(data.title.c_str(), "bigFont.fnt").limitLabelWidth(200, 0.46f, 0.1f).anchorPoint(0, 0.5).pos(-65, 10).parent(node);
            break;
        default:
            titlePos = CCPoint(-27, 23);
            break;
    }
    if (data.type != EventType::smallChest && data.type != EventType::largeChest && data.type != EventType::Announcement) {
        if (data.platformer) {
            Build<CCSprite>::createSpriteName("moon_small01_001.png").scale(.775f).pos(31, 18).parent(bg);
        } else {
            Build<CCSprite>::createSpriteName((data.type == EventType::List) ? "diamond_small01_001.png" : "star_small01_001.png").scale(.775f).pos(31, 18).parent(bg);
        }
        if (data.type != EventType::List) {
        Build<CCSprite>::createSpriteName((!data.demon) ? getDifficultyIcon(starsum) : getDemonDifficultyIcon(starsum))
            .pos(26, 43)
            .scale(.8f)
            .parent(bg);
        GJDifficultySprite* mythic = GJDifficultySprite::create(
            static_cast<int>((!data.demon) ? getDifficulty(starsum) : getDemonDifficulty(starsum)),
            static_cast<GJDifficultyName>(0)
        );
        mythic->updateFeatureState(GJFeatureState::Mythic);
        switch (data.rate) {
            default:
                break;
        }

        CCSprite* featured = cocos2d::CCSprite::createWithSpriteFrameName("GJ_featuredCoin_001.png");
        CCSprite* epic = cocos2d::CCSprite::createWithSpriteFrameName("GJ_epicCoin_001.png");
        CCSprite* legendary = cocos2d::CCSprite::createWithSpriteFrameName("GJ_epicCoin2_001.png");
        mythic->updateFeatureState(static_cast<GJFeatureState>(4));

        Build<CCLabelBMFont>::create(std::to_string(stars).c_str(), "bigFont.fnt").pos(25, 18).anchorPoint(1, 0.5).scale(0.35f).parent(bg);

        featured->setPosition({26.f, 43.f});
        epic->setPosition({26.f, 43.f});
        legendary->setPosition({26.f, 43.f});
        mythic->setPosition({26.f, 43.f});
        featured->setScale(.8F);
        epic->setScale(.8F);
        legendary->setScale(.8F);
        mythic->setScale(.8F);

        switch (data.type) {
            default:
                break;
            case 1: // Featured
                bg->addChild(featured);
                break;
            case 2: // Epic
                bg->addChild(epic);
                break;
            case 3: // Legendary
                bg->addChild(legendary);

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
        } else {

        }
    } else if (data.type == EventType::List) {
    }
    Build<CCLabelBMFont>::create(titleStr.c_str(), "goldFont.fnt").scale(0.575F).pos(titlePos).anchorPoint(0, 0.5).parent(node);

    if (Mod::get()->getSettingValue<bool>("sfx")) {
        float volume = (Mod::get()->getSettingValue<bool>("sfx-link")) ? GameManager::get()->m_sfxVolume : Mod::get()->getSettingValue<double>("sfx-volume");
        if (data.type != EventType::smallChest && data.type != EventType::largeChest) {
            FMODAudioEngine::get()->playEffect("crystal01.ogg", 1, 1, volume);
        } else {
            FMODAudioEngine::get()->playEffect("reward01.ogg", 1, 1, volume);
        }
    }

    bg->runAction(CCSequence::create(
        CCEaseOut::create(CCMoveBy::create(0.5f, { moveX, 0.0f }), 0.6f),
        CCDelayTime::create(Mod::get()->getSettingValue<double>("time")),
        //CCEaseIn::create(CCMoveBy::create(0.5f, { -moveX, 0.0f }), 0.6f),
        //CCDelayTime::create(0.5F),
        //CCCallFunc::create(this, callfunc_selector(EventsPush::destroySelf)),
        nullptr
    ));

    this->setZOrder(scene->getHighestChildZ() + 10);
    this->setID("EventsPush"_spr);
    return true;
}

void EventsPush::processNextEvent(CCScene *self) {
    if (!g_eventQueue.empty()) {
        auto data = std::move(g_eventQueue.front());
        g_eventQueue.pop();
        g_processingEvents = true;
        auto layer = EventsPush::create(data);
        GDUtils::Events::OnRate(data.type).send(data);
        if (layer && self) {
            self->addChild(layer);
        }
    }
}

void EventsPush::pushRateLevel(CCScene *self, EventData data) {
    log::debug(
        "EventData [\ndemon = {};\nstarsum = {};\nstars = {};\nrate = {};\ntype = {};\ntitle = {};\nsprite = {};\nlevel_name = {};\nlevel_creator = {};\ncoins = {};\nverified_coins = {};\nplatformer = {};\nlevel_id = {};\nlevels_list = {};\nmaxToCompleteList = {};\n]",
        data.demon,
        data.starsum,
        data.stars,
        data.rate,
        data.typeToInt(),
        data.title,
        data.sprite,
        data.level_name,
        data.level_creator,
        data.coins,
        data.verified_coins,
        data.platformer,
        data.level_id,
        data.levels_list,
        data.maxToCompleteList
    );
    g_eventQueue.push(data);
    // Start processing events if no events are being processed currently.
    if (!g_processingEvents) {
        processNextEvent(self);
    }
}
/*
payload = {
    "type": 7,
    "title": "test"
}
*/

void EventsPush::callCancel() {
    if (g_finishTx) {
        (void)g_finishTx->trySend(std::move(true));
    }
}
void EventsPush::eventCompletedCallback(CCScene *self) {
    g_processingEvents = false;
    processNextEvent(self);
}

std::string getNodeName(cocos2d::CCObject* node) {
#ifdef GEODE_IS_WINDOWS
    return typeid(*node).name() + 6;
#else 
    {
        std::string ret;

        int status = 0;
        auto demangle = abi::__cxa_demangle(typeid(*node).name(), 0, 0, &status);
        if (status == 0) {
            ret = demangle;
        }
        free(demangle);

        return ret;
    }
#endif
}

static std::string s_currentLayer;

void EventsPush::setTx(arc::mpsc::Sender<bool> tx) {
    g_finishTx = std::move(tx);
}

bool EventsPush::canSendEvent(CCScene* scene) {
    auto layer = scene->getChildren()->objectAtIndex(0);
    std::string layerName = getNodeName(layer);
    if (layerName == "cocos2d::CCLayerColor") return false;
    if (layerName == "LoadingLayer") return false;
    bool everywhereElse = Mod::get()->getSettingValue<bool>("everywhereElse");
    bool inLevels = Mod::get()->getSettingValue<bool>("inLevels");
    bool inEditor = Mod::get()->getSettingValue<bool>("inEditor");
    bool inPlatformers = Mod::get()->getSettingValue<bool>("inPlatformers");
    bool pushEvent = true;
    if (layerName == "PlayLayer") {
        if (!inLevels) pushEvent = false;
    }
    if (layerName == "LevelEditorLayer" && !inEditor) {
        pushEvent = false;
    }
    if ((layerName != "LevelEditorLayer" && layerName != "PlayLayer") && !everywhereElse) {
        pushEvent = false;
    }
    return pushEvent;
}
$execute {
    GDUtils::Events::RateEvent().listen([](EventData const& e) {
        auto json = matjson::Serialize<EventData>::toJson(e);
        std::string toStr = json.dump(0);
        AMQT::sendToTx(std::move(toStr));
        return ListenerResult::Stop;
    }).leak();
    // ".leak() is for global listeners pretty much"
}
