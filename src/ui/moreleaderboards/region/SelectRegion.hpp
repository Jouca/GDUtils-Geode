#pragma once
#include <defs/geode.hpp>
#include <Geode/utils/web.hpp>
#include "SelectRegionCell.hpp"
#include "../../OldBorder.hpp"
#include <functional>

class SelectRegion : public Popup {
    LoadingCircle* loading_circle;
    std::function<void(std::string)> selectedRegion;

protected:
    bool init();
    void loadPage();

    cocos2d::CCSize m_sScrLayerSize;
public:
    static std::vector<std::string> displayedData;

    SelectRegion(const std::function<void(std::string)>& callback) : selectedRegion(callback) {}

    static SelectRegion* create(const std::function<void(std::string)>& callback);
    static void scene(const std::function<void(std::string)>& callback);
    static std::vector<std::string> getWords(std::string s, std::string delim);
};
