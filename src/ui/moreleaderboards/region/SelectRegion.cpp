#include "SelectRegion.hpp"

std::vector<std::string> SelectRegion::displayedData = {};

std::vector<std::string> SelectRegion::getWords(std::string s, std::string delim) {
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

SelectRegion* SelectRegion::create(const std::function<void(std::string)>& callback) {
    auto ret = new SelectRegion(callback);
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void SelectRegion::scene(const std::function<void(std::string)>& callback) {
    if (auto popup = SelectRegion::create(callback)) {
        CCDirector::sharedDirector()->getRunningScene()->addChild(popup);
    }
}

bool SelectRegion::init() {
    if (!Popup::init(250.0f, 210.0f)) return false;
    setTouchEnabled(true);
    setKeypadEnabled(true);
    loadPage();
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

    auto border = OldBorder::create(ListView::create(data, 30, 218, 178), {0xBF, 0x72, 0x3E, 0xFF}, {220, 180}, {1, 1});
    border->setPosition(m_mainLayer->getContentSize() / 2 - border->getContentSize() / 2);
    setTouchPriority(100);
    m_mainLayer->addChild(border);
}
