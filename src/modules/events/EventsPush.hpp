#pragma once
#include <defs/geode.hpp>
#include <arc/sync/mpsc.hpp>
#include "../../include/RateEvent.hpp"

class EventsPush : public CCLayer {
    protected:
        bool m_alreadyDead = false;
        GJGameLevel* level = GJGameLevel::create();
        EventType eventType = EventType::NA;
        virtual bool init(EventData data);
        void destroySelf() {
            m_alreadyDead = true;
            auto scene = CCDirector::get()->getRunningScene();
            if (scene->getChildByID("EventsPush"_spr) != nullptr) {
                scene->removeChildByID("EventsPush"_spr);
            } 
            eventCompletedCallback(scene);
        }
    public:
        void onExit() override {
            CCLayer::onExit();
            if (!m_alreadyDead) {
                callCancel();
            }
        }
        void onClickBtn(CCObject*);
        static void pushRateLevel(CCScene* self, std::string data) {
            auto jsonStr = matjson::parse(data);
            if (jsonStr.isErr()) {
                log::error("Couldn't parse JSON string: {}", jsonStr.unwrapErr());
            }
            auto eventData = matjson::Serialize<EventData>::fromJson(jsonStr.unwrapOrDefault());
            if (eventData.isErr()) {
                log::error("Couldn't parse JSON to EventData: {}", eventData.unwrapErr());
            }
            pushRateLevel(
                self,
                eventData.unwrapOrDefault()
            );
        }
        static void pushRateLevel(CCScene* self, EventData data);
        static void processNextEvent(CCScene* self);
        static void eventCompletedCallback(CCScene* self);
        static void callCancel();
        static std::vector<std::string> getWords(std::string s, std::string delim);
        static bool canSendEvent(CCScene* self);
        static EventsPush* create(EventData data) {
            auto ret = new EventsPush();
            if (ret && ret->init(data)) {
                ret->autorelease();
                return ret;
            }
            CC_SAFE_DELETE(ret);
            return nullptr;
        }
        static void setTx(arc::mpsc::Sender<bool> tx);
};
