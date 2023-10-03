#pragma once

using namespace cocos2d;
using namespace geode::prelude;

class MoreLeaderboardsListView : public CustomListView {
    protected:
        void setupList() override;
        TableViewCell* getListCell(const char* key) override;
        void loadCell(TableViewCell* cell, int index) override;
        static CCRect calculateNodeCoverage(CCArray* nodes);
    public:
        static MoreLeaderboardsListView* create(CCArray* levels, float width, float height);
};