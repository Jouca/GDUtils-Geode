#include "Credits.hpp"
#include <ui/credits/CreditsMenu.hpp>
#include <ui/credits/CreditsBetaMenu.hpp>

SettingNodeV3* CreditsValue::createNode(float width) {
    return CreditsNode::create(static_pointer_cast<CreditsValue>(shared_from_this()), width);
}

bool CreditsNode::init(std::shared_ptr<CreditsValue> setting, float width) {
    if (!SettingNodeV3::init(setting, width))
        return false;
    this->setContentSize({ width, 40.f });
    getNameLabel()->setVisible(false);
    getButtonMenu()->setContentWidth(width);
    Build<ButtonSprite>::create("Credits", "bigFont.fnt", "GJ_button_01.png").scale(0.6f).intoMenuItem([]() {
        CreditsMenu::create()->show();
    }).parentAtPos(getButtonMenu(), Anchor::Center, {-55, 0});
    Build<ButtonSprite>::create("Beta Testers", "bigFont.fnt", "GJ_button_01.png").scale(0.6f).intoMenuItem([]() {
        CreditsBetaMenu::create()->show();
    }).parentAtPos(getButtonMenu(), Anchor::Center, {85, 0});
    getButtonMenu()->updateLayout();
    return true;
}

$execute {
    (void)Mod::get()->registerCustomSettingType("credit-buttons", &CreditsValue::parse);
}
