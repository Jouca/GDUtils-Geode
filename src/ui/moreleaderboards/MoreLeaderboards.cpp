#include "MoreLeaderboards.hpp"
#include "tabs/TabButton.hpp"
#include <Geode/ui/GeodeUI.hpp>
#include <Geode/loader/Loader.hpp>

StatsListType MoreLeaderboards::g_tab = StatsListType::Stars;
bool MoreLeaderboards::modFilter = false;
int MoreLeaderboards::modFilterType = 0;
std::string MoreLeaderboards::username = "";
int MoreLeaderboards::scroll_int = 0;
std::string MoreLeaderboards::data_response_moreLB = "";

static int page = 0;
static int start_count = 0;
static int end_count = 0;
static int total_count = 0;

std::vector<std::string> MoreLeaderboards::getWords(std::string s, std::string delim) {
    std::vector<std::string> res;
    std::string token = "";
    for (int i = 0; i < (int)s.size(); i++) {
        bool flag = true;
        for (int j = 0; j < (int)delim.size(); j++) {
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

class SearchUserLBLayer : public Popup {
protected:
    bool init(MoreLeaderboards* layer) {
        if (!Popup::init(260.f, 75.f)) return false;
        m_layer = layer;
        input_username->setMaxCharCount(20);
        input_username->setPlaceholder("");
        m_buttonMenu->addChildAtPosition(input_username, Anchor::Center, {0, 10});

        Build<ButtonSprite>::create("Search", 60, true, "bigFont.fnt", "GJ_button_01.png", 30, .5F)
            .intoMenuItem([this]() {
                if (this->m_layer->loading) return;
                MoreLeaderboards::username = input_username->getString();
                this->m_layer->changeTabPage();
                this->m_layer->onTab(nullptr);
                Popup::onClose(nullptr);
            }).parentAtPos(m_buttonMenu, Anchor::Center, {0, -30});

        if (MoreLeaderboards::username != "") {
            input_username->setString(MoreLeaderboards::username);
            Build<CCSprite>::createSpriteName("GJ_deleteBtn_001.png").scale(.45f)
                .intoMenuItem([this]() {
                    MoreLeaderboards::username = "";
                    MoreLeaderboards::scroll_int = 0;
                    input_username->setString("");
                    this->m_layer->changeTabPage();
                    this->m_layer->onTab(nullptr);
                    Popup::onClose(nullptr);
                }).parentAtPos(m_buttonMenu, Anchor::Right, {-17, 0});
        }
        return true;
    }

public:
    MoreLeaderboards* m_layer = nullptr;
    TextInput* input_username = TextInput::create(200.0F, "Username", "bigFont.fnt");

    static SearchUserLBLayer* create(MoreLeaderboards* layer) {
        auto ret = new SearchUserLBLayer();
        if (ret->init(layer)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
};

CCDictionary* MoreLeaderboards::responseToDict(const std::string& response) {
    auto dict = CCDictionary::create();
    std::stringstream responseStream(response);
    std::string currentKey;
    std::string keyID;
    std::string playerID;
    std::string username;
    std::string accountID;

    GameLevelManager* glm = GameLevelManager::sharedState();
    unsigned int i = 0;
    while (getline(responseStream, currentKey, ':')) {
        if (i % 2 == 0) {
            keyID = currentKey;
        } else {
            if (keyID == "2")  playerID = currentKey;
            else if (keyID == "16") accountID = currentKey;
            else if (keyID == "1")  username = currentKey;
            dict->setObject(CCString::create(currentKey.c_str()), keyID);
        }
        i++;
    }
    if (playerID.length() > 0 && username.length() > 0 && accountID.length() > 0) {
        glm->storeUserName(
            numFromString<int>(playerID).unwrapOrDefault(),
            numFromString<int>(accountID).unwrapOrDefault(),
            username
        );
    }
    return dict;
}

void MoreLeaderboards::onMoreLeaderboards(CCObject*) {
    auto scene = CCScene::create();
    scene->addChild(MoreLeaderboards::create("more"));
    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5, scene));
}

void MoreLeaderboards::onModsList(CCObject*) {
    auto scene = CCScene::create();
    scene->addChild(MoreLeaderboards::create("mods"));
    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5, scene));
}

void MoreLeaderboards::onRegion(CCObject*) {
    SelectRegion::scene([this](std::string id) {
        country_id = id;
        if (loading) return;
        loading = true;
        loading_circle = LoadingCircle::create();
        loading_circle->setZOrder(25);
        loading_circle->setParentLayer(this);
        loading_circle->show();

        if (displayedData) {
            displayedData->release();
            displayedData = cocos2d::CCArray::create();
            displayedData->retain();
        }
        page = 0;
        if (loading) {
            startLoadingMore();
            loadPageMore();
        }
        resetInfos();
    });
}

void MoreLeaderboards::onInfo(CCObject*) {
    FLAlertLayer::create(
        nullptr,
        "More Leaderboards",
        R"text(
<cy>More Leaderboards</c> is a feature that allows you to view more leaderboards than the ones in the game.
You can view leaderboards for <cj>Diamonds</c>, User Coins, <cr>Demons</c>, <cl>Moons</c>, and Creators Points of people that are registered on <cy>Updated Leaderboards</c> project.

Thanks to <cj>ChiefWoof</c> & <co>the Helper team</c> for providing the data for this feature!
        )text",
        "OK", nullptr, 400.0f
    )->show();
}

