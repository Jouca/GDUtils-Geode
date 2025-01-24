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

        if (!Mod::get()->template getSettingValue<bool>("eventLevelInfo")) return true;

        auto infoSpr = CCSprite::createWithSpriteFrameName("info_btn.png"_spr);
        auto infoBtn = CCMenuItemSpriteExtra::create(infoSpr, this, menu_selector(EventPage::onInfo));

        // Put on the corner top left of the background
        auto menu = as<CCMenu*>(this->m_mainLayer->getChildByIDRecursive("main-menu"));
        if (menu) {
            infoBtn->setPosition({ 14, -194 });
            menu->addChild(infoBtn);
        }

        return true;
    }
};

void EventLevelInformation::scene() {
    auto popup = new EventLevelInformation();

    if (popup && popup->initAnchored(400.0f, 225.0f, "GJ_square02.png")) {
        popup->autorelease();
        CCDirector::sharedDirector()->getRunningScene()->addChild(popup, 1000);
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
    // Make loading circle
    loading_circle = LoadingCircle::create();
    loading_circle->setZOrder(25);
    loading_circle->setParentLayer(this);
    loading_circle->show();

    // Load the image
    std::string URL = "https://clarifygdps.com/gdutils/event_info/image_banner.png?randome=" + std::to_string(static_cast<unsigned int>(time(nullptr)));

    auto req = web::WebRequest();
    m_downloadListener.bind([this](web::WebTask::Event* e){
        if (auto res = e->getValue()){
            if (!res->ok()) {
                onDownloadFail();
            } else {
                auto data = res->data();
                std::thread imageThread = std::thread([data,this](){
                    m_image = new CCImage();
                    m_image->initWithImageData(const_cast<uint8_t*>(data.data()),data.size());
                    geode::Loader::get()->queueInMainThread([this](){
                        imageCreationFinished(m_image);
                        m_image->release();
                    });
                });
                imageThread.detach();
            }
        }
    });

    auto downloadTask = req.header("Cache-Control", "no-cache").header("Pragma", "no-cache").get(URL);
    m_downloadListener.setFilter(downloadTask);
}

void EventLevelInformation::imageCreationFinished(CCImage* image) {
    fadeLoadingCircle();

    auto size = this->m_mainLayer->getContentSize();
    auto texture = new CCTexture2D();
    texture->initWithImage(image);
    auto spr = CCSprite::createWithTexture(texture);
    texture->autorelease();
    spr->setPosition({ size.width / 2, size.height / 2 });
    spr->setScale(1.1f);

    this->m_mainLayer->addChild(spr);
}

void EventLevelInformation::onDownloadFail() {
    fadeLoadingCircle();

    auto size = this->m_mainLayer->getContentSize();
    auto failLabel = CCLabelBMFont::create("No event banner found.", "bigFont.fnt", 0.0f, kCCTextAlignmentCenter);
    failLabel->setPosition({ size.width / 2, size.height / 2 });
    failLabel->setScale(0.7f);

    this->m_mainLayer->addChild(failLabel);
}

void EventLevelInformation::fadeLoadingCircle() {
    if (loading_circle == nullptr) return;
    loading_circle->fadeAndRemove();
};
