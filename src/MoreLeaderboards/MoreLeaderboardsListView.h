#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class MoreLeaderboardsListView : public CustomListView {
    protected:
        void setupList(float) override;
        TableViewCell* getListCell(const char* key) override;
        void loadCell(TableViewCell* cell, int index) override;
        static CCRect calculateNodeCoverage(CCArray* nodes);
    public:
        static MoreLeaderboardsListView* create(CCArray* levels, float width, float height);
};
