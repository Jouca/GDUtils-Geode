#pragma once
#include <Geode/Geode.hpp>
#include <Geode/loader/Mod.hpp>
#include "../includes.h"
using namespace geode::prelude;
enum EventType {
    Rate,
    Daily,
    Weekly,
    smallChest,
    largeChest,
    List,
    NA
};

class EventsPush : public CCLayer {
    protected:
        GJGameLevel* level = GJGameLevel::create();
        #ifdef GEODE_IS_MACOS
        int levelId = 0; // because for some reason GJGameLevel doesnt even work :(
        #endif
        EventType eventType = EventType::NA;
        virtual bool init(sio::message::ptr const& data);
    public:
        void destroySelf();
        void onClickBtn(CCObject*);
        static EventsPush* create(sio::message::ptr const& data);
        static void pushRateLevel(CCScene* self, sio::message::ptr const& data);
        static void processNextEvent(CCScene* self);
        static void eventCompletedCallback(CCScene* self);
        static void stopNow(CCScene* scene);
        static GJGameLevel* convertLevelToJSON(std::string& data);
};
