#pragma once
#include "BrownAlertDelegate.hpp"
class CreditsBetaMenu : public BrownAlertDelegate {
    protected:
        virtual void setup();
        virtual CCLayer* getPage(int pageNum);
        cocos2d::CCSize m_sScrLayerSize;
        float m_fWidth = s_defWidth;
        float m_fHeight = s_defHeight;

        int pageIndex = 0;
        int maxPages = 4;
    public:
        static constexpr const float s_defWidth = 300.0f;
        static constexpr const float s_defHeight = 200.0f;
        static CreditsBetaMenu* create();
        void goToURL(CCObject*);
        void nextPage(CCObject*);
        void prevPage(CCObject*);
};
/*
class RedirectLink : public gd::CCMenuItemSpriteExtra {
    std::string redirectURI;
    public:
        // i like and dislike inheritance, i cant even use redirectURI because "it doesnt exist in gd::CCMenuItemSpriteExtra", i mean its true but WHY
        RedirectLink(CCNode* sprite, CCObject* target, SEL_MenuHandler callback) {
            gd::CCMenuItemSpriteExtra::create(sprite, target, callback);
            addChild(sprite);
        }
        void goToURL(CCObject*);
        static RedirectLink* createWithLink(std::string url, cocos2d::CCNode *sprite, cocos2d::CCObject *target);
};
*/
