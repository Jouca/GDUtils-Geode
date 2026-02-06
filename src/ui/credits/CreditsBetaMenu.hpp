#pragma once
class CreditsBetaMenu : public geode::Popup {
    protected:
        cocos2d::CCNode* m_section;
        uint8_t m_currentPage = 0;
        bool init();
    public:
        void updatePage();
        static CreditsBetaMenu* create() {
            auto ret = new CreditsBetaMenu();
            if (ret->init()) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }
};
