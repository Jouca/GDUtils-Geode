#pragma once
#include "../includes.h"
#include <Geode/utils/web.hpp>
#include "../BrownAlertDelegate.hpp"
#include "MoreLeaderboardsListView.h"
#include "SelectRegion.h"

#include <iostream>
#include <vector>

enum class StatsListType {
    Stars,
    Diamonds,
    UserCoins,
    Demons,
    Moons,
    Creators,
    classicDemonsEasy,
    classicDemonsMedium,
    classicDemonsHard,
    classicDemonsInsane,
    classicDemonsExtreme,
    platformerDemonsEasy,
    platformerDemonsMedium,
    platformerDemonsHard,
    platformerDemonsInsane,
    platformerDemonsExtreme,
    BetterProgression,
    Pointercrate
};

class MoreLeaderboards : public CCLayer {
    MoreLeaderboardsListView* leaderboardView = nullptr;
    CustomListView* leaderboardViewScore = nullptr;
    GJListLayer* listLayer = nullptr;
    LoadingCircle* loading_circle = nullptr;
    cocos2d::CCArray* displayedData;

    CCLabelBMFont* page_label = nullptr;
    CCMenuItemSpriteExtra* page_left = nullptr;
    CCMenuItemSpriteExtra* page_right = nullptr;
    CCMenuItemSpriteExtra* tab_page_left = nullptr;
    CCMenuItemSpriteExtra* tab_page_right = nullptr;
    CCMenuItemSpriteExtra* mod_filter_btn = nullptr;
    CCMenuItemSpriteExtra* mod_filter_type_btn = nullptr;
    CCSprite* trophy = nullptr;

    CCString* datas = nullptr;
    std::string data_region = "";

    int country_id = 0;

    CCMenu* m_menu;
    CCMenuItemToggler* m_tab1;
    CCMenuItemToggler* m_tab2;
    CCMenuItemToggler* m_tab3;
    CCMenuItemToggler* m_tab4;
    CCMenuItemToggler* m_tab5;
    CCMenuItemToggler* m_tab6;
    int tab_page = 0;

    CCMenuItemSpriteExtra* m_search;

    #ifndef GEODE_IS_IOS
    CCClippingNode* m_tabsGradientNode = nullptr;
    #else
    CCNode* m_tabsGradientNode = nullptr;
    #endif
    CCSprite* m_tabsGradientSprite = nullptr;
    CCSprite* m_tabsGradientStencil = nullptr;

    protected:
        virtual bool init(std::string type);
        virtual void keyBackClicked();
        void startLoadingMods();
        void loadPageMods();
        void startLoadingMore();
        void loadPageMore();
        void loadPageStats();

        void resetInfos();
    public:
        static std::string data_response_moreLB;
        static StatsListType g_tab;
        static std::string username;
        static int scroll_int;
        static bool modFilter;
        static int modFilterType;
        bool loading = false;

        void onMoreLeaderboards(CCObject*);
        void onModsList(CCObject*);
        void onRegion(CCObject*);
        void onInfo(CCObject*);
        void onLBInfo(CCObject*);

        void onTab(CCObject* pSender);
        void onPageLeft(CCObject* pSender);
        void onPageRight(CCObject* pSender);

        void loadTabPageButtons();
        void onTabPageLeft(CCObject* pSender);
        void onTabPageRight(CCObject* pSender);
        void onModFilter(CCObject* pSender);
        void onModFilterType(CCObject* pSender);
        void changeTabPage();

        void onSearch(CCObject*);

        static MoreLeaderboards* create(std::string type);
        void fadeLoadingCircle();
        void handle_request_mods(std::string const& data);
        void handle_request_more(std::string const& data);

        static std::vector<std::string> getWords(std::string s, std::string delim);
        static CCDictionary* responseToDict(const std::string& response);

        void backButton(CCObject*);
};
