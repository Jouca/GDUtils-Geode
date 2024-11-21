#include "EventLevelInformation.h"
#include "Geode/Enums.hpp"
#include <Geode/modify/DailyLevelPage.hpp>

class $modify(EventPage, DailyLevelPage) {
    void onInfo(CCObject* pSender) {
        EventLevelInformation::scene();
    }

    bool init(GJTimedLevelType type) {
        if (!DailyLevelPage::init(type)) return false;

        if (type != GJTimedLevelType::Event) return true;

        auto infoSpr = CCSprite::createWithSpriteFrameName("info_btn.png"_spr);
        auto infoBtn = CCMenuItemSpriteExtra::create(infoSpr, this, menu_selector(EventPage::onInfo));

        // Put on the corner top left of the background
        auto levelCell = as<CCLayer*>(this->getChildByIDRecursive("main-layer"));
        auto menu = as<CCMenu*>(levelCell->getChildByIDRecursive("main-menu"));
        infoBtn->setPosition({ -292, -56 });

        menu->addChild(infoBtn);

        return true;
    }
};

void EventLevelInformation::scene() {
    auto popup = new EventLevelInformation();

    if (popup && popup->initAnchored(400.0f, 225.0f, "GJ_square02.png")) {
        popup->autorelease();
        CCDirector::sharedDirector()->getRunningScene()->addChild(popup, 200);
    } else {
        CC_SAFE_DELETE(popup);
    }
};

bool EventLevelInformation::setup() {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    auto director = cocos2d::CCDirector::sharedDirector();

    registerWithTouchDispatcher();
    setTouchEnabled(true);
    setKeypadEnabled(true);

    EventLevelInformation::loadPage();
    return true;
}

void EventLevelInformation::loadPage() {
    auto image_spr = CCSprite::createWithSpriteFrameName("ncs_album.png"_spr);

    auto size = this->m_mainLayer->getContentSize();

    image_spr->setPosition({ size.width / 2, size.height / 2 });
    image_spr->setScale(1.1f);
    this->m_mainLayer->addChild(image_spr);
}