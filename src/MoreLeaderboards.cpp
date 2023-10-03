#include "MoreLeaderboards.h"

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
        // Adding soon for more leaderboards
    } else if (type == "mods") {
        startLoadingMods();
        loadPageMods();
    }

    return true;
}

void MoreLeaderboards::backButton(cocos2d::CCObject*) {
    cocos2d::CCDirector::sharedDirector()->popSceneWithTransition(0.5F, cocos2d::PopTransition::kPopTransitionFade);
};

void MoreLeaderboards::fadeLoadingCircle() {
    loading_circle->fadeAndRemove();
};

void MoreLeaderboards::handle_request(std::string const& data) {
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

    web::AsyncWebRequest()
    .fetch("https://clarifygdps.com/gdutils/modslist.php")
    .text()
    .then([this](std::string const& data) {
        fadeLoadingCircle();

        handle_request(data);
    })
    .expect([this](std::string const& error) {
        fadeLoadingCircle();
    });
};

void MoreLeaderboards::loadPageMods() {
    if(listLayer != nullptr) listLayer->removeFromParentAndCleanup(true);

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    leaderboardView = MoreLeaderboardsListView::create(displayedData, 356.f, 220.f);
    listLayer = GJListLayer::create(leaderboardView, "GD Moderators", {191, 114, 62, 255}, 356.f, 220.f);
    listLayer->setPosition(winSize / 2 - listLayer->getScaledContentSize() / 2 - CCPoint(0,5));

    addChild(listLayer);
}