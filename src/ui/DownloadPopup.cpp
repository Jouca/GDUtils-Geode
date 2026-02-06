#include "DownloadPopup.hpp"
#ifndef GEODE_IS_MACOS // unfortunately figs video player not supported on mac becuase opengl32 doesnt want to compile correctly koeirjnfioewbnfejwf
#include <math.h>
#include <Geode/utils/web.hpp>
#include <Geode/utils/file.hpp>
#include <defs/geode.hpp>

// ------
// 204 = MAX
// 0 = MIN
bool RobProgressBar::init() { // on purpose im not even providing any params because why would we!?;
    progressBar->setPosition({3, 8});
    progressBar->setAnchorPoint({ 0, 0.5 });
    progressBar->setTextureRect(CCRect(0, 0, 204, 8));
    ccTexParams params = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT }; // tf is this nonsense mat
    progressBar->getTexture()->setTexParameters(&params);
    this->addChild(outerBg);
    progressBar->setZOrder(-1);
    outerBg->addChild(progressBar);
    return true;
}

void RobProgressBar::setProgress(float percentage) {
    this->m_pPercentage = percentage;
    float realPercentage = (204/100) * this->m_pPercentage; // "Result of integer division used in a floating point" ok i know clangd warnings wow
    progressBar->setTextureRect(CCRect(0, 0, realPercentage, 8));
}

RobProgressBar* RobProgressBar::create() {
    auto pRet = new RobProgressBar();
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
}

bool DownloadPopup::init(const char* url, std::filesystem::path destination, std::function<void()> callback) {
    if (!Popup::init(300.f, 150.f)) return false;
    Build<CCLabelBMFont>::create("Downloading...", "bigFont.fnt").scale(0.8f).parentAtPos(m_mainLayer, Anchor::Top, {0, -25});

    loading_circle->setParentLayer(this);
    loading_circle->setScale(0.675f);
    loading_circle->show();
    m_progressBar = RobProgressBar::create();
    m_progressBar->setProgress(0.0F);
    m_mainLayer->addChildAtPosition(m_progressBar, Anchor::Center, {0, -30});

    Build<CCLabelBMFont>::create("Do not close this menu while the file is being downloaded.", "chatFont.fnt").scale(.5f).alignment(cocos2d::kCCTextAlignmentCenter).parentAtPos(m_mainLayer, Anchor::Bottom, {0, 18});
    geode::utils::web::WebRequest req = web::WebRequest();
    req.onProgress([this](web::WebProgress const& progress) {
        if (!progress.downloadProgress().has_value()) return;
        log::debug("Progress({},{},{})", progress.downloadProgress(), progress.downloaded(), progress.downloadTotal());
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        if (m_progressBar != nullptr) {
            m_progressBar->setProgress(progress.downloadProgress().value());
        }
    });
    m_listener.spawn(
        req.get(url),
        [this, destination, callback](web::WebResponse value) {
            (void)geode::utils::file::writeBinarySafe(destination, value.data());
            onClose(nullptr);
            callback();
        }
    );
    return true;
}

DownloadPopup* DownloadPopup::create(const char* url, std::filesystem::path destination, std::function<void()> callback) {
    auto ret = new DownloadPopup();
    if (ret && ret->init(url, destination, callback)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}
// below is copied from the gdr mod lol
#endif
