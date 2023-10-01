#include "DownloadManager.h"
#ifdef GEODE_IS_WINDOWS // unfortunately figs video player not supported on mac becuase opengl32 doesnt want to compile correctly koeirjnfioewbnfejwf
#include "ProcessLambdas.h"
#include <math.h>

// ------
// 204 = MAX
// 0 = MIN
bool ProgressBar::init() { // on purpose im not even providing any params because why would we!?
    //progressBar->setPosition({0, 200});
    progressBar->setPosition({3, 8});
    progressBar->setAnchorPoint({ 0, 0.5 });
    //progressBar->setTextureRect(CCRect(0, 0, 200, 8));
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
int DownloadManager::progress_func(void*, double totalDownload, double downloadAmount, double TotalToUpload, double NowUploaded) {
    //std::cout << "progress_func(" << totalDownload << ", " << downloadAmount << ")" << std::endl;
    if (totalDownload <= 0.0) return 0;
    float amountDownloaded = round((downloadAmount / totalDownload) * 100);
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    auto layer = reinterpret_cast<ProgressBar*>(reinterpret_cast<CCLayer*>(reinterpret_cast<CCLayer*>(scene->getChildByTag(6942084))->getChildren()->objectAtIndex(0))->getChildByTag(4592));
    if (layer != nullptr) {
        //std::cout << "Update progress " << amountDownloaded << "%" << std::endl;
        layer->setProgress(amountDownloaded);
    }
    return 0;
}
DWORD WINAPI DownloadManager::curl_thread(void* self) {
    auto layer = reinterpret_cast<DownloadManager*>(self);
    auto response = layer->sendWebRequest(layer->m_sUrl);
    if (response.curlCode != CURLE_OK) {
        log::error("curl not ok. " + std::to_string(response.curlCode));
        layer->error_box1->setVisible(true);
        layer->error_box2->removeFromParentAndCleanup(true);
    } else {
        log::debug("dumpToFile");
        if (layer->dumpToFile(layer->m_sDestination, response.content)) {
            //(layer->*layer->m_pSelector)(layer);
            ProcessLambdas::callMenuHandler(layer, layer->m_pSelector);
            //ConnectionHandler::callSELMenuHandler(layer, layer->m_pSelector);
            layer->error_box1->removeFromParentAndCleanup(true);
            layer->error_box2->removeFromParentAndCleanup(true);
        } else {
            layer->error_box2->setVisible(true);
            layer->error_box1->removeFromParentAndCleanup(true);
        }
        //std::cout << "success, not calling because crash rn lol" << std::endl;
        //gd::FLAlertLayer::create(nullptr, "Success!", "OK", nullptr, 300.0F, "<cy>GDUtils</c> has been updated to the <cg>latest version</c> !\nIf you want to apply the update, restart <cy>Geometry Dash</c>.")->show();
    }
    layer->onClose(CCNode::create());
    return true;
}


void DownloadManager::setup() {
    log::debug("DownloadManager::setup()");
    log::debug(m_sUrl);
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    loading_circle->setParentLayer(this);
    loading_circle->setScale(0.675f);
    loading_circle->setPositionY(20);
    loading_circle->show();
    auto progressBar = ProgressBar::create();
    progressBar->setPosition({ 280, 130 });
    progressBar->setProgress(0.0F);
    progressBar->setTag(4592);
    this->setTag(6942084);
    this->m_mainLayer->addChild(progressBar);

    error_box1->show();
    error_box2->show();
    error_box1->setVisible(false);
    error_box2->setVisible(false);
    auto text = cocos2d::CCLabelBMFont::create("Do not close this menu while the file is being downloaded.", "chatFont.fnt");
    text->setPosition(
        winSize.width / 2,
        winSize.height / 2 - 18
    ); // needs to be 100
    text->setAlignment(cocos2d::kCCTextAlignmentCenter);
    text->setTag(39);
    text->setScale(.5F);
    this->m_mainLayer->addChild(text);
    setTouchEnabled(true);
    CreateThread(NULL, 0, DownloadManager::curl_thread, (void*)this, 0, NULL);
}

DownloadManager* DownloadManager::create(const char* url, const char* destination, cocos2d::SEL_MenuHandler selector) {
    auto pRet = new DownloadManager();
    if (pRet) {
        pRet->m_pSelector = selector;
        pRet->m_sUrl = url;
        pRet->m_sDestination = destination;
        if (pRet->init(DownloadManager::s_defWidth, DownloadManager::s_defHeight, "GJ_square01.png", "Downloading...")) {
            pRet->autorelease();
            return pRet;
        }
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
};
// below is copied from the gdr mod lol
#endif
