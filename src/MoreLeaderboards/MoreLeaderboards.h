#pragma once
#include "../includes.h"
#include <Geode/utils/web.hpp>
#include "MoreLeaderboardsListView.h"
#include "SelectRegion.h"

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
    LoadingCircle* loading_circle = nullptr;
    cocos2d::CCArray* displayedData;

    CCLabelBMFont* page_label = nullptr;
    CCMenuItemSpriteExtra* page_left = nullptr;
    CCMenuItemSpriteExtra* page_right = nullptr;
    CCSprite* trophy = nullptr;

    CCString* datas = nullptr;
    std::string data_region = "";

    int country_id = 0;

    bool loading = false;

    CCMenu* m_menu;
    CCMenuItemToggler* m_diamondsTabBtn;
    CCMenuItemToggler* m_usercoinsTabBtn;
    CCMenuItemToggler* m_demonsTabBtn;
    CCMenuItemToggler* m_moonsTabBtn;
    CCMenuItemToggler* m_creatorsTabBtn;
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

        void onMoreLeaderboards(CCObject*);
        void onModsList(CCObject*);
        void onRegion(CCObject*);
        void onInfo(CCObject*);
        void onLBInfo(CCObject*);

        void onTab(CCObject* pSender);
        void onPageLeft(CCObject* pSender);
        void onPageRight(CCObject* pSender);

        static MoreLeaderboards* create(std::string type);
        void fadeLoadingCircle();
        void handle_request_mods(std::string const& data);
        void handle_request_more(std::string const& data);

        static std::vector<std::string> getWords(std::string s, std::string delim);
        static CCDictionary* responseToDict(const std::string& response);

        void backButton(CCObject*);
};
