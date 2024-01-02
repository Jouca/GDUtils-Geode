#include "CreditsBetaMenu.h"

CreditsBetaMenu* CreditsBetaMenu::create() {
    auto pRet = new CreditsBetaMenu();
    if (pRet && pRet->init(CreditsBetaMenu::s_defWidth, CreditsBetaMenu::s_defHeight, "GJ_square01.png", "Beta Credits")) {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return nullptr;
};

void CreditsBetaMenu::setup() {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    registerWithTouchDispatcher();

    auto page1 = getPage(0);
    auto page2 = getPage(1);
    auto page3 = getPage(2);
    auto page4 = getPage(3);
    auto page5 = getPage(4);
    page1->setTag(2001);
    page2->setTag(2002);
    page3->setTag(2003);
    page4->setTag(2004);
    page5->setTag(2005);
    page2->setVisible(false);
    page3->setVisible(false);
    page4->setVisible(false);
    page5->setVisible(false);

    auto prevArrow = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    auto nextArrow = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    nextArrow->setFlipX(true);
    auto nextBtn = CCMenuItemSpriteExtra::create(
        nextArrow,
        this,
        menu_selector(CreditsBetaMenu::nextPage)
    );
    auto prevBtn = CCMenuItemSpriteExtra::create(
        prevArrow,
        this,
        menu_selector(CreditsBetaMenu::prevPage)
    );
    nextBtn->setPosition({ (this->m_pLrSize.width) - 120, 0 });
    prevBtn->setPosition({ (-this->m_pLrSize.width) + 120, 0 });
    this->m_buttonMenu->addChild(prevBtn);
    this->m_buttonMenu->addChild(nextBtn);

    this->m_mainLayer->addChild(page1);
    this->m_mainLayer->addChild(page2);
    this->m_mainLayer->addChild(page3);
    this->m_mainLayer->addChild(page4);
    this->m_mainLayer->addChild(page5);
    setTouchEnabled(true);
    setKeypadEnabled(true);
}
/*
void RedirectLink::goToURL(CCObject* ret) {
    auto node = dynamic_cast<RedirectLink*>(ret);
    if (node != nullptr) {
        std::cout << node->redirectURI << std::endl;
    }
}

RedirectLink* RedirectLink::createWithLink(std::string url, cocos2d::CCNode *sprite, cocos2d::CCObject *target) {
    auto pRet = new RedirectLink(sprite, target, menu_selector(RedirectLink::goToURL));
    if (pRet) {
        pRet->redirectURI = url;
        pRet->autorelease();
        return pRet;
    } else {
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
}
*/

void CreditsBetaMenu::goToURL(CCObject* ret) {
    auto node = reinterpret_cast<CCMenuItemSpriteExtra*>(ret);
    if (node != nullptr) {
        std::string redirectURI = static_cast<CCString*>(node->getUserObject())->getCString();
        // implement code for redirectURI
    }
}

CCLayer* CreditsBetaMenu::getPage(int pageNum) {
    
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
    auto layer = CCLayer::create();
    auto menu = CCMenu::create();
    layer->addChild(menu);
    #ifdef GEODE_IS_ANDROID32 // uncommenting only when SimplePlayer gets added for Windows
    switch (pageNum) {
        case 0: {
            auto page1betaSection = CCNode::create();

            auto Icon1 = SimplePlayer::create(133);
            auto firstColorIcon1 = ccColor3B {0,0,0};
            auto secondColorIcon1 = ccColor3B {255,255,255};
            Icon1->setChildColor(firstColorIcon1);
            Icon1->setSecondColor(secondColorIcon1);
            Icon1->enableCustomGlowColor(secondColorIcon1);
            Icon1->setPosition({ -93, 11 });
            page1betaSection->addChild(Icon1);
            auto Icon1Label = CCLabelBMFont::create("TMNono", "goldFont.fnt");
            Icon1Label->setScale(.8F);
            auto Icon1Link = CCMenuItemSpriteExtra::create(Icon1Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon1Link->setUserObject(CCString::create("https://www.google.com"));
            Icon1Link->setPosition({ -92, 44 });
            menu->addChild(Icon1Link);

            auto Icon2 = SimplePlayer::create(111);
            auto firstColorIcon2 = ccColor3B {90,90,90};
            auto secondColorIcon2 = ccColor3B {175, 0, 75};
            Icon2->setChildColor(firstColorIcon2);
            Icon2->setSecondColor(secondColorIcon2);
            Icon2->enableCustomGlowColor(secondColorIcon2);
            Icon2->setPosition({ 3, 11 });
            page1betaSection->addChild(Icon2);
            auto Icon2Label = CCLabelBMFont::create("Devilmine", "goldFont.fnt");
            Icon2Label->setScale(.7F);
            auto Icon2Link = CCMenuItemSpriteExtra::create(Icon2Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon2Link->setUserObject(CCString::create("https://www.google.com"));
            Icon2Link->setPosition({ 2, 44 });
            menu->addChild(Icon2Link);

            auto Icon3 = SimplePlayer::create(50);
            auto firstColorIcon3 = ccColor3B {125, 125, 255};
            auto secondColorIcon3 = ccColor3B {125, 255, 175};
            Icon3->setChildColor(firstColorIcon3);
            Icon3->setSecondColor(secondColorIcon3);
            Icon3->enableCustomGlowColor(secondColorIcon3);
            Icon3->setPosition({ 95, 11 });
            page1betaSection->addChild(Icon3);
            auto Icon3Label = CCLabelBMFont::create("SupuGD", "goldFont.fnt");
            Icon3Label->setScale(.8F);
            auto Icon3Link = CCMenuItemSpriteExtra::create(Icon3Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon3Link->setUserObject(CCString::create("https://www.google.com"));
            Icon3Link->setPosition({ 95, 44 });
            menu->addChild(Icon3Link);

            auto Icon4 = SimplePlayer::create(140);
            auto firstColorIcon4 = ccColor3B {0, 75, 175};
            auto secondColorIcon4 = ccColor3B {255, 255, 0};
            Icon4->setChildColor(firstColorIcon4);
            Icon4->setSecondColor(secondColorIcon4);
            Icon4->enableCustomGlowColor(secondColorIcon4);
            Icon4->setPosition({ -93, -58 });
            page1betaSection->addChild(Icon4);
            auto Icon4Label = CCLabelBMFont::create("Silvow", "goldFont.fnt");
            Icon4Label->setScale(.8F);
            auto Icon4Link = CCMenuItemSpriteExtra::create(Icon4Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon4Link->setUserObject(CCString::create("https://www.google.com"));
            Icon4Link->setPosition({ -93, -24 });
            menu->addChild(Icon4Link);

            auto Icon5 = SimplePlayer::create(46);
            auto firstColorIcon5 = ccColor3B {90, 90, 90};
            auto secondColorIcon5 = ccColor3B {175, 175, 175};
            Icon5->setChildColor(firstColorIcon5);
            Icon5->setSecondColor(secondColorIcon5);
            Icon5->setPosition({ 2, -58 });
            page1betaSection->addChild(Icon5);
            auto Icon5Label = CCLabelBMFont::create("SantyBoyMX", "goldFont.fnt");
            Icon5Label->setScale(.6F);
            auto Icon5Link = CCMenuItemSpriteExtra::create(Icon5Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon5Link->setUserObject(CCString::create("https://www.google.com"));
            Icon5Link->setPosition({ 2, -24 });
            menu->addChild(Icon5Link);

            auto Icon6 = SimplePlayer::create(57);
            auto firstColorIcon6 = ccColor3B {0, 75, 175};
            auto secondColorIcon6 = ccColor3B {255, 255, 255};
            Icon6->setChildColor(firstColorIcon6);
            Icon6->setSecondColor(secondColorIcon6);
            Icon6->enableCustomGlowColor(secondColorIcon6);
            Icon6->setPosition({ 96, -58 });
            page1betaSection->addChild(Icon6);
            auto Icon6Label = CCLabelBMFont::create("Tygore", "goldFont.fnt");
            Icon6Label->setScale(.8F);
            auto Icon6Link = CCMenuItemSpriteExtra::create(Icon6Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon6Link->setUserObject(CCString::create("https://www.google.com"));
            Icon6Link->setPosition({ 96, -24 });
            menu->addChild(Icon6Link);

            menu->addChild(page1betaSection);
            break;
        }
        case 1: {
            auto page2betaSection = CCNode::create();

            auto Icon1 = SimplePlayer::create(141);
            auto firstColorIcon1 = ccColor3B {0, 125, 125};
            auto secondColorIcon1 = ccColor3B {0, 255, 255};
            Icon1->setChildColor(firstColorIcon1);
            Icon1->setSecondColor(secondColorIcon1);
            Icon1->enableCustomGlowColor(secondColorIcon1);
            Icon1->setPosition({ -93, 11 });
            page2betaSection->addChild(Icon1);
            auto Icon1Label = CCLabelBMFont::create("Hir0Shi", "goldFont.fnt");
            Icon1Label->setScale(.8F);
            auto Icon1Link = CCMenuItemSpriteExtra::create(Icon1Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon1Link->setUserObject(CCString::create("https://www.google.com"));
            Icon1Link->setPosition({ -92, 44 });
            menu->addChild(Icon1Link);

            auto Icon2 = SimplePlayer::create(48);
            auto firstColorIcon2 = ccColor3B {0, 125, 255};
            auto secondColorIcon2 = ccColor3B {255, 255, 0};
            Icon2->setChildColor(firstColorIcon2);
            Icon2->setSecondColor(secondColorIcon2);
            Icon2->setPosition({ 3, 11 });
            page2betaSection->addChild(Icon2);
            auto Icon2Label = CCLabelBMFont::create("Komatic5", "goldFont.fnt");
            Icon2Label->setScale(.7F);
            auto Icon2Link = CCMenuItemSpriteExtra::create(Icon2Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon2Link->setUserObject(CCString::create("https://www.google.com"));
            Icon2Link->setPosition({ 2, 44 });
            menu->addChild(Icon2Link);

            auto Icon3 = SimplePlayer::create(4);
            auto firstColorIcon3 = ccColor3B {255, 125, 125};
            auto secondColorIcon3 = ccColor3B {255, 255, 255};
            Icon3->setChildColor(firstColorIcon3);
            Icon3->setSecondColor(secondColorIcon3);
            Icon3->enableCustomGlowColor(secondColorIcon3);
            Icon3->setPosition({ 95, 11 });
            page2betaSection->addChild(Icon3);
            auto Icon3Label = CCLabelBMFont::create("Xanii", "goldFont.fnt");
            Icon3Label->setScale(.8F);
            auto Icon3Link = CCMenuItemSpriteExtra::create(Icon3Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon3Link->setUserObject(CCString::create("https://www.google.com"));
            Icon3Link->setPosition({ 95, 44 });
            menu->addChild(Icon3Link);

            auto Icon4 = SimplePlayer::create(127);
            auto firstColorIcon4 = ccColor3B {255, 0, 125};
            auto secondColorIcon4 = ccColor3B {0, 255, 255};
            Icon4->setChildColor(firstColorIcon4);
            Icon4->setSecondColor(secondColorIcon4);
            Icon4->enableCustomGlowColor(secondColorIcon4);
            Icon4->setPosition({ -93, -58 });
            page2betaSection->addChild(Icon4);
            auto Icon4Label = CCLabelBMFont::create("maverick", "goldFont.fnt");
            Icon4Label->setScale(.7F);
            auto Icon4Link = CCMenuItemSpriteExtra::create(Icon4Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon4Link->setUserObject(CCString::create("https://www.google.com"));
            Icon4Link->setPosition({ -93, -24 });
            menu->addChild(Icon4Link);

            auto Icon5 = SimplePlayer::create(115);
            auto firstColorIcon5 = ccColor3B {0, 255, 255};
            auto secondColorIcon5 = ccColor3B {0, 200, 255};
            Icon5->setChildColor(firstColorIcon5);
            Icon5->setSecondColor(secondColorIcon5);
            Icon5->enableCustomGlowColor(secondColorIcon5);
            Icon5->setPosition({ 2, -58 });
            page2betaSection->addChild(Icon5);
            auto Icon5Label = CCLabelBMFont::create("arcanin", "goldFont.fnt");
            Icon5Label->setScale(.7F);
            auto Icon5Link = CCMenuItemSpriteExtra::create(Icon5Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon5Link->setUserObject(CCString::create("https://www.google.com"));
            Icon5Link->setPosition({ 2, -24 });
            menu->addChild(Icon5Link);

            auto Icon6 = SimplePlayer::create(54);
            auto firstColorIcon6 = ccColor3B {125, 125, 255};
            auto secondColorIcon6 = ccColor3B {125, 255, 175};
            Icon6->setChildColor(firstColorIcon6);
            Icon6->setSecondColor(secondColorIcon6);
            Icon6->enableCustomGlowColor(secondColorIcon6);
            Icon6->setPosition({ 96, -58 });
            page2betaSection->addChild(Icon6);
            auto Icon6Label = CCLabelBMFont::create("Ficelo", "goldFont.fnt");
            Icon6Label->setScale(.8F);
            auto Icon6Link = CCMenuItemSpriteExtra::create(Icon6Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon6Link->setUserObject(CCString::create("https://www.google.com"));
            Icon6Link->setPosition({ 96, -24 });
            menu->addChild(Icon6Link);

            menu->addChild(page2betaSection);
            break;
        }
        case 2: { // check for update page
            auto page3betaSection = CCNode::create();

            auto Icon1 = SimplePlayer::create(105);
            auto firstColorIcon1 = ccColor3B {125, 0, 255};
            auto secondColorIcon1 = ccColor3B {0, 255, 255};
            Icon1->setChildColor(firstColorIcon1);
            Icon1->setSecondColor(secondColorIcon1);
            Icon1->enableCustomGlowColor(secondColorIcon1);
            Icon1->setPosition({ -93, 11 });
            page3betaSection->addChild(Icon1);
            auto Icon1Label = CCLabelBMFont::create("JustL3o", "goldFont.fnt");
            Icon1Label->setScale(.8F);
            auto Icon1Link = CCMenuItemSpriteExtra::create(Icon1Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon1Link->setUserObject(CCString::create("https://www.google.com"));
            Icon1Link->setPosition({ -92, 44 });
            menu->addChild(Icon1Link);

            auto Icon2 = SimplePlayer::create(28);
            auto firstColorIcon2 = ccColor3B {135,0,0};
            auto secondColorIcon2 = ccColor3B {255, 255, 255};
            Icon2->setChildColor(firstColorIcon2);
            Icon2->setSecondColor(secondColorIcon2);
            Icon2->enableCustomGlowColor(secondColorIcon2);
            Icon2->setPosition({ 3, 11 });
            page3betaSection->addChild(Icon2);
            auto Icon2Label = CCLabelBMFont::create("TheRealWeenside", "goldFont.fnt");
            Icon2Label->setScale(.45F);
            auto Icon2Link = CCMenuItemSpriteExtra::create(Icon2Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon2Link->setUserObject(CCString::create("https://www.google.com"));
            Icon2Link->setPosition({ 2, 44 });
            menu->addChild(Icon2Link);

            auto Icon3 = SimplePlayer::create(123);
            auto firstColorIcon3 = ccColor3B {255, 255, 255};
            auto secondColorIcon3 = ccColor3B {0, 255, 255};
            Icon3->setChildColor(firstColorIcon3);
            Icon3->setSecondColor(secondColorIcon3);
            Icon3->enableCustomGlowColor(secondColorIcon3);
            Icon3->setPosition({ 95, 11 });
            page3betaSection->addChild(Icon3);
            auto Icon3Label = CCLabelBMFont::create("Klafterno", "goldFont.fnt");
            Icon3Label->setScale(.6F);
            auto Icon3Link = CCMenuItemSpriteExtra::create(Icon3Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon3Link->setUserObject(CCString::create("https://www.google.com"));
            Icon3Link->setPosition({ 95, 44 });
            menu->addChild(Icon3Link);

            auto Icon4 = SimplePlayer::create(77);
            auto firstColorIcon4 = ccColor3B {255, 255, 0};
            auto secondColorIcon4 = ccColor3B {125, 125, 255};
            Icon4->setChildColor(firstColorIcon4);
            Icon4->setSecondColor(secondColorIcon4);
            Icon4->setPosition({ -93, -58 });
            page3betaSection->addChild(Icon4);
            auto Icon4Label = CCLabelBMFont::create("MarkyForce", "goldFont.fnt");
            Icon4Label->setScale(.6F);
            auto Icon4Link = CCMenuItemSpriteExtra::create(Icon4Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon4Link->setUserObject(CCString::create("https://www.google.com"));
            Icon4Link->setPosition({ -93, -24 });
            menu->addChild(Icon4Link);

            auto Icon5 = SimplePlayer::create(98);
            auto firstColorIcon5 = ccColor3B {255, 255, 255};
            auto secondColorIcon5 = ccColor3B {0, 255, 255};
            Icon5->setChildColor(firstColorIcon5);
            Icon5->setSecondColor(secondColorIcon5);
            Icon5->enableCustomGlowColor(secondColorIcon5);
            Icon5->setPosition({ 2, -58 });
            page3betaSection->addChild(Icon5);
            auto Icon5Label = CCLabelBMFont::create("Cobalium", "goldFont.fnt");
            Icon5Label->setScale(.7F);
            auto Icon5Link = CCMenuItemSpriteExtra::create(Icon5Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon5Link->setUserObject(CCString::create("https://www.google.com"));
            Icon5Link->setPosition({ 2, -24 });
            menu->addChild(Icon5Link);

            auto Icon6 = SimplePlayer::create(1);
            auto firstColorIcon6 = ccColor3B {125, 255, 0};
            auto secondColorIcon6 = ccColor3B {0, 255, 255};
            Icon6->setChildColor(firstColorIcon6);
            Icon6->setSecondColor(secondColorIcon6);
            Icon6->setPosition({ 96, -58 });
            page3betaSection->addChild(Icon6);
            auto Icon6Label = CCLabelBMFont::create("jihotv01", "goldFont.fnt");
            Icon6Label->setScale(.8F);
            auto Icon6Link = CCMenuItemSpriteExtra::create(Icon6Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon6Link->setUserObject(CCString::create("https://www.google.com"));
            Icon6Link->setPosition({ 96, -24 });
            menu->addChild(Icon6Link);

            menu->addChild(page3betaSection);
            break;
        }
        case 3: { // check for update page
            auto page4betaSection = CCNode::create();

            auto Icon1 = SimplePlayer::create(31);
            auto firstColorIcon1 = ccColor3B {255, 255, 255};
            auto secondColorIcon1 = ccColor3B {255, 0, 0};
            Icon1->setChildColor(firstColorIcon1);
            Icon1->setSecondColor(secondColorIcon1);
            Icon1->enableCustomGlowColor(secondColorIcon1);
            Icon1->setPosition({ -93, 11 });
            page4betaSection->addChild(Icon1);
            auto Icon1Label = CCLabelBMFont::create("Paultam", "goldFont.fnt");
            Icon1Label->setScale(.7F);
            auto Icon1Link = CCMenuItemSpriteExtra::create(Icon1Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon1Link->setUserObject(CCString::create("https://www.google.com"));
            Icon1Link->setPosition({ -92, 44 });
            menu->addChild(Icon1Link);

            auto Icon2 = SimplePlayer::create(38);
            auto firstColorIcon2 = ccColor3B {0, 200, 255};
            auto secondColorIcon2 = ccColor3B {255, 0, 0};
            Icon2->setChildColor(firstColorIcon2);
            Icon2->setSecondColor(secondColorIcon2);
            Icon2->setPosition({ 3, 11 });
            page4betaSection->addChild(Icon2);
            auto Icon2Label = CCLabelBMFont::create("ItzElectrix", "goldFont.fnt");
            Icon2Label->setScale(.5F);
            auto Icon2Link = CCMenuItemSpriteExtra::create(Icon2Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon2Link->setUserObject(CCString::create("https://www.google.com"));
            Icon2Link->setPosition({ 2, 44 });
            menu->addChild(Icon2Link);

            auto Icon3 = SimplePlayer::create(123);
            auto firstColorIcon3 = ccColor3B {255, 0, 125};
            auto secondColorIcon3 = ccColor3B {255, 255, 0};
            Icon3->setChildColor(firstColorIcon3);
            Icon3->setSecondColor(secondColorIcon3);
            Icon3->enableCustomGlowColor(secondColorIcon3);
            Icon3->setPosition({ 95, 11 });
            page4betaSection->addChild(Icon3);
            auto Icon3Label = CCLabelBMFont::create("Ausk", "goldFont.fnt");
            Icon3Label->setScale(.8F);
            auto Icon3Link = CCMenuItemSpriteExtra::create(Icon3Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon3Link->setUserObject(CCString::create("https://www.google.com"));
            Icon3Link->setPosition({ 95, 44 });
            menu->addChild(Icon3Link);

            auto Icon4 = SimplePlayer::create(4);
            auto firstColorIcon4 = ccColor3B {255, 255, 255};
            auto secondColorIcon4 = ccColor3B {0, 75, 175};
            Icon4->setChildColor(firstColorIcon4);
            Icon4->setSecondColor(secondColorIcon4);
            Icon4->enableCustomGlowColor(secondColorIcon4);
            Icon4->setPosition({ -93, -58 });
            page4betaSection->addChild(Icon4);
            auto Icon4Label = CCLabelBMFont::create("krenogd", "goldFont.fnt");
            Icon4Label->setScale(.7F);
            auto Icon4Link = CCMenuItemSpriteExtra::create(Icon4Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon4Link->setUserObject(CCString::create("https://www.google.com"));
            Icon4Link->setPosition({ -93, -24 });
            menu->addChild(Icon4Link);

            auto Icon5 = SimplePlayer::create(44);
            auto firstColorIcon5 = ccColor3B {90, 90, 90};
            auto secondColorIcon5 = ccColor3B {175, 175, 175};
            Icon5->setChildColor(firstColorIcon5);
            Icon5->setSecondColor(secondColorIcon5);
            Icon5->enableCustomGlowColor(secondColorIcon5);
            Icon5->setPosition({ 2, -58 });
            page4betaSection->addChild(Icon5);
            auto Icon5Label = CCLabelBMFont::create("realwhata", "goldFont.fnt");
            Icon5Label->setScale(.7F);
            auto Icon5Link = CCMenuItemSpriteExtra::create(Icon5Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon5Link->setUserObject(CCString::create("https://www.google.com"));
            Icon5Link->setPosition({ 2, -24 });
            menu->addChild(Icon5Link);

            auto Icon6 = SimplePlayer::create(110);
            auto firstColorIcon6 = ccColor3B {150, 0, 0};
            auto secondColorIcon6 = ccColor3B {185, 0, 255};
            Icon6->setChildColor(firstColorIcon6);
            Icon6->setSecondColor(secondColorIcon6);
            Icon6->setPosition({ 96, -58 });
            page4betaSection->addChild(Icon6);
            auto Icon6Label = CCLabelBMFont::create("OmegaT", "goldFont.fnt");
            Icon6Label->setScale(.8F);
            auto Icon6Link = CCMenuItemSpriteExtra::create(Icon6Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon6Link->setUserObject(CCString::create("https://www.google.com"));
            Icon6Link->setPosition({ 96, -24 });
            menu->addChild(Icon6Link);

            menu->addChild(page4betaSection);
            break;
        }
        case 4: { // check for update page
            auto page5betaSection = CCNode::create();

            auto Icon1 = SimplePlayer::create(2);
            auto firstColorIcon1 = ccColor3B {0, 125, 255};
            auto secondColorIcon1 = ccColor3B {0, 200, 255};
            Icon1->setChildColor(firstColorIcon1);
            Icon1->setSecondColor(secondColorIcon1);
            Icon1->enableCustomGlowColor(secondColorIcon1);
            Icon1->setPosition({ -93, 11 });
            page5betaSection->addChild(Icon1);
            auto Icon1Label = CCLabelBMFont::create("Gepsoni4", "goldFont.fnt");
            Icon1Label->setScale(.7F);
            auto Icon1Link = CCMenuItemSpriteExtra::create(Icon1Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon1Link->setUserObject(CCString::create("https://www.google.com"));
            Icon1Link->setPosition({ -92, 44 });
            menu->addChild(Icon1Link);

            auto Icon2 = SimplePlayer::create(50);
            auto firstColorIcon2 = ccColor3B {255, 185, 0};
            auto secondColorIcon2 = ccColor3B {0, 125, 255};
            Icon2->setChildColor(firstColorIcon2);
            Icon2->setSecondColor(secondColorIcon2);
            Icon2->setPosition({ 3, 11 });
            page5betaSection->addChild(Icon2);
            auto Icon2Label = CCLabelBMFont::create("YuyuGMD", "goldFont.fnt");
            Icon2Label->setScale(.8F);
            auto Icon2Link = CCMenuItemSpriteExtra::create(Icon2Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon2Link->setUserObject(CCString::create("https://www.google.com"));
            Icon2Link->setPosition({ 2, 44 });
            menu->addChild(Icon2Link);

            auto Icon3 = SimplePlayer::create(109);
            auto firstColorIcon3 = ccColor3B {125, 0, 255};
            auto secondColorIcon3 = ccColor3B {0, 255, 255};
            Icon3->setChildColor(firstColorIcon3);
            Icon3->setSecondColor(secondColorIcon3);
            Icon3->enableCustomGlowColor(secondColorIcon3);
            Icon3->setPosition({ 95, 11 });
            page5betaSection->addChild(Icon3);
            auto Icon3Label = CCLabelBMFont::create("DashTY", "goldFont.fnt");
            Icon3Label->setScale(.8F);
            auto Icon3Link = CCMenuItemSpriteExtra::create(Icon3Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon3Link->setUserObject(CCString::create("https://www.google.com"));
            Icon3Link->setPosition({ 95, 44 });
            menu->addChild(Icon3Link);

            auto Icon4 = SimplePlayer::create(53);
            auto firstColorIcon4 = ccColor3B {0, 125, 255};
            auto secondColorIcon4 = ccColor3B {255,255,255};
            Icon4->setChildColor(firstColorIcon4);
            Icon4->setSecondColor(secondColorIcon4);
            Icon4->enableCustomGlowColor(secondColorIcon4);
            Icon4->setPosition({ -93, -58 });
            page5betaSection->addChild(Icon4);
            auto Icon4Label = CCLabelBMFont::create("SharKing09", "goldFont.fnt");
            Icon4Label->setScale(.7F);
            auto Icon4Link = CCMenuItemSpriteExtra::create(Icon4Label, this, menu_selector(CreditsBetaMenu::goToURL));
            Icon4Link->setUserObject(CCString::create("https://www.google.com"));
            Icon4Link->setPosition({ -93, -24 });
            menu->addChild(Icon4Link);

            menu->addChild(page5betaSection);
            break;
        }
    }
    #endif
    return layer;
}

void CreditsBetaMenu::nextPage(CCObject*) {
    if (pageIndex >= maxPages) return;
    pageIndex = pageIndex + 1;
    int pageTag = 2000 + (pageIndex + 1);
    auto currLayer = this->m_mainLayer->getChildByTag(pageTag);
    if (currLayer != nullptr) {
        auto prevLayer = this->m_mainLayer->getChildByTag(pageTag - 1);
        if (prevLayer != nullptr) {
            currLayer->setVisible(true);
            prevLayer->setVisible(false);            
        }
    }
}

void CreditsBetaMenu::prevPage(CCObject*) {
    if (pageIndex <= 0) return;
    pageIndex = pageIndex - 1;
    int pageTag = 2000 + (pageIndex + 1);
    auto currLayer = this->m_mainLayer->getChildByTag(pageTag);
    if (currLayer != nullptr) {
        auto prevLayer = this->m_mainLayer->getChildByTag(pageTag + 1);
        if (prevLayer != nullptr) {
            currLayer->setVisible(true);
            prevLayer->setVisible(false);            
        }
    }
}
