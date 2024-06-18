#include "Newtab.h"

NewTabButton* NewTabButton::create(
    TabBaseColor unselected, TabBaseColor selected, CCNode* icon, cocos2d::CCObject* target,
    cocos2d::SEL_MenuHandler callback
) {
    auto ret = new NewTabButton();
    auto sprOff = NewTabButtonSprite::create(icon, unselected);
    auto sprOn = NewTabButtonSprite::create(icon, selected);
    if (ret && ret->init(sprOff, sprOn, target, callback)) {
        ret->m_offButton->m_colorDip = .3f;
        ret->m_offButton->m_colorEnabled = true;
        ret->m_offButton->m_scaleMultiplier = 1.f;
        ret->m_onButton->setEnabled(false);
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

CCSize NewBasedButtonSprite::getMaxTopSize() const {
    return m_obContentSize - CCSize(18.f, 18.f);
}

void NewBasedButtonSprite::setTopOffset(CCPoint const& offset) {
    m_topOffset = offset;
    if (m_onTop) {
        m_onTop->setPosition(this->getContentSize() / 2 + offset);
    }
}
void NewBasedButtonSprite::setTopRelativeScale(float scale) {
    m_onTopRelativeScale = scale;
    if (m_onTop) {
        limitNodeSize(m_onTop, this->getMaxTopSize(), 999.f, .1f);
        m_onTop->setScale(m_onTop->getScale() * m_onTopRelativeScale);
    }
}

bool NewBasedButtonSprite::initWithSprite(
    char const* sprName, float sprScale, BaseType type, int size, int color
) {
    auto spr = CCSprite::create(sprName);
    if (!spr) return false;
    m_onTopRelativeScale = sprScale;
    return this->init(spr, type, size, color);
}

bool NewBasedButtonSprite::initWithSpriteFrameName(
    char const* sprName, float sprScale, BaseType type, int size, int color
) {
    auto spr = CCSprite::createWithSpriteFrameName(sprName);
    if (!spr) return false;
    m_onTopRelativeScale = sprScale;
    return this->init(spr, type, size, color);
}

CCNode* NewBasedButtonSprite::getTopNode() const {
    return m_onTop;
}

NewBasedButtonSprite::~NewBasedButtonSprite() {}

NewTabButtonSprite* NewTabButtonSprite::create(CCNode* icon, TabBaseColor color, TabBaseSize size) {
    auto ret = new NewTabButtonSprite();
    if (ret && ret->init(
        icon, BaseType::Tab, 
        static_cast<int>(size), static_cast<int>(color)
    )) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

static std::string baseEnumsToString(BaseType type, int size, int color) {
#define ENUMS_TO_STRING(ty_)    \
    case BaseType::ty_: {       \
        sizeStr = baseEnumToString(static_cast<ty_##BaseSize>(size));\
        colorStr = baseEnumToString(static_cast<ty_##BaseColor>(color));\
    } break

    const char* typeStr = baseEnumToString(type);
    const char* sizeStr;
    const char* colorStr;
    switch (type) {
        ENUMS_TO_STRING(Circle);
        ENUMS_TO_STRING(Cross);
        ENUMS_TO_STRING(Account);
        ENUMS_TO_STRING(IconSelect);
        ENUMS_TO_STRING(Leaderboard);
        ENUMS_TO_STRING(Editor);
        ENUMS_TO_STRING(Tab);
        ENUMS_TO_STRING(Category);
    }
    return fmt::format("newbase{}_{}_{}.png", typeStr, sizeStr, colorStr);
}

bool NewBasedButtonSprite::init(CCNode* ontop, BaseType type, int size, int color) {
    if (!CCSprite::initWithFile(
        Mod::get()->expandSpriteName(baseEnumsToString(type, size, color)).data()
    )) return false;

    m_type = type;
    m_size = size;
    m_color = color;

    if (ontop) {
        m_onTop = ontop;
        m_onTop->setPosition(this->getContentSize() / 2 + m_topOffset);
        limitNodeSize(m_onTop, this->getMaxTopSize(), 999.f, .1f);
        m_onTop->setScale(m_onTop->getScale() * m_onTopRelativeScale);
        this->addChild(m_onTop);
    }

    return true;
}