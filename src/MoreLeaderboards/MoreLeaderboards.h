#pragma once
#include "../includes.h"
#include <Geode/utils/web.hpp>
#include "MoreLeaderboardsListView.h"

#include <iostream>
#include <vector>

enum class StatsListType {
    Diamonds,
    UserCoins,
    Demons,
    Moons,
    Creators
};

class MoreLeaderboards : public CCLayer {
    MoreLeaderboardsListView* leaderboardView = nullptr;
    CustomListView* leaderboardViewScore = nullptr;
    GJListLayer* listLayer = nullptr;
    cocos2d::CCArray* displayedData = nullptr;
    LoadingCircle* loading_circle = nullptr;
    CCString* datas = nullptr;

    bool loading = false;

    CCMenu* m_menu;
    CCMenuItemToggler* m_diamondsTabBtn;
    CCMenuItemToggler* m_usercoinsTabBtn;
    CCMenuItemToggler* m_demonsTabBtn;
    CCMenuItemToggler* m_moonsTabBtn;
    CCMenuItemToggler* m_creatorsTabBtn;
    CCClippingNode* m_tabsGradientNode = nullptr;
    CCSprite* m_tabsGradientSprite = nullptr;
    CCSprite* m_tabsGradientStencil = nullptr;

    protected:
        virtual bool init(std::string type);
        virtual void keyBackClicked();
        void startLoadingMods();
        void loadPageMods();
        void startLoadingMore();
        void loadPageMore();
    public:
        void onMoreLeaderboards(CCObject*);
        void onModsList(CCObject*);

        void onTab(CCObject* pSender);

        static MoreLeaderboards* create(std::string type);
        void fadeLoadingCircle();
        void handle_request_mods(std::string const& data);
        void handle_request_more(std::string const& data);

        static std::vector<std::string> getWords(std::string s, std::string delim);
        static CCDictionary* responseToDict(const std::string& response);

        void backButton(CCObject*);
};
