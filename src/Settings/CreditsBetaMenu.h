#pragma once
class CreditsBetaMenu : public geode::Popup<> {
    protected:
        cocos2d::CCNode* m_section;
        uint8_t m_currentPage = 0;
        bool setup() override;
    public:
        void updatePage();
        static CreditsBetaMenu* create() {
            auto ret = new CreditsBetaMenu();
            if (ret->initAnchored(300.f, 200.f)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }
};
