#include "SelectRegion.h"

static std::unordered_map<std::string, web::WebTask> RUNNING_REQUESTS {};
std::vector<std::string> SelectRegion::displayedData = {};

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

void SelectRegion::scene(const std::function<void(std::string)>& callback) {
    auto popup = new SelectRegion(callback);

    if (popup && popup->initAnchored(250.0f, 210.0f)) {
        popup->autorelease();
        CCDirector::sharedDirector()->getRunningScene()->addChild(popup);
    } else {
        CC_SAFE_DELETE(popup);
    }
};

bool SelectRegion::setup() {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    auto director = cocos2d::CCDirector::sharedDirector();

    setTouchEnabled(true);
    setKeypadEnabled(true);

    SelectRegion::loadPage();
    return true;
}

void SelectRegion::loadPage() {
    CCArray* data = CCArray::create();

    for (std::string const& mod : SelectRegion::displayedData) {
        auto cell = SelectRegionCell::create(mod, [this](std::string id) {
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
