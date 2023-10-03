#pragma once
#include "MoreLeaderboards.h"

#include <Geode/Bindings.hpp>

using namespace cocos2d;
using namespace geode::prelude;

class MoreLeaderboardsCell : public GenericListCell {
    void draw() override;
public:
    inline static CCDictionary* static_content;
    inline static CCString* static_data_other;

    MoreLeaderboardsCell(const char* name, CCSize size);
    void loadFromDict(CCDictionary* dict);
    void callback_user(CCObject* pSender);
    void callback_link(CCObject* pSender);
    static MoreLeaderboardsCell* create(const char* key, CCSize size);
};