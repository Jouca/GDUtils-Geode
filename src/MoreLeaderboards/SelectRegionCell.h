#pragma once
#include "SelectRegion.h"
#include <Geode/Bindings.hpp>

class SelectRegionCell : public cocos2d::CCLayer {
    std::function<void(int)> selectedRegion;
    int id_ctr;

    void onClick(cocos2d::CCObject* pSender);
public:
    SelectRegionCell(const std::function<void(int)>& callback) : selectedRegion(callback) {}

    static int country_id;
    bool init(std::string mod);
    static SelectRegionCell* create(std::string mod, const std::function<void(int)>& callback);
};