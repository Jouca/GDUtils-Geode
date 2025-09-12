#pragma once
class CreditsMenu : public geode::Popup<> {
    protected:
        bool setup() override;
    public:
        static CreditsMenu* create() {
            auto ret = new CreditsMenu();
            if (ret->initAnchored(300.f, 200.f)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }
};
