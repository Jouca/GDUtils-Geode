#pragma once
#include <defs/geode.hpp>

class EventInfoPopup : public Popup {
    protected:
        LazySprite* m_sprite;
        async::TaskHolder<Result<ByteVector, std::string>> m_listener;
        Ref<CCImage> m_image;

        bool init() override;
        void loadPage();
        void imageCreationFinished(Result<>);
        void onDownloadFail();
        arc::Future<Result<ByteVector, std::string>> reqTask();
        void onFetch(Result<ByteVector, std::string>);
    public:
        static EventInfoPopup* create();
        virtual void onClose(CCObject*) override {
            m_listener.cancel();
            Popup::onClose(nullptr);
        }
};
