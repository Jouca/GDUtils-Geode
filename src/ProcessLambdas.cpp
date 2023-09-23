#include "ProcessLambdas.h"
cocos2d::CCLayer* queueLayer = nullptr;
cocos2d::SEL_MenuHandler queueCallback = nullptr;
bool startCallbackQueue = false;
void ProcessLambdas::callMenuHandler(cocos2d::CCLayer* self, cocos2d::SEL_MenuHandler callback) {
    startCallbackQueue = true;
    queueLayer = self;
    queueCallback = callback;
}
void ProcessLambdas::processMenuHandler() {
    startCallbackQueue = false;
    (queueLayer->*queueCallback)(queueLayer);
    queueLayer = nullptr;
    queueCallback = nullptr;
}
bool ProcessLambdas::shouldProcessMenuHandler() {
    return startCallbackQueue;
}
MenuHandlerResponse ProcessLambdas::getMenuHandler() {
    MenuHandlerResponse response;
    response.m_queueLayer = queueLayer;
    response.m_queueCallback = queueCallback;
    return response;
}
