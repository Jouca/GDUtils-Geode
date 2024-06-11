#include "OldBorder.hpp"

OldBorder* OldBorder::create(CCNode* node, const ccColor4B& backgroundColor, const CCSize& size, const CCPoint& padding) {
    OldBorder* instance = new OldBorder(padding);

    if (instance && instance->init(node, backgroundColor, size)) {
        instance->autorelease();

        return instance;
    } else {
        CC_SAFE_DELETE(instance);

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

    CCScale9Sprite* OldBorder = CCScale9Sprite::create("inverseborder.png"_spr);
    CCLayer* content = CCLayer::create();

    OldBorder->setID("border_sprite"_spr);
    OldBorder->setAnchorPoint({ 0, 0 });
    OldBorder->setPosition({ 0, 0 });
    OldBorder->setZOrder(1);
    content->setID("border_content"_spr);
    content->setContentSize(size);
    this->addChild(OldBorder);
    this->addChild(content);

    if (node != nullptr) {
        this->setNode(node);
    }

    this->setSize(size);

    return true;
}

void OldBorder::setPadding(const CCPoint& padding) {
    this->setPadding(padding.x, padding.y);
}

void OldBorder::setPadding(float x, float y) {
    this->setPaddingX(x);
    this->setPaddingY(y);
}

void OldBorder::setPadding(float padding) {
    this->setPadding(padding, padding);
}

void OldBorder::setPaddingX(float x) {
    this->setPaddingLeft(x);
    this->setPaddingRight(x);
}

OldBorder::Padding OldBorder::getPadding() {
    return m_padding;
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

float OldBorder::getPaddingTop() {
    return m_padding.top;
}

void OldBorder::setPaddingRight(float right) {
    m_padding.right = right;

    this->updatePadding();
}

float OldBorder::getPaddingRight() {
    return m_padding.right;
}

void OldBorder::setPaddingBottom(float bottom) {
    m_padding.bottom = bottom;

    this->updatePadding();
}

float OldBorder::getPaddingBottom() {
    return m_padding.bottom;
}

void OldBorder::setPaddingLeft(float left) {
    m_padding.left = left;

    this->updatePadding();
}

float OldBorder::getPaddingLeft() {
    return m_padding.left;
}

void OldBorder::setBackgroundColor(const ccColor4B& color) {
    this->setColor({ color.r, color.g, color.b });
    this->setOpacity(color.a);
}

ccColor4B OldBorder::getBackgroundColor() {
    return { _displayedColor.r, _displayedColor.g, _displayedColor.b, _displayedOpacity };
}

void OldBorder::setNode(CCNode* node) {
    CCNode* content = this->getChildByID("border_content"_spr);

    // Can't assume an ID as the node is a user input and may have its ID changed
    if (CCNode* oldNode = cocos::getChild<CCNode>(content, 0)) {
        // Not going to mess with releasing the node, I'll leave that to the user
        oldNode->removeFromParent();
    }

    content->addChild(node);

    this->updatePadding();
}

CCNode* OldBorder::getNode() {
    if (CCNode* node = cocos::getChild<CCNode>(this->getChildByID("border_content"_spr), 0)) {
        return node;
    } else {
        return nullptr;
    }
}

void OldBorder::setSize(const CCSize& size) {
    this->setContentSize(size);
    this->getChildByID("border_sprite"_spr)->setContentSize(size);
    this->getChildByID("border_content"_spr)->setContentSize(size);
    this->updatePadding();
}

void OldBorder::updatePadding() {
    if (CCNode* node = this->getNode()) {
        CCSize size = this->getContentSize();

        node->setAnchorPoint({ 0, 0 });
        node->setPosition({ m_padding.left, m_padding.bottom });
        node->setContentSize(size - ccp(this->getPaddingX(), this->getPaddingY()) * 2);
    }
}