void MoreLeaderboards::onLBInfo(CCObject*) {
    geode::createQuickPopup(
        "Submit your user stats",
        R"text(
You can submit your <cy>user stats</c> to the <cj>Updated Leaderboards</c> team by joining their <cb>Discord server</c> and following the instructions right there.
        )text",
        "OK", "Discord Server",
        [](auto, bool btn2) {
            if (btn2) web::openLinkInBrowser("https://discord.gg/mZnDm886sR");
        }
    );
}

MoreLeaderboards* MoreLeaderboards::create(std::string type) {
    auto pRet = new MoreLeaderboards();
    SelectRegionCell::country_id = "NAN";
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
    background->setColor(Mod::get()->getSettingValue<ccColor3B>("background"));
    background->setZOrder(-1);
    this->addChild(background);

    auto corner_left = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    auto corner_right = CCSprite::createWithSpriteFrameName("GJ_sideArt_001.png");
    corner_right->setRotation(-90);
    corner_left->setPosition({35, 35});
    corner_right->setPosition({size.width - 35, 35});
    this->addChild(corner_left);
    this->addChild(corner_right);

    auto backBtn = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
        this, menu_selector(MoreLeaderboards::backButton)
    );
    auto backMenu = CCMenu::create();
    backMenu->addChild(backBtn);
    backMenu->setPosition({25, size.height - 25});
    this->addChild(backMenu);

    if (type == "more") {
        auto menu_region = CCMenu::create();

        auto regionSpr = CCSprite::create("earth_btn.png"_spr);
        regionSpr->setScale(.8f);
        auto regionBtn = CCMenuItemSpriteExtra::create(
            regionSpr, this, menu_selector(MoreLeaderboards::onRegion)
        );
        regionBtn->setPosition(239, -52);
        menu_region->addChild(regionBtn);

        auto infoBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png"),
            this, menu_selector(MoreLeaderboards::onInfo)
        );
        infoBtn->setPosition(-239, -120);
        menu_region->addChild(infoBtn);

        auto lbBtn = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_levelLeaderboardBtn_001.png"),
            this, menu_selector(MoreLeaderboards::onLBInfo)
        );
        lbBtn->setPosition(-239, -57);
        menu_region->addChild(lbBtn);

        this->addChild(menu_region);

        tab_page = 0;

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

        this->addChild(m_tabsGradientNode);
        this->addChild(m_tabsGradientStencil);

        changeTabPage();
        this->onTab(nullptr);
    } else if (type == "mods") {
        loading_circle = LoadingCircle::create();
        loading_circle->setZOrder(25);
        loading_circle->setParentLayer(this);
        loading_circle->show();
        startLoadingMods();
        loadPageMods();
    }

    setTouchEnabled(true);
    setKeypadEnabled(true);
    return true;
}

