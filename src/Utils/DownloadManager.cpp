#include "DownloadManager.h"
#ifndef GEODE_IS_MACOS // unfortunately figs video player not supported on mac becuase opengl32 doesnt want to compile correctly koeirjnfioewbnfejwf
#include "../ProcessLambdas.h"
#include <math.h>
#include <Geode/utils/web.hpp>
#include <Geode/utils/file.hpp>

EventListener<web::WebTask> m_listener_dl;

// ------
// 204 = MAX
// 0 = MIN
bool ProgressBar::init() { // on purpose im not even providing any params because why would we!?;
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

void ProgressBar::setProgress(float percentage) {
    this->m_pPercentage = percentage;
    float realPercentage = (204/100) * this->m_pPercentage; // "Result of integer division used in a floating point" ok i know clangd warnings wow
    progressBar->setTextureRect(CCRect(0, 0, realPercentage, 8));
}

ProgressBar* ProgressBar::create() {
    auto pRet = new ProgressBar();

    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return nullptr;
}
// ------
// credits https://stackoverflow.com/a/1639047
int DownloadManager::progress_func(void*, double totalDownload, std::optional<float> downloadAmount, double TotalToUpload, double NowUploaded) {
    if (totalDownload <= 0.0 || !downloadAmount) return 0;
    float amountDownloaded = round((downloadAmount.value() / totalDownload) * 100);
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    if (m_progressBar != nullptr) {
        m_progressBar->setProgress(amountDownloaded);
    }
    return 0;
}

bool DownloadManager::setup(const char* url, const char* destination, cocos2d::SEL_MenuHandler selector) {
    auto title = CCLabelBMFont::create("Downloading...", "bigFont.fnt");
    title->setScale(0.8f);
    m_mainLayer->addChildAtPosition(title, Anchor::Top, {0, -25});

    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    loading_circle->setParentLayer(this);
    loading_circle->setScale(0.675f);
    loading_circle->show();
    m_progressBar = ProgressBar::create();
    m_progressBar->setProgress(0.0F);
    m_mainLayer->addChildAtPosition(m_progressBar, Anchor::Center, {0, -30});

    auto text = cocos2d::CCLabelBMFont::create("Do not close this menu while the file is being downloaded.", "chatFont.fnt");
    text->setAlignment(cocos2d::kCCTextAlignmentCenter);
    text->setScale(.5F);
    m_mainLayer->addChildAtPosition(text, Anchor::Bottom, {0, 18});

    // def not copied from geode hahahaha
    const std::function<void(geode::ByteVector const&)> then = [this, destination, selector](geode::ByteVector const& data) {
        // Save the file
        (void)geode::utils::file::writeBinary(destination, data);

        ProcessLambdas::callMenuHandler(this, selector);
        this->onClose(nullptr);
    };
    const std::function<void(int const&)> expect = [this](int const& error) {
        FLAlertLayer::create(nullptr, "Error!", "An error occured while trying to send a request to the servers. Check <cy>logs</c> for more information.", "OK", nullptr, 200.0F)->show();
        log::error("Error downloading: {}", error);
    };

    geode::utils::web::WebRequest request = web::WebRequest();
    m_listener_dl.bind([this, expect = std::move(expect), then = std::move(then)] (web::WebTask::Event* e) {
        if (web::WebProgress* progress = e->getProgress()) {
            progress_func(NULL, 100, progress->downloadProgress(), progress->uploadTotal(), progress->uploaded());
        } else if (web::WebResponse* result = e->getValue()) {
            if (result->ok()) {
                then(result->data());
            } else {
                expect(result->code());
            }
        }
    });

    m_listener_dl.setFilter(request.get(url));
    return true;
}

DownloadManager* DownloadManager::create(const char* url, const char* destination, cocos2d::SEL_MenuHandler selector) {
    auto ret = new DownloadManager();
    if (ret->initAnchored(300.f, 150.f, url, destination, selector)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}
// below is copied from the gdr mod lol
#endif
