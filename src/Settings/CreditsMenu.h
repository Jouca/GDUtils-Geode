#pragma once
#include "../BrownAlertDelegate.hpp"
class CreditsMenu : public BrownAlertDelegate {
    protected:
        virtual void setup();
        cocos2d::CCSize m_sScrLayerSize;
        float m_fWidth = s_defWidth;
        float m_fHeight = s_defHeight;
    public:
        static constexpr const float s_defWidth = 300.0f;
        static constexpr const float s_defHeight = 200.0f;
        static CreditsMenu* create();
        void onFireeBtn(CCObject*);
        void onJoucaBtn(CCObject*);
        void onMaverickBtn(CCObject*);
};
