#pragma once
#include <Geode/Geode.hpp>
#include <Geode/loader/Mod.hpp>
#include "../../include/RateEvent.hpp"
#include "../includes.h"
using namespace geode::prelude;

class EventsPush : public CCLayer {
    protected:
        GJGameLevel* level = GJGameLevel::create();
        #ifdef GEODE_IS_MACOS
        int levelId = 0; // because for some reason GJGameLevel doesnt even work :(
        #endif
        EventType eventType = EventType::NA;
        virtual bool init(EventData data);
    public:
        void destroySelf();
        void onClickBtn(CCObject*);
        static EventsPush* create(EventData data);
        static void pushRateLevel(CCScene* self, std::string data);
        static void pushRateLevel(CCScene* self, EventData data);
        static void processNextEvent(CCScene* self);
        static void eventCompletedCallback(CCScene* self);
        static void stopNow(CCScene* scene);
        static GJGameLevel* convertLevelToJSON(std::string& data);
        static std::vector<std::string> getWords(std::string s, std::string delim);
};