void MoreLeaderboards::backButton(cocos2d::CCObject*) {
    MoreLeaderboards::data_response_moreLB = "";
    MoreLeaderboards::g_tab = StatsListType::Stars;
    MoreLeaderboards::username = "";
    MoreLeaderboards::scroll_int = 0;
    MoreLeaderboards::modFilter = false;
    MoreLeaderboards::modFilterType = 0;
    cocos2d::CCDirector::sharedDirector()->popSceneWithTransition(0.5F, cocos2d::PopTransition::kPopTransitionFade);
}

void MoreLeaderboards::keyBackClicked() {
    backButton(CCNode::create());
}

void MoreLeaderboards::fadeLoadingCircle() {
    if (loading_circle == nullptr) return;
    loading_circle->fadeAndRemove();
}

void MoreLeaderboards::handle_request_mods(std::string const& data) {
    if (!displayedData) { displayedData = CCArray::create(); displayedData->retain(); }
    if (data != "-1") {
        displayedData = CCArray::create();
        std::vector<std::string> mods = getWords(data, "|");
        while (mods.size() > 0) {
            CCDictionary* modDict = CCDictionary::create();
            modDict->setObject(CCString::create(mods[0]), "modstring");
            displayedData->addObject(modDict);
            mods.erase(mods.begin());
        }
    }
    loadPageMods();
}

void MoreLeaderboards::startLoadingMods() {
    this->retain();
    m_listener.spawn(
        web::WebRequest().get("https://clarifygdps.com/gdutils/modslist.php"),
        [this](web::WebResponse value) {
            if (value.ok()) handle_request_mods(value.string().unwrapOrDefault());
            fadeLoadingCircle();
            this->release();
        }
    );
}

void MoreLeaderboards::loadPageMods() {
    if (listLayer != nullptr) listLayer->removeFromParentAndCleanup(true);
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    leaderboardView = MoreLeaderboardsListView::create(displayedData, 356.f, 220.f);
    listLayer = GJListLayer::create(leaderboardView, "GD Moderators", {191, 114, 62, 255}, 356.f, 220.f, 0);
    listLayer->setPosition(winSize / 2 - listLayer->getScaledContentSize() / 2 - CCPoint(0, 5));
    addChild(listLayer);
}

static std::string tabTypeToString(StatsListType t) {
    switch (t) {
        case StatsListType::Diamonds:                return "diamonds";
        case StatsListType::UserCoins:               return "ucoins";
        case StatsListType::Demons:                  return "demons";
        case StatsListType::Moons:                   return "moons";
        case StatsListType::Creators:                return "cp";
        case StatsListType::Stars:                   return "stars";
        case StatsListType::classicDemonsEasy:       return "classicDemonsEasy";
        case StatsListType::classicDemonsMedium:     return "classicDemonsMedium";
        case StatsListType::classicDemonsHard:       return "classicDemonsHard";
        case StatsListType::classicDemonsInsane:     return "classicDemonsInsane";
        case StatsListType::classicDemonsExtreme:    return "classicDemonsExtreme";
        case StatsListType::platformerDemonsEasy:    return "platformerDemonsEasy";
        case StatsListType::platformerDemonsMedium:  return "platformerDemonsMedium";
        case StatsListType::platformerDemonsHard:    return "platformerDemonsHard";
        case StatsListType::platformerDemonsInsane:  return "platformerDemonsInsane";
        case StatsListType::platformerDemonsExtreme: return "platformerDemonsExtreme";
        case StatsListType::BetterProgression:       return "betterProgression";
        case StatsListType::Pointercrate:            return "pointercratePoints";
    }
    return "stars";
}

