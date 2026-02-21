#pragma once
#include <defs/geode.hpp>

class OldBorder : public cocos2d::CCLayerColor {
public:
    struct Padding {
        float top;
        float right;
        float bottom;
        float left;
    };

    static OldBorder* create(cocos2d::CCNode* node, const cocos2d::ccColor4B& backgroundColor, const cocos2d::CCSize& size = { 0, 0 }, const cocos2d::CCPoint& padding = { 0, 0 });

    void setPadding(const cocos2d::CCPoint& padding) { this->setPadding(padding.x, padding.y); }
    void setPadding(float x, float y);
    void setPadding(float padding) { this->setPadding(padding, padding); }
    Padding getPadding() { return m_padding; }
    void setPaddingX(float x);
    float getPaddingX();
    void setPaddingY(float y);
    float getPaddingY();
    void setPaddingTop(float top);
    float getPaddingTop() { return m_padding.top; }
    void setPaddingRight(float right);
    float getPaddingRight() { return m_padding.right; }
    void setPaddingBottom(float bottom);
    float getPaddingBottom() { return m_padding.bottom; }
    void setPaddingLeft(float left);
    float getPaddingLeft() { return m_padding.left; }
    void setBackgroundColor(const cocos2d::ccColor4B& color);
    cocos2d::ccColor4B getBackgroundColor();
    void setNode(CCNode* node);
    void setSize(const cocos2d::CCSize& size);
protected:
    Padding m_padding;
    CCLayer* m_content;
    CCScale9Sprite* m_inverseBorder;
    OldBorder(const cocos2d::CCPoint& padding = { 0, 0 });
    bool init(const cocos2d::ccColor4B& backgroundColor, const cocos2d::CCSize& size);
    bool init(CCNode* node, const cocos2d::ccColor4B& backgroundColor, const cocos2d::CCSize& size);
private:
    void updatePadding();
};
