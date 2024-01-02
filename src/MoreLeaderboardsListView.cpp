#include "MoreLeaderboardsListView.h"
#include "MoreLeaderboards.h"
#include "MoreLeaderboardsCell.h"

MoreLeaderboardsListView* MoreLeaderboardsListView::create(cocos2d::CCArray* levels, float width, float height) {
    auto ret = new MoreLeaderboardsListView();
    if (ret && ret->init(levels, static_cast<BoomListType>(0x420), width, height)) {
        ret->autorelease();
    } else {
        delete ret;
        ret = nullptr;
    }
    return ret;
};

void MoreLeaderboardsListView::setupList(float d) {
    this->m_itemSeparation = 40.0f;

    this->m_tableView->reloadData();

    auto coverage = calculateNodeCoverage(m_tableView->m_contentLayer->getChildren());
    if (this->m_entries->count() > 4)
        m_tableView->m_contentLayer->setContentSize({-coverage.origin.x + coverage.size.width, -coverage.origin.y + coverage.size.height});

    this->m_tableView->moveToTop();

    if (this->m_entries->count() == 1)
        this->m_tableView->moveToTopWithOffset(this->m_itemSeparation);
};

cocos2d::CCRect MoreLeaderboardsListView::calculateNodeCoverage(CCArray* nodes){
    CCRect coverage;
    CCObject* childObj;
    CCARRAY_FOREACH(nodes, childObj) {
        CCNode* child = dynamic_cast<CCNode*>(childObj);
        if(child == nullptr) continue;

        auto contentSize = child->getScaledContentSize();
        contentSize.width *= child->getAnchorPoint().x;
        contentSize.height *= child->getAnchorPoint().y;
        auto pos = child->getPosition() - contentSize;

        contentSize = child->getScaledContentSize();
        auto contentPoint = CCPoint { 1.f, 1.f } - child->getAnchorPoint();
        contentSize.width *= contentPoint.x;
        contentSize.height *= contentPoint.y;

        auto csize = child->getPosition() + contentSize;
        if (pos.x < coverage.origin.x) {
            coverage.origin.x = pos.x;
        }
        if (pos.y < coverage.origin.y) {
            coverage.origin.y = pos.y;
        }
        if (csize.x > coverage.size.width) {
            coverage.size.width = csize.x;
        }
        if (csize.y > coverage.size.height) {
            coverage.size.height = csize.y;
        }
    }
    return coverage;
};

void MoreLeaderboardsListView::loadCell(TableViewCell* cell, int index) {
    static_cast<MoreLeaderboardsCell*>(cell)->loadFromDict(
        static_cast<CCDictionary*>(this->m_entries->objectAtIndex(index))
    );
    static_cast<MoreLeaderboardsCell*>(cell)->updateBGColor(index);
}

TableViewCell* MoreLeaderboardsListView::getListCell(const char* key) {
    return new MoreLeaderboardsCell(key, { this->m_width, 40.0f });
};
