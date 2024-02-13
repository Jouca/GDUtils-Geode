#pragma once
#include <iostream>
#include <Geode/Geode.hpp>
using namespace geode::prelude;

class InfoNewLayer : public CCLayer {
    public:
        void onCopyLevelURL(CCObject* pSender);
};
