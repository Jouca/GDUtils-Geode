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
        CCEaseIn::create(CCMoveBy::create(0.5f, { -moveX, 0.0f }), 0.6f),
        CCDelayTime::create(0.5F),
        CCCallFunc::create(this, callfunc_selector(EventsPush::destroySelf)),
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
        GDUtils::Events::RateEvent::typeToInt(data.type),
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
