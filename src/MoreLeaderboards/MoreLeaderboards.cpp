#include "MoreLeaderboards.h"
#include <Geode/modify/LeaderboardsLayer.hpp>
#include <Geode/ui/GeodeUI.hpp>

static StatsListType g_tab = StatsListType::Diamonds;
static int page = 0;
static int start_count = 0;
static int end_count = 0;
static int total_count = 0;
static std::unordered_map<std::string, web::WebTask> RUNNING_REQUESTS {};

std::vector<std::string> MoreLeaderboards::getWords(std::string s, std::string delim) {
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
CCDictionary* MoreLeaderboards::responseToDict(const std::string& response){
    auto dict = CCDictionary::create();
    std::stringstream responseStream(response);
    std::string currentKey;
    std::string keyID;
    std::string playerID;
    std::string username;
    std::string accountID;

    GameLevelManager* glm = GameLevelManager::sharedState();
    unsigned int i = 0;
    while(getline(responseStream, currentKey, ':')){
        if(i % 2 == 0) {
            keyID = currentKey;
        } else {
            if (keyID == "2") { // Player ID
                playerID = currentKey;
            } else if (keyID == "16") {
                accountID = currentKey;
            } else if (keyID == "1") {
                username = currentKey;
            }
            dict->setObject(CCString::create(currentKey.c_str()),keyID);
        }
        i++;
    }
    if (playerID.length() > 0 && username.length() > 0 && accountID.length() > 0) {
        glm->storeUserName(std::stoi(playerID), std::stoi(accountID), username);
    }
    return dict;
}
void MoreLeaderboards::onMoreLeaderboards(CCObject* pSender) {
    auto scene = CCScene::create();
    auto layer = MoreLeaderboards::create("more");
    scene->addChild(layer);
    auto transitionFade = CCTransitionFade::create(0.5, scene);
    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void MoreLeaderboards::onModsList(CCObject* pSender) {
    auto scene = CCScene::create();
    auto layer = MoreLeaderboards::create("mods");
    scene->addChild(layer);
    auto transitionFade = CCTransitionFade::create(0.5, scene);
    CCDirector::sharedDirector()->pushScene(transitionFade);
}

void MoreLeaderboards::onRegion(CCObject* pSender) {
    geode::createQuickPopup(
        "Coming soon!",
        R"text(
<cy>Filter by country</c> leaderboards is coming soon on GDUtils!
Stay tuned for updates via our <cj>Discord server</c> on the page of the mod in the <cy>Geode Index</c>!
        )text",
        "OK", "Mod page",
        [](auto, bool btn2) {
            if (btn2) {
                openIndexPopup(Mod::get());
            }
        }
    );
}

MoreLeaderboards* MoreLeaderboards::create(std::string type) {
    auto pRet = new MoreLeaderboards();
    if (pRet && pRet->init(type)) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool MoreLeaderboards::init(std::string type) {
    auto director = cocos2d::CCDirector::sharedDirector();
    auto size = director->getWinSize();

    displayedData = cocos2d::CCArray::create();
    displayedData->retain();

    auto background = CCSprite::create("GJ_gradientBG.png");
    auto backgroundSize = background->getContentSize();

    background->setScaleX(size.width / backgroundSize.width);
    background->setScaleY(size.height / backgroundSize.height);
    background->setAnchorPoint({0, 0});
    background->setColor({0, 101, 253});
    background->setZOrder(-1);
    this->addChild(background);

    // Corners
    CCSprite* corner_left = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    CCSprite* corner_right = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    corner_right->setRotation(-90);
    corner_left->setPosition({35, 35});
    corner_right->setPosition({size.width - 35, 35});
    this->addChild(corner_left);
    this->addChild(corner_right);

    CCSprite* button = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
    auto backButton = CCMenuItemSpriteExtra::create(button, this, menu_selector(MoreLeaderboards::backButton));
    CCMenu* menu = cocos2d::CCMenu::create();
    menu->addChild(backButton);
    menu->setPosition({ 25, size.height - 25 });
    this->addChild(menu);

    if (type == "more") {
        // Change region
        auto menu_region = CCMenu::create();

        auto regionSpr = CCSprite::create(Mod::get()->expandSpriteName("earth_btn.png"));
        regionSpr->setScale(.8f);
        auto regionBtn = CCMenuItemSpriteExtra::create(
            regionSpr,
            this,
            menu_selector(MoreLeaderboards::onRegion)
        );
        regionBtn->setPosition(239, -70);
        menu_region->addChild(regionBtn);

        this->addChild(menu_region);

        // create menus
        m_menu = CCMenu::create();

        m_diamondsTabBtn = TabButton::create("Diamonds", this, menu_selector(MoreLeaderboards::onTab));
        m_diamondsTabBtn->setPosition(-155.f, 132);
        m_diamondsTabBtn->setTag(static_cast<int>(StatsListType::Diamonds));
        m_diamondsTabBtn->setZOrder(30);
        m_diamondsTabBtn->setScale(0.8f);
        m_menu->addChild(m_diamondsTabBtn);

        m_usercoinsTabBtn = TabButton::create("User Coins", this, menu_selector(MoreLeaderboards::onTab));
        m_usercoinsTabBtn->setPosition(-77.f, 132);
        m_usercoinsTabBtn->setTag(static_cast<int>(StatsListType::UserCoins));
        m_usercoinsTabBtn->setZOrder(30);
        m_usercoinsTabBtn->setScale(0.8f);
        m_menu->addChild(m_usercoinsTabBtn);

        m_demonsTabBtn = TabButton::create("Demons", this, menu_selector(MoreLeaderboards::onTab));
        m_demonsTabBtn->setPosition(0.f, 132);
        m_demonsTabBtn->setTag(static_cast<int>(StatsListType::Demons));
        m_demonsTabBtn->setZOrder(30);
        m_demonsTabBtn->setScale(0.8f);
        m_menu->addChild(m_demonsTabBtn);

        m_moonsTabBtn = TabButton::create("Moons", this, menu_selector(MoreLeaderboards::onTab));
        m_moonsTabBtn->setPosition(78.f, 132);
        m_moonsTabBtn->setTag(static_cast<int>(StatsListType::Moons));
        m_moonsTabBtn->setZOrder(30);
        m_moonsTabBtn->setScale(0.8f);
        m_menu->addChild(m_moonsTabBtn);

        m_creatorsTabBtn = TabButton::create("Creators", this, menu_selector(MoreLeaderboards::onTab));
        m_creatorsTabBtn->setPosition(156.f, 132);
        m_creatorsTabBtn->setTag(static_cast<int>(StatsListType::Creators));
        m_creatorsTabBtn->setZOrder(30);
        m_creatorsTabBtn->setScale(0.8f);
        m_menu->addChild(m_creatorsTabBtn);

        // tabs gradient
        #ifndef GEODE_IS_IOS
        m_tabsGradientNode = CCClippingNode::create();
        #else
        m_tabsGradientNode = CCNode::create();
        #endif
        m_tabsGradientNode->setContentSize(this->getContentSize());
        m_tabsGradientNode->setAnchorPoint({0.5f, 0.5f});
        m_tabsGradientNode->ignoreAnchorPointForPosition(true);
        m_tabsGradientNode->setZOrder(0);
        m_tabsGradientNode->setScale(0.8f);
        #ifndef GEODE_IS_IOS
        m_tabsGradientNode->setInverted(false);
        m_tabsGradientNode->setAlphaThreshold(0.7f);
        #endif

        m_tabsGradientSprite = CCSprite::create("tab-gradient.png"_spr);
        m_tabsGradientNode->addChild(m_tabsGradientSprite);

        m_tabsGradientStencil = CCSprite::create("tab-gradient-mask.png"_spr);
        m_tabsGradientStencil->setAnchorPoint({0.f, 0.f});
        m_tabsGradientStencil->setColor({172, 255, 67});
        m_tabsGradientStencil->setZOrder(1);
        m_tabsGradientStencil->setScale(0.8f);

        #ifndef GEODE_IS_IOS
        m_tabsGradientNode->setStencil(m_tabsGradientStencil);
        #endif

        // add menus
        m_menu->setZOrder(1);

        this->addChild(m_tabsGradientNode);
        this->addChild(m_tabsGradientStencil);
        this->addChild(m_menu);

        // select first tab
        this->onTab(nullptr);
    } else if (type == "mods") {
        startLoadingMods();
        loadPageMods();
    }
    setTouchEnabled(true);
    setKeypadEnabled(true);
    return true;
}

void MoreLeaderboards::backButton(cocos2d::CCObject*) {
    cocos2d::CCDirector::sharedDirector()->popSceneWithTransition(0.5F, cocos2d::PopTransition::kPopTransitionFade);
};

void MoreLeaderboards::keyBackClicked() {
    backButton(CCNode::create());
}

void MoreLeaderboards::fadeLoadingCircle() {
    if (loading_circle == nullptr) return;
    loading_circle->fadeAndRemove();
};

void MoreLeaderboards::handle_request_mods(std::string const& data) {
    if(!displayedData) { displayedData = CCArray::create(); displayedData->retain(); };
    if (data != "-1") {
        displayedData = CCArray::create();
        
        std::vector<std::string> mods = getWords(data, "|");

        while (mods.size() > 0) {
            std::string mod = mods[0];
            CCDictionary* modDict = CCDictionary::create();
            modDict->setObject(cocos2d::CCString::create(mod), "modstring");
            displayedData->addObject(modDict);
            mods.erase(mods.begin());
        };
    }
    loadPageMods();
}

void MoreLeaderboards::startLoadingMods() {
    loading_circle = LoadingCircle::create();
    loading_circle->setParentLayer(this);
    loading_circle->show();

    const geode::utils::MiniFunction<void(std::string const&)> then = [this](std::string const& data) {
        fadeLoadingCircle();
        handle_request_mods(data);
    };
    const geode::utils::MiniFunction<void(std::string const&)> expect = [this](std::string const& error) {
        fadeLoadingCircle();
    };

    geode::utils::web::WebRequest request = web::WebRequest();
    RUNNING_REQUESTS.emplace(
        "@loaderModListCheck",
        request.get("https://clarifygdps.com/gdutils/modslist.php").map(
            [expect = std::move(expect), then = std::move(then)](web::WebResponse* response) {
                if (response->ok()) {
                    then(response->string().value());
                } else {
                    expect("An error occured while sending a request on our server. Please try again later.");
                }

                RUNNING_REQUESTS.erase("@loaderModListCheck");
                return *response;
            }
        )
    );
};

void MoreLeaderboards::loadPageMods() {
    if(listLayer != nullptr) listLayer->removeFromParentAndCleanup(true);

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    leaderboardView = MoreLeaderboardsListView::create(displayedData, 356.f, 220.f);
    listLayer = GJListLayer::create(leaderboardView, "GD Moderators", {191, 114, 62, 255}, 356.f, 220.f, 0);
    listLayer->setPosition(winSize / 2 - listLayer->getScaledContentSize() / 2 - CCPoint(0,5));

    addChild(listLayer);
}

void MoreLeaderboards::startLoadingMore() {
    loading_circle = LoadingCircle::create();
    loading_circle->setParentLayer(this);
    loading_circle->show();

    std::string type = "";

    if (g_tab == StatsListType::Diamonds) {
        type = "diamonds";
    } else if (g_tab == StatsListType::UserCoins) {
        type = "ucoins";
    } else if (g_tab == StatsListType::Demons) {
        type = "demons";
    } else if (g_tab == StatsListType::Moons) {
        type = "moons";
    } else if (g_tab == StatsListType::Creators) {
        type = "cp";
    }

    this->retain();

    geode::utils::web::WebRequest request = web::WebRequest();
    const geode::utils::MiniFunction<void(std::string const&)> then = [this](std::string const& data) {
        loading = false;
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        auto layer = scene->getChildren()->objectAtIndex(0);
        if (layer == nullptr) return this->release();
        if (misc::getNodeName(layer) != "MoreLeaderboards") return this->release(); // prevent le crash, even though the layer shouldve already been destructed
        if (data == "-1" || data.length() < 2) {
            fadeLoadingCircle();
            geode::createQuickPopup(
                "Error",
                "An error occured while sending a request on <cy>our server</c>. Please try again later.",
                "OK", nullptr,
                [this](auto, bool btn2) {
                    if (!btn2) {
                        g_tab = StatsListType::Diamonds;
                        keyBackClicked();
                    }
                }
            );
            this->release();
            return;
        }
        fadeLoadingCircle();

        handle_request_more(data);
        loading = false;

        this->release();
    };
    const geode::utils::MiniFunction<void(std::string const&)> expect = [this](std::string const& error) {
        loading = false;
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        auto layer = scene->getChildren()->objectAtIndex(0);
        if (layer == nullptr) return this->release();
        if (misc::getNodeName(layer) != "MoreLeaderboards") return this->release();
        geode::createQuickPopup(
            "Error",
            "An error occured while sending a request on <cy>our server</c>. Please try again later.",
            "OK", nullptr,
            [this](auto, bool btn2) {
                if (!btn2) {
                    g_tab = StatsListType::Diamonds;
                    keyBackClicked();
                }
            }
        );
        fadeLoadingCircle();
        loading = false;
        this->release();
    };

    RUNNING_REQUESTS.emplace(
        "@loaderMoreLeaderboardCheck",
        request.bodyString(fmt::format("type={}&page={}", type, page)).post("https://clarifygdps.com/gdutils/moreleaderboards.php").map(
            [expect = std::move(expect), then = std::move(then)](web::WebResponse* response) {
                if (response->ok()) {
                    then(response->string().value());
                } else {
                    expect("An error occured while sending a request on our server. Please try again later.");
                }

                RUNNING_REQUESTS.erase("@loaderMoreLeaderboardCheck");
                return *response;
            }
        )
    );
};

void MoreLeaderboards::handle_request_more(std::string const& data) {
    if(!displayedData) { displayedData = CCArray::create(); displayedData->retain(); };

    if (data != "-1") {
        displayedData = CCArray::create();

        std::vector<std::string> dataString = getWords(data, "#");

        std::vector<std::string> users = getWords(dataString[0], "|");
        std::vector<std::string> data_page = getWords(dataString[1], "|");

        while (users.size() > 0) {
            std::string user = users[0];

            auto score = GJUserScore::create(
                MoreLeaderboards::responseToDict(user)
            );
            displayedData->addObject(score);
            users.erase(users.begin());
        };

        int id = 0;
        while (data_page.size() > 0) {
            std::string page = data_page[0];

            if (id == 0) {
                start_count = std::stoi(page);
            } else if (id == 1) {
                end_count = std::stoi(page);
            } else if (id == 2) {
                total_count = std::stoi(page);
            }

            id++;
            data_page.erase(data_page.begin());
        }
    }

    loadPageMore();
    loadPageStats();
}

void MoreLeaderboards::loadPageMore() {
    if(listLayer != nullptr) listLayer->removeFromParentAndCleanup(true);

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    leaderboardViewScore = CustomListView::create(displayedData, nullptr, 220.f, 356.f, 0, BoomListType::Score, 0.0F);
    listLayer = GJListLayer::create(leaderboardViewScore, nullptr, {191, 114, 62, 255}, 356.f, 220.f, 0);
    listLayer->setZOrder(31);
    listLayer->setPosition(winSize / 2 - listLayer->getScaledContentSize() / 2 - CCPoint(0,5));

    addChild(listLayer);
}

void MoreLeaderboards::loadPageStats() {
    if (page_label != nullptr) page_label->removeFromParentAndCleanup(true);

    CCMenu* menu_label = CCMenu::create();
    menu_label->setLayout(
        RowLayout::create()
        ->setAxisAlignment(AxisAlignment::Center)
        ->setGap(10.f)
    );
    menu_label->setPosition({ 285.f, 12.f });
    menu_label->setContentSize({ 300.f, 7.f });

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    std::string fmt = fmt::format("Top {} - {} of {}", start_count, end_count, total_count);
    page_label = CCLabelBMFont::create(fmt.c_str(), "goldFont.fnt");
    page_label->setZOrder(40);
    page_label->setScale(.4f);
    menu_label->addChild(page_label);

    trophy = CCSprite::createWithSpriteFrameName("rankIcon_top10_001.png");
    trophy->setScale(.3f);
    trophy->setPosition(CCPoint { -5.f, 0.f } - page_label->getScaledContentSize().width);
    menu_label->addChild(trophy);

    addChild(menu_label);
    menu_label->updateLayout();

    CCMenu* menu = CCMenu::create();

    if (page_left != nullptr) page_left->removeFromParentAndCleanup(true);
    if (page_right != nullptr) page_right->removeFromParentAndCleanup(true);

    if (page > 0) {
        page_left = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png"),
            this,
            menu_selector(MoreLeaderboards::onPageLeft)
        );
        page_left->setPosition(-220, 0);
        menu->addChild(page_left);
    }

    if (end_count < total_count) {
        auto sprite = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
        sprite->setFlipX(true);
        page_right = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(MoreLeaderboards::onPageRight)
        );
        page_right->setPosition(220, 0);
        menu->addChild(page_right);
    }

    addChild(menu);
}

