#pragma once
#include <Geode/Geode.hpp>
// too lazy to deal with pairs or tuples
struct MenuHandlerResponse {
    cocos2d::CCLayer* m_queueLayer;
    cocos2d::SEL_MenuHandler m_queueCallback;
};

// bad coding ik, dont blame me
class ProcessLambdas {
    public:
        static void callMenuHandler(cocos2d::CCLayer* self, cocos2d::SEL_MenuHandler callback);
        static bool shouldProcessMenuHandler();
        static MenuHandlerResponse getMenuHandler();
        static void processMenuHandler();
};