void MoreLeaderboards::startLoadingLeaderboard() {
    this->retain();
    auto type = tabTypeToString(MoreLeaderboards::g_tab);
    m_lbListener.spawn(
        web::WebRequest()
            .param("type", type)
            .param("page", page)
            .param("country", country_id)
            .param("username", username)
            .param("mod", (modFilter ? "1" : "0"))
            .param("modFilter", modFilterType)
            .param("version", "1.0")
            .get("https://clarifygdps.com/gdutils/moreleaderboards.php"),
        [this](web::WebResponse value) {
            loading = false;
            auto scene = CCDirector::sharedDirector()->getRunningScene();
            auto layer = scene->getChildren()->objectAtIndex(0);
            if (layer == nullptr || typeinfo_cast<MoreLeaderboards*>(layer) == nullptr) {
                this->release();
                return;
            }
            if (!value.ok()) {
                fadeLoadingCircle();
                geode::createQuickPopup("Error",
                    "An error occured while sending a request on <cy>our server</c>. Please try again later.",
                    "OK", nullptr,
                    [this](auto, bool btn2) {
                        if (!btn2) { MoreLeaderboards::g_tab = StatsListType::Stars; keyBackClicked(); }
                    }
                );
                this->release();
                return;
            }
            auto data = value.string().unwrap();
            if (data == "-1" || data.length() < 2) {
                fadeLoadingCircle();
                geode::createQuickPopup("Error",
                    "An error occured while sending a request on <cy>our server</c>. Please try again later.",
                    "OK", nullptr,
                    [this](auto, bool btn2) {
                        if (!btn2) { MoreLeaderboards::g_tab = StatsListType::Stars; keyBackClicked(); }
                    }
                );
                this->release();
                return;
            }
            if (data == "-2") {
                fadeLoadingCircle();
                FLAlertLayer::create(nullptr, "Error",
                    "User not found : " + MoreLeaderboards::username,
                    "OK", nullptr, 200.0F
                )->show();
                MoreLeaderboards::scroll_int = 0;
                loadTabPageButtons();
                this->release();
                return;
            }
            handle_request_more(data);
            fadeLoadingCircle();
            this->release();
        }
    );
}

void MoreLeaderboards::startLoadingMore() {
    if (data_region != "") {
        startLoadingLeaderboard();
        return;
    }
    this->retain();
    m_listener.spawn(
        web::WebRequest().get("https://clarifygdps.com/gdutils/moreleaderboards_region.php"),
        [this](web::WebResponse value) {
            if (!value.ok()) {
                loading = false;
                auto scene = CCDirector::sharedDirector()->getRunningScene();
                auto layer = scene->getChildren()->objectAtIndex(0);
                if (layer == nullptr || typeinfo_cast<MoreLeaderboards*>(layer) == nullptr) {
                    this->release();
                    return;
                }
                fadeLoadingCircle();
                geode::createQuickPopup("Error",
                    "An error occured while sending a request on <cy>our server</c>. Please try again later.",
                    "OK", nullptr,
                    [this](auto, bool btn2) {
                        if (!btn2) { MoreLeaderboards::g_tab = StatsListType::Stars; keyBackClicked(); }
                    }
                );
                this->release();
                return;
            }
            auto data = value.string().unwrap();
            if (data != "-1") {
                data_region = data;
                SelectRegion::displayedData = MoreLeaderboards::getWords(data, "|");
            }
            this->release();
            startLoadingLeaderboard();
        }
    );
}

void MoreLeaderboards::handle_request_more(std::string const& data) {
    if (!displayedData) { displayedData = CCArray::create(); displayedData->retain(); }
    if (data != "-1") {
        data_response_moreLB = data;
        displayedData = CCArray::create();
        std::vector<std::string> dataString = getWords(data, "#");
        std::vector<std::string> users = getWords(dataString[0], "|");
        std::vector<std::string> data_page = getWords(dataString[1], "|");

        while (users.size() > 0) {
            auto score = GJUserScore::create(MoreLeaderboards::responseToDict(users[0]));
            displayedData->addObject(score);
            users.erase(users.begin());
        }

        int id = 0;
        while (data_page.size() > 0) {
            int val = numFromString<int>(data_page[0]).unwrapOrDefault();
            if (id == 0) start_count = val;
            else if (id == 1) end_count = val;
            else if (id == 2) total_count = val;
            else if (id == 3) page = val;
            else if (id == 4 && val != 0) MoreLeaderboards::scroll_int = val;
            id++;
            data_page.erase(data_page.begin());
        }
    }
    loadPageMore();
    loadPageStats();
}

void MoreLeaderboards::loadPageMore() {
    if (listLayer != nullptr) listLayer->removeFromParentAndCleanup(true);
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    leaderboardViewScore = CustomListView::create(displayedData, nullptr, 220.f, 356.f, 0, BoomListType::Score, 0.0F);
    listLayer = GJListLayer::create(leaderboardViewScore, nullptr, {191, 114, 62, 255}, 356.f, 220.f, 0);
    listLayer->setZOrder(31);
    listLayer->setPosition(winSize / 2 - listLayer->getScaledContentSize() / 2 - CCPoint(0, 5));
    addChild(listLayer);

    if (MoreLeaderboards::scroll_int != 0) {
        listLayer->m_listView->m_tableView->scrollLayer((float)-INT_MAX);
        listLayer->m_listView->m_tableView->scrollLayer(MoreLeaderboards::scroll_int);
    }
}

