#include "Position.hpp"

SettingNodeV3* PositionValue::createNode(float width) {
    return PositionNode::create(static_pointer_cast<PositionValue>(shared_from_this()), width);
}

bool PositionNode::init(std::shared_ptr<PositionValue> setting, float width) {
    if (!SettingValueNodeV3::init(setting, width))
        return false;
    this->setContentSize({ width, 70.f });
    getButtonMenu()->setContentSize({width / 2, 45.F});
    CCSprite* toggleOn = Build<CCSprite>::createSpriteName("GJ_checkOn_001.png").scale(0.7f).collect();
    CCSprite* toggleOff = Build<CCSprite>::createSpriteName("GJ_checkOff_001.png").scale(0.7f).collect();
    auto currentCorner = static_cast<PositionEnum>(this->getValue());
    Build<CCMenuItemToggler>::createToggle(toggleOn, toggleOff, [this](auto) {
        onCornerClick(PositionEnum::TopLeft);
    }).toggle(currentCorner != PositionEnum::TopLeft).parentAtPos(getButtonMenu(), Anchor::TopLeft).store(m_tlBtn);
    Build<CCMenuItemToggler>::createToggle(toggleOn, toggleOff, [this](auto) {
        onCornerClick(PositionEnum::TopRight);
    }).toggle(currentCorner != PositionEnum::TopRight).parentAtPos(getButtonMenu(), Anchor::TopRight).store(m_trBtn);
    Build<CCMenuItemToggler>::createToggle(toggleOn, toggleOff, [this](auto) {
        onCornerClick(PositionEnum::BottomLeft);
    }).toggle(currentCorner != PositionEnum::BottomLeft).parentAtPos(getButtonMenu(), Anchor::BottomLeft).store(m_blBtn);
    Build<CCMenuItemToggler>::createToggle(toggleOn, toggleOff, [this](auto) {
        onCornerClick(PositionEnum::BottomRight);
    }).toggle(currentCorner != PositionEnum::BottomRight).parentAtPos(getButtonMenu(), Anchor::BottomRight).store(m_brBtn);
    getButtonMenu()->updateLayout();
    getButtonMenu()->setAnchorPoint({0.5, 0.5});
    getButtonMenu()->updateAnchoredPosition(Anchor::Center, {0, 0});
    this->getNameLabel()->setVisible(false);
    this->updateState(nullptr);
    return true;
}

$execute {
    auto ret = Mod::get()->registerCustomSettingType("notification-placement", &PositionValue::parse);
    if (!ret) {
        log::error("Unable to register setting type: {}", ret.unwrapErr());
    }
}
