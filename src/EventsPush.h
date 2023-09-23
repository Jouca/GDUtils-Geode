#pragma once
#include <Geode/Geode.hpp>
#include <Geode/loader/Mod.hpp>
#include "includes.h"
using namespace geode::prelude;
class EventsPush : public CCLayer {
    protected:
        GJGameLevel* level = GJGameLevel::create();
        virtual bool init(sio::message::ptr const& data);
    public:
        void destroySelf();
        void onClickBtn(CCObject*);
        void onLoadFinished(cocos2d::extension::CCHttpClient* client, cocos2d::extension::CCHttpResponse* response);
        static EventsPush* create(sio::message::ptr const& data);
        static void pushRateLevel(CCScene* self, sio::message::ptr const& data);
        static void processNextEvent(CCScene* self);
        static void eventCompletedCallback(CCScene* self);
        static void stopNow(CCScene* scene);
};
