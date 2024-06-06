#pragma once
#include "../BrownAlertDelegate.hpp"
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
