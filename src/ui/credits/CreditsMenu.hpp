#pragma once
class CreditsMenu : public geode::Popup {
    protected:
        bool init();
    public:
        static CreditsMenu* create() {
            auto ret = new CreditsMenu();
            if (ret->init()) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }
};
