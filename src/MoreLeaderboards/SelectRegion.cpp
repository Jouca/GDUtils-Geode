#include "SelectRegion.h"

static std::unordered_map<std::string, web::WebTask> RUNNING_REQUESTS {};

std::vector<std::string> SelectRegion::getWords(std::string s, std::string delim) {
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

void SelectRegion::scene(const std::function<void(int)>& callback) {
    auto popup = new SelectRegion(callback);

    if (popup && popup->init(250.0f, 210.0f)) {
        popup->autorelease();
        CCDirector::sharedDirector()->getRunningScene()->addChild(popup);
    } else {
        CC_SAFE_DELETE(popup);
    }
};

bool SelectRegion::setup() {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    auto director = cocos2d::CCDirector::sharedDirector();

    registerWithTouchDispatcher();
    setTouchEnabled(true);
    setKeypadEnabled(true);

    startLoading();
    return true;
}

void SelectRegion::fadeLoadingCircle() {
    if (loading_circle == nullptr) return;
    loading_circle->fadeAndRemove();
};

void SelectRegion::startLoading() {
    loading_circle = LoadingCircle::create();
    loading_circle->setParentLayer(this);
    loading_circle->show();

    geode::utils::web::WebRequest request = web::WebRequest();
    const geode::utils::MiniFunction<void(std::string const&)> then = [this](std::string const& data) {
        fadeLoadingCircle();
        handle_request(data);
    };
    const geode::utils::MiniFunction<void(std::string const&)> expect = [this](std::string const& error) {
        fadeLoadingCircle();
    };

    RUNNING_REQUESTS.emplace(
        "@loaderMoreLeaderboardRegionGet",
        request.get("https://clarifygdps.com/gdutils/moreleaderboards_region.php").map(
            [expect = std::move(expect), then = std::move(then)](web::WebResponse* response) {
                if (response->ok()) {
                    then(response->string().value());
                } else {
                    expect("An error occured while sending a request on our server. Please try again later.");
                }

                RUNNING_REQUESTS.erase("@loaderMoreLeaderboardRegionGet");
                return *response;
            }
        )
    );
};

void SelectRegion::loadPage() {
    CCArray* data = CCArray::create();

    for (std::string const& mod : displayedData) {
        auto cell = SelectRegionCell::create(mod, [this](int id) {
            selectedRegion(id);
            removeFromParent();
        });
        data->addObject(cell);
    }

    auto border = OldBorder::create(ListView::create(data, 30, 218, 178), { 0xBF, 0x72, 0x3E, 0xFF }, {220, 180}, {1, 1});
    border->setPosition(m_mainLayer->getContentSize() / 2 - border->getContentSize() / 2);
    setTouchPriority(100);
    m_mainLayer->addChild(border);
}

void SelectRegion::handle_request(std::string const& data) {
    if (data != "-1") {
        displayedData = getWords(data, "|");
        loadPage();
    }
}