void MoreLeaderboards::onSearch(CCObject*) {
    SearchUserLBLayer::create(this)->show();
}

void MoreLeaderboards::loadPageStats() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    if (page_label != nullptr) page_label->removeFromParentAndCleanup(true);
    if (auto n = this->getChildByIDRecursive("menu_label")) n->removeFromParentAndCleanup(true);
    if (auto n = this->getChildByIDRecursive("menu_page"))  n->removeFromParentAndCleanup(true);

    CCMenu* menu_label = CCMenu::create();
    menu_label->setID("menu_label");
    menu_label->setLayout(
        RowLayout::create()->setAxisAlignment(AxisAlignment::Center)->setGap(10.f)
    );
    menu_label->setPosition({winSize.width / 2, 12.f});
    menu_label->setContentSize({300.f, 7.f});

    std::string fmt = fmt::format("Top {} - {} of {}", start_count, end_count, total_count);
    page_label = CCLabelBMFont::create(fmt.c_str(), "goldFont.fnt");
    page_label->setZOrder(40);
    page_label->setLayoutOptions(
        AxisLayoutOptions::create()->setAutoScale(true)->setScaleLimits(0, 0.7F)->setScalePriority(1)
    );
    menu_label->addChild(page_label);

    trophy = CCSprite::createWithSpriteFrameName("rankIcon_top10_001.png");
    trophy->setScale(.3f);
    trophy->setPosition(CCPoint{-5.f, 0.f} - page_label->getScaledContentSize().width);
    menu_label->addChild(trophy);

    addChild(menu_label);
    menu_label->updateLayout();

    CCMenu* menu = CCMenu::create();
    menu->setID("menu_page");

    if (page_left != nullptr) page_left->removeFromParentAndCleanup(true);
    if (page_right != nullptr) page_right->removeFromParentAndCleanup(true);

    if (page > 0) {
        page_left = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png"),
            this, menu_selector(MoreLeaderboards::onPageLeft)
        );
        page_left->setPosition(-220, 0);
        menu->addChild(page_left);
    }

    if (end_count < total_count) {
        auto sprite = CCSprite::createWithSpriteFrameName("GJ_arrow_02_001.png");
        sprite->setFlipX(true);
        page_right = CCMenuItemSpriteExtra::create(
            sprite, this, menu_selector(MoreLeaderboards::onPageRight)
        );
        page_right->setPosition(220, 0);
        menu->addChild(page_right);
    }

    addChild(menu);
    loadTabPageButtons();
}

