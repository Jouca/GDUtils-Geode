#pragma once
#include <Geode/utils/web.hpp>
#include "MoreLeaderboardsListView.h"

#include <iostream>
#include <vector>

using namespace cocos2d;
using namespace geode::prelude;

class MoreLeaderboards : public CCLayer {
    MoreLeaderboardsListView* leaderboardView = nullptr;
    GJListLayer* listLayer = nullptr;
    cocos2d::CCArray* displayedData = nullptr;
    LoadingCircle* loading_circle = nullptr;
    CCString* datas = nullptr;
    protected:
        virtual bool init(std::string type);
        void startLoadingMods();
        void loadPageMods();
    public:
        void onMoreLeaderboards(CCObject*);
        void onModsList(CCObject*);
        static MoreLeaderboards* create(std::string type);
        void fadeLoadingCircle();
        void handle_request(std::string const& data);

        static std::vector<std::string> getWords(std::string s, std::string delim);

        void backButton(CCObject*);
};