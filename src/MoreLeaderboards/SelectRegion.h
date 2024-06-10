#pragma once
#include "../includes.h"
#include <Geode/utils/web.hpp>
#include "SelectRegionCell.h"

class SelectRegion : public Popup<> {
    std::vector<std::string> displayedData;
    LoadingCircle* loading_circle;

    protected:
        bool setup() override;
        void startLoading();
        void loadPage();

        cocos2d::CCSize m_sScrLayerSize;
        float m_fWidth = s_defWidth;
        float m_fHeight = s_defHeight;
    public:
        static constexpr const float s_defWidth = 400.0f;
        static constexpr const float s_defHeight = 250.0f;
        static void scene();

        static std::vector<std::string> getWords(std::string s, std::string delim);

        void fadeLoadingCircle();
        void handle_request(std::string const& data);
};