void MoreLeaderboards::loadTabPageButtons() {
    if (tab_page_left != nullptr)  { tab_page_left->removeFromParentAndCleanup(true);  tab_page_left = nullptr; }
    if (tab_page_right != nullptr) { tab_page_right->removeFromParentAndCleanup(true); tab_page_right = nullptr; }
    if (m_search != nullptr)       { m_search->removeFromParentAndCleanup(true);       m_search = nullptr; }
    if (mod_filter_btn != nullptr) { mod_filter_btn->removeFromParentAndCleanup(true); mod_filter_btn = nullptr; }
    if (mod_filter_type_btn != nullptr) { mod_filter_type_btn->removeFromParentAndCleanup(true); mod_filter_type_btn = nullptr; }

    if (tab_page > 0) {
        auto spr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
        spr->setScale(0.6f);
        tab_page_left = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreLeaderboards::onTabPageLeft));
        tab_page_left->setPosition(-184, 138);
        m_menu->addChild(tab_page_left);
    }

    if (tab_page < 2) {
        auto spr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
        spr->setScale(0.6f);
        spr->setFlipX(true);
        tab_page_right = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreLeaderboards::onTabPageRight));
        tab_page_right->setPosition(184, 138);
        m_menu->addChild(tab_page_right);
    }

    m_search = CCMenuItemSpriteExtra::create(
        CCSprite::createWithSpriteFrameName("gj_findBtn_001.png"),
        this, menu_selector(MoreLeaderboards::onSearch)
    );
    m_search->setPosition(239, 104);
    m_menu->addChild(m_search);

    auto mod_filter_spr = CCSprite::createWithSpriteFrameName("modBadge_01_001.png");
    mod_filter_spr->setScale(1.4f);
    if (!modFilter) mod_filter_spr->setColor({100, 100, 100});
    mod_filter_btn = CCMenuItemSpriteExtra::create(
        mod_filter_spr, this, menu_selector(MoreLeaderboards::onModFilter)
    );
    mod_filter_btn->setPosition(239.f, 69.f);
    m_menu->addChild(mod_filter_btn);

    if (modFilter) {
        CCSprite* mod_filter_type_spr;
        switch (modFilterType) {
            case 0: {
                auto s1 = CCSprite::createWithSpriteFrameName("modBadge_01_001.png"); s1->setScale(1.2f); s1->setPosition({30, 21});
                auto s2 = CCSprite::createWithSpriteFrameName("modBadge_02_001.png"); s2->setScale(1.2f); s2->setPosition({10, 21});
                auto s3 = CCSprite::createWithSpriteFrameName("modBadge_03_001.png"); s3->setScale(1.2f); s3->setPosition({20, 11});
                mod_filter_type_spr = CCSprite::create();
                mod_filter_type_spr->addChild(s1);
                mod_filter_type_spr->addChild(s2);
                mod_filter_type_spr->addChild(s3);
                mod_filter_type_spr->setContentSize({40, 40});
                break;
            }
            case 1: mod_filter_type_spr = CCSprite::createWithSpriteFrameName("modBadge_01_001.png"); mod_filter_type_spr->setScale(1.4f); break;
            case 2: mod_filter_type_spr = CCSprite::createWithSpriteFrameName("modBadge_02_001.png"); mod_filter_type_spr->setScale(1.4f); break;
            default: mod_filter_type_spr = CCSprite::createWithSpriteFrameName("modBadge_03_001.png"); mod_filter_type_spr->setScale(1.4f); break;
        }
        mod_filter_type_btn = CCMenuItemSpriteExtra::create(
            mod_filter_type_spr, this, menu_selector(MoreLeaderboards::onModFilterType)
        );
        mod_filter_type_btn->setPosition(239.f, 34.f);
        m_menu->addChild(mod_filter_type_btn);
    }
}

void MoreLeaderboards::onTabPageLeft(CCObject*) {
    if (loading) return;
    tab_page--;
    changeTabPage();
    page = 0;
    this->onTab(nullptr);
}

void MoreLeaderboards::onTabPageRight(CCObject*) {
    if (loading) return;
    tab_page++;
    changeTabPage();
    page = 0;
    this->onTab(nullptr);
}

void MoreLeaderboards::onModFilter(CCObject*) {
    if (loading) return;
    modFilter = !modFilter;
    modFilterType = 0;
    page = 0;
    this->onTab(nullptr);
}

void MoreLeaderboards::onModFilterType(CCObject*) {
    if (loading) return;
    modFilterType = (modFilterType == 3) ? 0 : modFilterType + 1;
    page = 0;
    this->onTab(nullptr);
}

