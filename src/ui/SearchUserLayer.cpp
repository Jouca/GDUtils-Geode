#include "SearchUserLayer.hpp"

bool SearchUserLayer::init(FriendsProfilePage* layer) {
    //if (!Popup::init(260.f, 60.f)) return false;
    if (!Popup::init(260.f, 120.f)) return false;
    m_friendPage = layer;
    input_username->setMaxCharCount(20);
    input_username->setPlaceholder("");
    m_buttonMenu->addChildAtPosition(input_username, Anchor::Center, {0, 10});
    Build<ButtonSprite>::create("Search", 60, true, "bigFont.fnt", "GJ_button_01.png", 30, .5F).intoMenuItem([this]() {
        onValidate();
    }).parentAtPos(m_buttonMenu, Anchor::Center, {0, -35});
    return true;
}