void MoreLeaderboards::onPageLeft(CCObject* pSender) {
    if (loading) return;

    loading = true;

    if (displayedData) {
        displayedData->release();
        displayedData = cocos2d::CCArray::create();
        displayedData->retain();
    }

    page--;
    resetInfos();
    
    if (loading) {
        startLoadingMore();
        loadPageMore();
    }
}

void MoreLeaderboards::onPageRight(CCObject* pSender) {
    if (loading) return;

    loading = true;

    if (displayedData) {
        displayedData->release();
        displayedData = cocos2d::CCArray::create();
        displayedData->retain();
    }

    page++;
    resetInfos();
    
    if (loading) {
        startLoadingMore();
        loadPageMore();
    }
}

void MoreLeaderboards::resetInfos() {
    if (page_label != nullptr) page_label->removeFromParentAndCleanup(true);
    if (page_left != nullptr) page_left->removeFromParentAndCleanup(true);
    if (page_right != nullptr) page_right->removeFromParentAndCleanup(true);
    if (trophy != nullptr) trophy->removeFromParentAndCleanup(true);

    page_label = nullptr;
    page_left = nullptr;
    page_right = nullptr;
    trophy = nullptr;
}

void MoreLeaderboards::onTab(CCObject* pSender) {
    if (loading) return;

    loading = true;

    if (displayedData) {
        displayedData->release();
        displayedData = cocos2d::CCArray::create();
        displayedData->retain();
    }

    if (pSender) {
        g_tab = static_cast<StatsListType>(pSender->getTag());
    }

    resetInfos();

    page = 0;

    auto toggleTab = [this](CCMenuItemToggler* member) -> void {
        auto isSelected = member->getTag() == static_cast<int>(g_tab);
        auto targetMenu = m_menu;
        member->toggle(isSelected);
        if (member->getParent() != targetMenu) {
            member->retain();
            member->removeFromParent();
            targetMenu->addChild(member);
            member->release();
        }
        if (isSelected && m_tabsGradientStencil)
            m_tabsGradientStencil->setPosition(member->m_onButton->convertToWorldSpace({0.f, 0.f}));
    };

    toggleTab(m_diamondsTabBtn);
    toggleTab(m_usercoinsTabBtn);
    toggleTab(m_demonsTabBtn);
    toggleTab(m_moonsTabBtn);
    toggleTab(m_creatorsTabBtn);

    if (loading) {
        startLoadingMore();
        loadPageMore();
    }
}

class $modify(LeaderboardsLayer) {
    bool init(LeaderboardState state) {
        if (!LeaderboardsLayer::init(state)) return false;

        auto menu = CCMenu::create();

        auto plusSpr = CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png");
        plusSpr->setScale(.8f);
        auto plusBtn = CCMenuItemSpriteExtra::create(
            plusSpr,
            this,
            menu_selector(MoreLeaderboards::onMoreLeaderboards)
        );
        plusBtn->setPosition(239, 20);
        menu->addChild(plusBtn);

        auto modsSpr = CCSprite::createWithSpriteFrameName("modBadge_01_001.png");
        modsSpr->setScale(1.5f);
        auto modsBtn = CCMenuItemSpriteExtra::create(
            modsSpr,
            this,
            menu_selector(MoreLeaderboards::onModsList)
        );
        modsBtn->setPosition(239, -24);
        menu->addChild(modsBtn);

        this->addChild(menu);

        return true;
    }
};
