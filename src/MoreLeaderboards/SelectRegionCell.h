#pragma once
#include "SelectRegion.h"
#include <Geode/Bindings.hpp>

class SelectRegionCell : public cocos2d::CCLayer {
    std::function<void(std::string)> selectedRegion;
    std::string id_ctr;

    void onClick(cocos2d::CCObject* pSender);
public:
    SelectRegionCell(const std::function<void(std::string)>& callback) : selectedRegion(callback) {}

    static std::string country_id;
    bool init(std::string mod);
    static SelectRegionCell* create(std::string mod, const std::function<void(std::string)>& callback);
};