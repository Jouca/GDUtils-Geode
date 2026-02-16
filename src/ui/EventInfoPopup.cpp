#include "EventInfoPopup.hpp"

EventInfoPopup* EventInfoPopup::create() {
    auto ret = new EventInfoPopup();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

arc::Future<Result<ByteVector, std::string>> EventInfoPopup::reqTask() {
    ARC_FRAME();
    auto req = web::WebRequest();
    req.header("Cache-Control", "no-cache").header("Pragma", "no-cache");

    std::string URL = "https://clarifygdps.com/gdutils/event_info/image_banner.png?randome=" + std::to_string(static_cast<unsigned int>(time(nullptr)));
    //std::string URL = "https://clarifygdps.com/gdutils/event_info/image_banner_oldNCS.png";

    auto response = co_await req.get(URL);

    if (response.ok()) {
        co_return Ok(std::move(response).data());
    }

    co_return Err(response.errorMessage());
}

bool EventInfoPopup::init() {
    if (!Popup::init(400.0f, 225.0f, "GJ_square02.png")) return false;

    m_sprite = LazySprite::create(this->m_mainLayer->getContentSize());
    m_sprite->setScale(.55f);
    this->m_mainLayer->addChildAtPosition(m_sprite, Anchor::Center);

    m_sprite->setLoadCallback([this](Result<> res) {
        this->imageCreationFinished(std::move(res));
    });

    m_listener.spawn(EventInfoPopup::reqTask(), [this](auto result) {
        onFetch(std::move(result));
    });
    return true;
}

void EventInfoPopup::imageCreationFinished(Result<> res) {
    if (!res) {
        log::debug("Failed to load image: {}", res.err().value_or(std::string{}));
        this->onDownloadFail();
        return;
    }
}

void EventInfoPopup::onDownloadFail() {
    auto failLabel = CCLabelBMFont::create("No event banner found.", "bigFont.fnt", 0.0f, kCCTextAlignmentCenter);
    m_sprite->cancelLoad();
    failLabel->setScale(0.7f);
    this->m_mainLayer->addChildAtPosition(failLabel, Anchor::Center);
}

void EventInfoPopup::onFetch(Result<ByteVector, std::string> result) {
    if (result.isErr()) {
        log::error("Error when fetching image: {}", result.unwrapErr());
        this->onDownloadFail();
    } else {
        m_sprite->loadFromData(std::move(result).unwrap());
    }
}
