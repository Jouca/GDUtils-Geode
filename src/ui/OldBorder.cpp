#include "OldBorder.hpp"

OldBorder* OldBorder::create(CCNode* node, const ccColor4B& backgroundColor, const CCSize& size, const CCPoint& padding) {
    OldBorder* instance = new OldBorder(padding);

    if (instance && instance->init(node, backgroundColor, size)) {
        instance->autorelease();
        return instance;
    } else {
        delete instance;
        return nullptr;
    }
}

OldBorder::OldBorder(const CCPoint& padding) : m_padding({ padding.x, padding.y, padding.x, padding.y }) { }

bool OldBorder::init(const ccColor4B& backgroundColor, const CCSize& size) {
    return this->init(nullptr, backgroundColor, size);
}

bool OldBorder::init(CCNode* node, const ccColor4B& backgroundColor, const CCSize& size) {
    if (!this->initWithColor(backgroundColor)) {
        return false;
    }
    Build<CCScale9Sprite>::create("inverseborder.png"_spr)
        .id("border_sprite"_spr)
        .anchorPoint({0,0})
        .pos(0,0)
        .zOrder(1)
        .parent(this)
        .store(m_inverseBorder);
    Build<CCLayer>::create().id("border_content"_spr).contentSize(size).parent(this).store(m_content);
    if (node != nullptr) {
        this->setNode(node);
    }
    this->setSize(size);
    return true;
}


void OldBorder::setPadding(float x, float y) {
    this->setPaddingX(x);
    this->setPaddingY(y);
}

void OldBorder::setPaddingX(float x) {
    this->setPaddingLeft(x);
    this->setPaddingRight(x);
}

float OldBorder::getPaddingX() {
    return (m_padding.left + m_padding.right) / 2;
}

void OldBorder::setPaddingY(float y) {
    this->setPaddingTop(y);
    this->setPaddingBottom(y);
}

float OldBorder::getPaddingY() {
    return (m_padding.top + m_padding.bottom) / 2;
}

void OldBorder::setPaddingTop(float top) {
    m_padding.top = top;

    this->updatePadding();
}


void OldBorder::setPaddingRight(float right) {
    m_padding.right = right;

    this->updatePadding();
}

void OldBorder::setPaddingBottom(float bottom) {
    m_padding.bottom = bottom;

    this->updatePadding();
}

void OldBorder::setPaddingLeft(float left) {
    m_padding.left = left;

    this->updatePadding();
}
void OldBorder::setBackgroundColor(const ccColor4B& color) {
    this->setColor({ color.r, color.g, color.b });
    this->setOpacity(color.a);
}

ccColor4B OldBorder::getBackgroundColor() {
    return { _displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity };
}

void OldBorder::setNode(CCNode* node) {
    // Can't assume an ID as the node is a user input and may have its ID changed
    if (CCNode* oldNode = m_content->getChildByType<CCNode*>(0)) {
        // Not going to mess with releasing the node, I'll leave that to the user
        oldNode->removeFromParent();
    }
    m_content->addChild(node);
    this->updatePadding();
}

void OldBorder::setSize(const CCSize& size) {
    this->setContentSize(size);
    m_inverseBorder->setContentSize(size);
    m_content->setContentSize(size);
    this->updatePadding();
}

void OldBorder::updatePadding() {
    CCSize size = this->getContentSize();
    m_content->setAnchorPoint({ 0, 0 });
    m_content->setPosition({ m_padding.left, m_padding.bottom });
    m_content->setContentSize(size - ccp(this->getPaddingX(), this->getPaddingY()) * 2);
}
