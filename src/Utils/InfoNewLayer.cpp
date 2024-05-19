#include "InfoNewLayer.h"
#include <Geode/modify/InfoLayer.hpp>

void InfoNewLayer::onCopyLevelURL(CCObject* pSender) {
    std::string level = static_cast<CCString*>(static_cast<CCNode*>(pSender)->getUserObject())->getCString();
    utils::clipboard::write("https://gdutils.com/" + level);
    
    auto notification = Notification::create("Copied level URL to clipboard!", NotificationIcon::Success);
    notification->setZOrder(1000);
    notification->setTime(0.2f);
    notification->show();
}

// Copy button for sharing levels
class $modify(InfoLayer) {
    bool init(GJGameLevel* level, GJUserScore* userscore, GJLevelList* levellist) {
        if (!InfoLayer::init(level, userscore, levellist)) return false;

        // Check if it's InfoLayer for level
        if (level == nullptr) return true;

        std::string levelID = std::to_string(level->m_levelID.value());

        CCMenu* menu = typeinfo_cast<CCMenu*>(this->getChildByIDRecursive("left-side-menu"));
        auto copySpr = CCSprite::createWithSpriteFrameName("GJ_redoBtn_001.png");
        copySpr->setScale(.7f);
        auto copyBtn = CCMenuItemSpriteExtra::create(
            copySpr,
            this,
            menu_selector(InfoNewLayer::onCopyLevelURL)
        );
        copyBtn->setUserObject(cocos2d::CCString::create(gd::string(levelID)));
        copyBtn->setPosition(14, -11);
        menu->addChild(copyBtn);
        
        return true;
    }
};
