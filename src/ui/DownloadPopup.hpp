#pragma once
// windows only because i cant get this to work, actually im too lazy
#ifndef GEODE_IS_MACOS
#include <Geode/Geode.hpp>
using namespace geode::prelude;
class RobProgressBar : public CCNode {
    protected:
        float m_pPercentage = 0;
        CCSprite* outerBg = CCSprite::create("slidergroove.png");
        CCSprite* progressBar = CCSprite::create("sliderBar.png");
        bool init();
    public:
        void testProgressbar();
        virtual void setProgress(float percentage);
        static RobProgressBar* create();
};
class DownloadPopup : public geode::Popup {
    protected:
        bool init(const char* url, std::filesystem::path, std::function<void()>);
        RobProgressBar* m_progressBar;
        LoadingCircle* loading_circle = LoadingCircle::create();
        async::TaskHolder<web::WebResponse> m_listener;
    public:
        static DownloadPopup* create(const char* url, std::filesystem::path, std::function<void()>); //, cocos2d::SEL_MenuHandler callback
        virtual void onClose(CCObject*) override {
            m_listener.cancel();
            Popup::onClose(nullptr);
        }
};
#endif
