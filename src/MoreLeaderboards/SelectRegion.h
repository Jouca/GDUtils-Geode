#pragma once
#include "../includes.h"
#include <Geode/utils/web.hpp>
#include "SelectRegionCell.h"
#include "../OldBorder.hpp"

class SelectRegion : public Popup<> {
    LoadingCircle* loading_circle;
    std::function<void(int)> selectedRegion;

    protected:
        bool setup() override;
        void loadPage();

        cocos2d::CCSize m_sScrLayerSize;
    public:
        static std::vector<std::string> displayedData;

        SelectRegion(const std::function<void(int)>& callback) : selectedRegion(callback) {}
        
        static void scene(const std::function<void(int)>& callback);

        static std::vector<std::string> getWords(std::string s, std::string delim);
};