#include "CreditsMenu.h"
#include <cstdlib>
#ifdef GEODE_IS_WINDOWS
#include <shellapi.h>
#endif

void OpenURL(const std::string& url) {
    #ifdef GEODE_IS_WINDOWS
    ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
    #else // mac or other platforms, though i might have to look how to do for android
    std::string command = "open " + url;
    std::system(command.c_str());
    #endif
}

void CreditsMenu::onFireeBtn(CCObject*) {
    OpenURL("https://www.youtube.com/@gdfiree");
}
void CreditsMenu::onJoucaBtn(CCObject*) {
    OpenURL("https://twitter.com/JoucaJouca"); // nitter.net if youre not signed in ok
}
void CreditsMenu::onMaverickBtn(CCObject*) {
    OpenURL("https://www.youtube.com/channel/UCoFBsXJ-6o6l6ZYP-k6DE_g");
}

void CreditsMenu::setup() {
    auto fireeIcon = SimplePlayer::create(98);
    auto firstColorFiree = ccColor3B {125,0,255};
    auto secondColorFiree = ccColor3B {255,255,255};
    fireeIcon->setChildColor(firstColorFiree);
    fireeIcon->setSecondColor(secondColorFiree);
    fireeIcon->setGlowOutline(true);
    
    auto joucaIcon = SimplePlayer::create(133);
    auto firstColorJouca = ccColor3B {255,0,255};
    auto secondColorJouca = ccColor3B {255,0,125};
    joucaIcon->setSecondColor(firstColorJouca);
    joucaIcon->setChildColor(secondColorJouca);
    joucaIcon->setGlowOutline(true);

    auto maverickIcon = SimplePlayer::create(127);
    auto firstColorMaverick = ccColor3B {0,255,254};
    auto secondColorMaverick = ccColor3B {255,0,125};
    maverickIcon->setSecondColor(firstColorMaverick);
    maverickIcon->setChildColor(secondColorMaverick);
    maverickIcon->setGlowOutline(true);

    fireeIcon->setScale(1.25F);
    joucaIcon->setScale(1.25F);
    maverickIcon->setScale(1.25F);
    joucaIcon->setPositionX(-90);
    fireeIcon->setPositionX(0);
    maverickIcon->setPositionX(90);

    auto fireeLabel = CCLabelBMFont::create("Firee", "goldFont.fnt");
    auto joucaLabel = CCLabelBMFont::create("Jouca", "goldFont.fnt");
    auto maverickLabel = CCLabelBMFont::create("Maverick", "goldFont.fnt");
    fireeLabel->setScale(.8F);
    joucaLabel->setScale(.8F);
    maverickLabel->setScale(.8F);
    auto fireeBtn = CCMenuItemSpriteExtra::create(fireeLabel, this, menu_selector(CreditsMenu::onFireeBtn));
    auto joucaBtn = CCMenuItemSpriteExtra::create(joucaLabel, this, menu_selector(CreditsMenu::onJoucaBtn));
    auto maverickBtn = CCMenuItemSpriteExtra::create(maverickLabel, this, menu_selector(CreditsMenu::onMaverickBtn));
    auto thankRob1 = CCLabelBMFont::create("Thanks to RobTopGames", "bigFont.fnt");
    auto thankRob2 = CCLabelBMFont::create("for his trust on this project!", "bigFont.fnt");

    
    fireeBtn->setPosition({ 0, 34 });
    joucaBtn->setPosition({ -90, 34 });
    maverickBtn->setPosition({ 90, 34 });
    thankRob1->setScale(0.5F);
    thankRob2->setScale(0.5F);
    thankRob1->setPositionY(-44);
    thankRob2->setPositionY(-58);

    this->m_buttonMenu->addChild(fireeBtn);
    this->m_buttonMenu->addChild(joucaBtn);
    this->m_buttonMenu->addChild(maverickBtn);
    this->m_buttonMenu->addChild(thankRob1);
    this->m_buttonMenu->addChild(thankRob2);
    this->m_buttonMenu->addChild(fireeIcon);
    this->m_buttonMenu->addChild(joucaIcon);
    this->m_buttonMenu->addChild(maverickIcon);
    setTouchEnabled(true);
}

CreditsMenu* CreditsMenu::create() {
    auto pRet = new CreditsMenu();
    if (pRet && pRet->init(CreditsMenu::s_defWidth, CreditsMenu::s_defHeight, "GJ_square01.png", "Credits")) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
};
