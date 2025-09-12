#include "CreditsMenu.h"
#include "Geode/utils/web.hpp"
using namespace geode::prelude;

bool CreditsMenu::setup() {
    auto title = CCLabelBMFont::create("Credits", "bigFont.fnt");
    title->setScale(0.8f);
    m_mainLayer->addChildAtPosition(title, Anchor::Top, {0, -25});

    auto fireeIcon = SimplePlayer::create(98);
    auto firstColorFiree = ccColor3B {125,0,255};
    auto secondColorFiree = ccColor3B {255,255,255};
    fireeIcon->setColor(firstColorFiree);
    fireeIcon->setSecondColor(secondColorFiree);
    fireeIcon->setGlowOutline(secondColorFiree);
    fireeIcon->updateColors();
    
    auto joucaIcon = SimplePlayer::create(133);
    auto firstColorJouca = ccColor3B {255,0,255};
    auto secondColorJouca = ccColor3B {255,0,125};
    joucaIcon->setSecondColor(firstColorJouca);
    joucaIcon->setChildColor(secondColorJouca);
    joucaIcon->setGlowOutline(firstColorJouca);
    joucaIcon->updateColors();

    auto maverickIcon = SimplePlayer::create(127);
    auto firstColorMaverick = ccColor3B {0,255,254};
    auto secondColorMaverick = ccColor3B {255,0,125};
    maverickIcon->setSecondColor(firstColorMaverick);
    maverickIcon->setChildColor(secondColorMaverick);
    maverickIcon->setGlowOutline(secondColorMaverick);
    maverickIcon->updateColors();

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
    auto thankRob1 = CCLabelBMFont::create("Thanks to RobTopGames", "bigFont.fnt");
    auto thankRob2 = CCLabelBMFont::create("for his trust on this project!", "bigFont.fnt");

    thankRob1->setScale(0.5F);
    thankRob2->setScale(0.5F);

    m_buttonMenu->addChildAtPosition(CCMenuItemExt::createSpriteExtra(fireeLabel, [](CCObject*) {
        web::openLinkInBrowser("https://www.youtube.com/@gdfiree");
    }), Anchor::Center, {0, 34});
    m_buttonMenu->addChildAtPosition(CCMenuItemExt::createSpriteExtra(joucaLabel, [](CCObject*) {
        web::openLinkInBrowser("https://twitter.com/JoucaJouca"); // nitter.net if youre not signed in ok
    }), Anchor::Center, {-90, 34});
    m_buttonMenu->addChildAtPosition(CCMenuItemExt::createSpriteExtra(maverickLabel, [](CCObject*) {
        web::openLinkInBrowser("https://www.youtube.com/channel/UCoFBsXJ-6o6l6ZYP-k6DE_g");
    }), Anchor::Center, {90, 34});
    m_mainLayer->addChildAtPosition(thankRob1, Anchor::Center, {0, -44});
    m_mainLayer->addChildAtPosition(thankRob2, Anchor::Center, {0, -58});
    m_mainLayer->addChildAtPosition(fireeIcon, Anchor::Center);
    m_mainLayer->addChildAtPosition(joucaIcon, Anchor::Center, {-90, 0});
    m_mainLayer->addChildAtPosition(maverickIcon, Anchor::Center, {90, 0});
    return true;
}
