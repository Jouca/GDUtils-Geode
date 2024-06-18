#pragma once
#include "../includes.h"

class NewTabButton : public CCMenuItemToggler {
    public:
        static NewTabButton* create(
            TabBaseColor unselected, TabBaseColor selected, CCNode* icon,
            cocos2d::CCObject* target, cocos2d::SEL_MenuHandler callback
        );
};

class NewBasedButtonSprite : public cocos2d::CCSprite {
    protected:
        BaseType m_type;
        int m_size;
        int m_color;
        cocos2d::CCNode* m_onTop = nullptr;
        float m_onTopRelativeScale = 1.3f;
        cocos2d::CCPoint m_topOffset = cocos2d::CCPointZero;

        bool init(cocos2d::CCNode* ontop, BaseType type, int size, int color);
        bool initWithSprite(
            const char* sprName, float sprScale, BaseType type, int size, int color
        );
        bool initWithSpriteFrameName(
            const char* sprName, float sprScale, BaseType type, int size, int color
        );

        virtual cocos2d::CCSize getMaxTopSize() const;

        virtual ~NewBasedButtonSprite();

    public:
        /**
         * Create a BasedButtonSprite. In general, it is recommended to use 
         * the specialized subclasses, such as CircleButtonSprite or 
         * TabButtonSprite
         * @param ontop The top node of the sprite
         * @param type The type of the base sprite
         * @param size The size of the base sprite. Cast this from the base 
         * type's respective BaseSize enum
         * @param color The color of the base sprite. Cast this from the base 
         * type's respective BaseColor enum
         * @returns Created BasedButtonSprite, or nullptr if parameters were 
         * invalid 
         */
        static NewBasedButtonSprite* create(
            cocos2d::CCNode* ontop, BaseType type, int size, int color
        );

        /**
         * Set an offset to the top sprite
         */
        void setTopOffset(cocos2d::CCPoint const& offset);
        void setTopRelativeScale(float scale);

        cocos2d::CCNode* getTopNode() const;
};

class NewTabButtonSprite : public NewBasedButtonSprite {
    public:
        static NewTabButtonSprite* create(
            CCNode* icon,
            TabBaseColor color,
            TabBaseSize size = TabBaseSize::Normal
        );
};