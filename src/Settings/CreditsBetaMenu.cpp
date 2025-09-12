#include "CreditsBetaMenu.h"
#include "../includes.h"

struct CreditData {
    std::string username;
    unsigned int iconID;
    ccColor3B col1;
    ccColor3B col2;
    ccColor3B glow;
};

std::vector<CreditData> users = {
    { "TMNono", 133, {0, 0, 0}, {255, 255, 255}, {255, 255, 255} },
    { "Devilmine", 111, {90, 90, 90}, {175, 0, 75}, {175, 0, 75} },
    { "SupuGD", 50, {125, 125, 255}, {125, 255, 175}, {125, 255, 175} },
    { "Silvow", 140, {0, 75, 175}, {255, 255, 0}, {255, 255, 0} },
    { "SantyBoyMX", 46, {90, 90, 90}, {175, 175, 175}, {175, 175, 175} },
    { "Tygore", 57, {0, 75, 175}, {255, 255, 255}, {255, 255, 255} },
    { "Hir0Shi", 141, {0, 125, 125}, {0, 255, 255}, {0, 255, 255} },
    { "Komatic5", 48, {0, 125, 255}, {255, 255, 0}, {255, 255, 0} },
    { "Xanii", 4, {255, 125, 125}, {255, 255, 255}, {255, 255, 255} },
    { "maverick", 127, {255, 0, 125}, {0, 255, 255}, {0, 255, 255} },
    { "arcanin", 115, {0, 255, 255}, {0, 200, 255}, {0, 200, 255} },
    { "Ficelo", 54, {125, 125, 255}, {125, 255, 175}, {125, 255, 175} },
    { "JustL3o", 105, {125, 0, 255}, {0, 255, 255}, {0, 255, 255} },
    { "TheRealWeenside", 28, {135, 0, 0}, {255, 255, 255}, {255, 255, 255} },
    { "Klafterno", 123, {255, 255, 255}, {0, 255, 255}, {0, 255, 255} },
    { "MarkyForce", 77, {255, 255, 0}, {125, 125, 255}, {125, 125, 255} },
    { "Cobalium", 98, {255, 255, 255}, {0, 255, 255}, {0, 255, 255} },
    { "jihotv01", 1, {125, 255, 0}, {0, 255, 255}, {0, 255, 255} },
    { "Paultam", 31, {255, 255, 255}, {255, 0, 0}, {255, 0, 0} },
    { "ItzElectrix", 38, {0, 200, 255}, {255, 0, 0}, {255, 0, 0} },
    { "Ausk", 123, {255, 0, 125}, {255, 255, 0}, {255, 255, 0} },
    { "krenogd", 4, {255, 255, 255}, {0, 75, 175}, {0, 75, 175} },
    { "realwhata", 44, {90, 90, 90}, {175, 175, 175}, {175, 175, 175} },
    { "OmegaT", 110, {150, 0, 0}, {185, 0, 255}, {185, 0, 255} },
    { "Gepsoni4", 2, {0, 125, 255}, {0, 200, 255}, {0, 200, 255} },
    { "YuyuGMD", 50, {255, 185, 0}, {0, 125, 255}, {0, 125, 255} },
    { "DashTY", 109, {125, 0, 255}, {0, 255, 255}, {0, 255, 255} },
    { "SharKing09", 53, {0, 125, 255}, {255, 255, 255}, {255, 255, 255} },
};

auto paginated = misc::paginate<CreditData>(users, 6);

bool CreditsBetaMenu::setup() {
    auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();

    auto title = CCLabelBMFont::create("Beta Credits", "bigFont.fnt");
    title->setScale(0.8f);
    m_mainLayer->addChildAtPosition(title, Anchor::Top, {0, -25});

    auto prevArrow = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    auto nextArrow = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    nextArrow->setFlipX(true);
    m_buttonMenu->addChildAtPosition(CCMenuItemExt::createSpriteExtra(prevArrow, [this](CCObject*) {
        if (m_currentPage > 0) {
            m_currentPage--;
        } else {
            m_currentPage = (paginated.size() - 1);
        }
        updatePage();
    }), Anchor::Left, {-30, 0});
    m_buttonMenu->addChildAtPosition(CCMenuItemExt::createSpriteExtra(nextArrow, [this](CCObject*) {
        if ((m_currentPage + 1) <= (paginated.size() - 1)) {
            m_currentPage++;
        } else {
            m_currentPage = 0;
        }
        updatePage();
    }), Anchor::Right, {30, 0});
    m_section = CCNode::create();
    m_mainLayer->addChildAtPosition(m_section, Anchor::Center);
    updatePage();
    return true;
}

void CreditsBetaMenu::updatePage() {
    m_section->removeAllChildrenWithCleanup(true);
    for (int i = 0; i < paginated[m_currentPage].size(); i++) {
        auto player = paginated[m_currentPage][i];
        CCPoint iconPos;
        CCPoint labelPos;
        switch (i) {
            case 0:
                iconPos = CCPoint(-93, 11);
                labelPos = CCPoint(-92, 44);
                break;
            case 1:
                iconPos = CCPoint(3, 11);
                labelPos = CCPoint(2, 44);
                break;
            case 2:
                iconPos = CCPoint(95, 11);
                labelPos = CCPoint(95, 44);
                break;
            case 3:
                iconPos = CCPoint(-93, -58);
                labelPos = CCPoint(-93, -24);
                break;
            case 4:
                iconPos = CCPoint(2, -58);
                labelPos = CCPoint(2, -24);
                break;
            case 5:
                iconPos = CCPoint(96, -58);
                labelPos = CCPoint(96, -24);
                break;
        }
        auto icon = SimplePlayer::create(player.iconID);
        icon->setChildColor(player.col1);
        icon->setSecondColor(player.col2);
        icon->setGlowOutline(player.glow);
        icon->setPosition(iconPos);
        icon->updateColors();
        m_section->addChild(icon);
        auto label = CCLabelBMFont::create(player.username.c_str(), "goldFont.fnt");
        label->limitLabelWidth(80, .8F, .45f);
        label->setPosition(labelPos);
        m_section->addChild(label);
    }
}
