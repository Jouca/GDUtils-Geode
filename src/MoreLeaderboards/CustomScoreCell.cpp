#include "../includes.h"
#include <Geode/modify/GjScoreCell.hpp>
#include <Geode/modify/Field.hpp>

class $modify(CustomScoreCell, GJScoreCell) {
    void loadFromScore(GJUserScore* score) {
        GJScoreCell::loadFromScore(score);

        auto scene = CCDirector::sharedDirector()->getRunningScene();
        auto layer = scene->getChildren()->objectAtIndex(0);
        auto layerName = misc::getNodeName(layer);

        if (layerName == "MoreLeaderboards") {
            CCLayer* layer = as<CCLayer*>(this->getChildren()->objectAtIndex(1));
            if (layer) {
                //log::debug("Layer found");
            }
        }
    }
};