void MoreLeaderboards::changeTabPage() {
    if (m_search != nullptr)       { m_search->removeFromParentAndCleanup(true); m_search = nullptr; }
    if (m_tab1 != nullptr)         { m_tab1->removeFromParentAndCleanup(true); m_tab1 = nullptr; }
    if (m_tab2 != nullptr)         { m_tab2->removeFromParentAndCleanup(true); m_tab2 = nullptr; }
    if (m_tab3 != nullptr)         { m_tab3->removeFromParentAndCleanup(true); m_tab3 = nullptr; }
    if (m_tab4 != nullptr)         { m_tab4->removeFromParentAndCleanup(true); m_tab4 = nullptr; }
    if (m_tab5 != nullptr)         { m_tab5->removeFromParentAndCleanup(true); m_tab5 = nullptr; }
    if (m_tab6 != nullptr)         { m_tab6->removeFromParentAndCleanup(true); m_tab6 = nullptr; }

    if (auto n = this->getChildByIDRecursive("menu_gdutils_buttons"))
        n->removeFromParentAndCleanup(true);
    tab_page_left = nullptr;
    tab_page_right = nullptr;
    mod_filter_btn = nullptr;
    mod_filter_type_btn = nullptr;

    m_menu = CCMenu::create();
    m_menu->setID("menu_gdutils_buttons");
    m_menu->setZOrder(1);

    auto stars_sprite       = CCSprite::createWithSpriteFrameName("star_small01_001.png");
    auto moons_sprite       = CCSprite::createWithSpriteFrameName("moon_small01_001.png");
    auto diamond_sprite     = CCSprite::createWithSpriteFrameName("diamond_small01_001.png");
    auto usercoins_sprite   = CCSprite::createWithSpriteFrameName("GJ_coinsIcon2_001.png");
    auto demons_sprite      = CCSprite::createWithSpriteFrameName("GJ_demonIcon_001.png");
    auto creators_sprite    = CCSprite::createWithSpriteFrameName("GJ_hammerIcon_001.png");
    auto better_progression = CCSprite::create("better_progression.png"_spr);
    auto pointercrate       = CCSprite::createWithSpriteFrameName("diffIcon_06_btn_001.png");
    auto pointercrate_text  = CCLabelBMFont::create("PC", "goldFont.fnt");
    pointercrate_text->setPosition(28, 0);
    pointercrate_text->setScale(0.5f);
    pointercrate->addChild(pointercrate_text);

    auto makeClassicSpr = [](const char* base) {
        auto classic = CCSprite::createWithSpriteFrameName("star_small01_001.png");
        classic->setPositionX(30); classic->setScale(1.2f);
        auto spr = CCSprite::createWithSpriteFrameName(base);
        spr->addChild(classic);
        return spr;
    };
    auto makePlatformerSpr = [](const char* base) {
        auto moon = CCSprite::createWithSpriteFrameName("moon_small01_001.png");
        moon->setPositionX(30); moon->setScale(1.2f);
        auto spr = CCSprite::createWithSpriteFrameName(base);
        spr->addChild(moon);
        return spr;
    };

    auto addTab = [&](CCMenuItemToggler*& slot, CCNode* icon, StatsListType type, float x) {
        slot = NewTabButton::create(TabBaseColor::Unselected, TabBaseColor::Selected, icon, this, menu_selector(MoreLeaderboards::onTab));
        slot->setPosition(x, 132);
        slot->setTag(static_cast<int>(type));
        slot->setZOrder(30);
        slot->setScale(0.8f);
        m_menu->addChild(slot);
    };

    switch (tab_page) {
        case 0:
            MoreLeaderboards::g_tab = StatsListType::Stars;
            addTab(m_tab1, stars_sprite,     StatsListType::Stars,     -140.f);
            addTab(m_tab2, moons_sprite,     StatsListType::Moons,      -86.f);
            addTab(m_tab3, usercoins_sprite, StatsListType::UserCoins,  -30.f);
            addTab(m_tab4, demons_sprite,    StatsListType::Demons,      26.f);
            addTab(m_tab5, diamond_sprite,   StatsListType::Diamonds,    82.f);
            addTab(m_tab6, creators_sprite,  StatsListType::Creators,   138.f);
            break;
        case 1:
            if (geode::Loader::get()->isModLoaded("itzkiba.better_progression")) {
                MoreLeaderboards::g_tab = StatsListType::BetterProgression;
                addTab(m_tab1, better_progression, StatsListType::BetterProgression, -140.f);
            } else {
                MoreLeaderboards::g_tab = StatsListType::Pointercrate;
                addTab(m_tab1, pointercrate, StatsListType::Pointercrate, -140.f);
            }
            addTab(m_tab2, makeClassicSpr("diffIcon_07_btn_001.png"), StatsListType::classicDemonsEasy,    -86.f);
            addTab(m_tab3, makeClassicSpr("diffIcon_08_btn_001.png"), StatsListType::classicDemonsMedium,  -30.f);
            addTab(m_tab4, makeClassicSpr("diffIcon_06_btn_001.png"), StatsListType::classicDemonsHard,     26.f);
            addTab(m_tab5, makeClassicSpr("diffIcon_09_btn_001.png"), StatsListType::classicDemonsInsane,   82.f);
            addTab(m_tab6, makeClassicSpr("diffIcon_10_btn_001.png"), StatsListType::classicDemonsExtreme, 138.f);
            break;
        case 2:
            MoreLeaderboards::g_tab = StatsListType::platformerDemonsEasy;
            addTab(m_tab1, makePlatformerSpr("diffIcon_07_btn_001.png"), StatsListType::platformerDemonsEasy,    -140.f);
            addTab(m_tab2, makePlatformerSpr("diffIcon_08_btn_001.png"), StatsListType::platformerDemonsMedium,   -86.f);
            addTab(m_tab3, makePlatformerSpr("diffIcon_06_btn_001.png"), StatsListType::platformerDemonsHard,     -30.f);
            addTab(m_tab4, makePlatformerSpr("diffIcon_09_btn_001.png"), StatsListType::platformerDemonsInsane,    26.f);
            addTab(m_tab5, makePlatformerSpr("diffIcon_10_btn_001.png"), StatsListType::platformerDemonsExtreme,   82.f);
            if (geode::Loader::get()->isModLoaded("itzkiba.better_progression"))
                addTab(m_tab6, pointercrate, StatsListType::Pointercrate, 138.f);
            break;
    }

    this->addChild(m_menu);
}

