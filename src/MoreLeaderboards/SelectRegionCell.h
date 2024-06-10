#pragma once
#include "SelectRegion.h"
#include <Geode/Bindings.hpp>

class SelectRegionCell : public cocos2d::CCLayer {
public:
    bool init(std::string mod);
    static SelectRegionCell* create(std::string mod);
};