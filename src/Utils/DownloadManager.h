#pragma once
// windows only because i cant get this to work, actually im too lazy
#ifndef GEODE_IS_MACOS
#include <Geode/Geode.hpp>
using namespace geode::prelude;
class ProgressBar : public CCNode {
    protected:
        float m_pPercentage = 0;
        CCSprite* outerBg = CCSprite::create("slidergroove.png");
        CCSprite* progressBar = CCSprite::create("sliderBar.png");
        bool init();
    public:
        void testProgressbar();
        virtual void setProgress(float percentage);
        static ProgressBar* create();
};
class DownloadManager : public geode::Popup<const char*, const char*, cocos2d::SEL_MenuHandler> {
    LoadingCircle* loading_circle = LoadingCircle::create();
    protected:
        bool setup(const char* url, const char* destination, cocos2d::SEL_MenuHandler selector);
        ProgressBar* m_progressBar;
    public:
        static constexpr const float s_defWidth = 300.0f;
        static constexpr const float s_defHeight = 180.0f;
        static DownloadManager* create(const char* url, const char* destination, cocos2d::SEL_MenuHandler callback); //, cocos2d::SEL_MenuHandler callback
        
        // Downloader class
        int progress_func(void*, double, std::optional<float>, double, double);
};
#endif
