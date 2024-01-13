#include "InfoNewLayer.h"

void InfoNewLayer::onCopyLevelURL(CCObject* pSender) {
    std::string level = static_cast<CCString*>(static_cast<CCNode*>(pSender)->getUserObject())->getCString();
    utils::clipboard::write("https://gdutils.com/" + level);
    
    auto notification = Notification::create("Copied level URL to clipboard!", NotificationIcon::Success);
    notification->setZOrder(1000);
    notification->setTime(0.2f);
    notification->show();
}