void MoreLeaderboards::onPageLeft(CCObject*) {
    if (loading) return;
    MoreLeaderboards::username = "";
    MoreLeaderboards::scroll_int = 0;
    loading = true;
    loading_circle = LoadingCircle::create();
    loading_circle->setParentLayer(this);
    loading_circle->setZOrder(25);
    loading_circle->show();
    if (displayedData) { displayedData->release(); displayedData = CCArray::create(); displayedData->retain(); }
    page--;
    resetInfos();
    if (loading) { startLoadingMore(); loadPageMore(); }
}

void MoreLeaderboards::onPageRight(CCObject*) {
    if (loading) return;
    MoreLeaderboards::username = "";
    MoreLeaderboards::scroll_int = 0;
    loading = true;
    loading_circle = LoadingCircle::create();
    loading_circle->setZOrder(25);
    loading_circle->setParentLayer(this);
    loading_circle->show();
    if (displayedData) { displayedData->release(); displayedData = CCArray::create(); displayedData->retain(); }
    page++;
    resetInfos();
    if (loading) { startLoadingMore(); loadPageMore(); }
}

void MoreLeaderboards::resetInfos() {
    if (page_label != nullptr)          { page_label->removeFromParentAndCleanup(true); page_label = nullptr; }
    if (page_left != nullptr)           { page_left->removeFromParentAndCleanup(true); page_left = nullptr; }
    if (page_right != nullptr)          { page_right->removeFromParentAndCleanup(true); page_right = nullptr; }
    if (trophy != nullptr)              { trophy->removeFromParentAndCleanup(true); trophy = nullptr; }
    if (mod_filter_btn != nullptr)      { mod_filter_btn->removeFromParentAndCleanup(true); mod_filter_btn = nullptr; }
    if (mod_filter_type_btn != nullptr) { mod_filter_type_btn->removeFromParentAndCleanup(true); mod_filter_type_btn = nullptr; }
}

void MoreLeaderboards::onTab(CCObject* pSender) {
    if (loading) return;
    loading = true;

    loading_circle = LoadingCircle::create();
    loading_circle->setZOrder(25);
    loading_circle->setParentLayer(this);
    loading_circle->show();

    if (displayedData) { displayedData->release(); displayedData = CCArray::create(); displayedData->retain(); }
    if (pSender) MoreLeaderboards::g_tab = static_cast<StatsListType>(pSender->getTag());
    resetInfos();
    page = 0;

    auto toggleTab = [this](CCMenuItemToggler* member) {
        auto isSelected = member->getTag() == static_cast<int>(MoreLeaderboards::g_tab);
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

    if (m_tab1) toggleTab(m_tab1);
    if (m_tab2) toggleTab(m_tab2);
    if (m_tab3) toggleTab(m_tab3);
    if (m_tab4) toggleTab(m_tab4);
    if (m_tab5) toggleTab(m_tab5);
    if (m_tab6) toggleTab(m_tab6);

    if (loading) { startLoadingMore(); loadPageMore(); }
}
