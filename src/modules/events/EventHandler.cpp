#include "EventHandler.hpp"
#include "EventsPush.hpp"
#include <network/AMQClient.hpp>

std::string getNodeNameA(cocos2d::CCObject* node) {
#ifdef GEODE_IS_WINDOWS
    return typeid(*node).name() + 6;
#else 
    {
        std::string ret;

        int status = 0;
        auto demangle = abi::__cxa_demangle(typeid(*node).name(), 0, 0, &status);
        if (status == 0) {
            ret = demangle;
        }
        free(demangle);

        return ret;
    }
#endif
}

void processEvent(CCScene* self) {
    if (!AMQT::getQueue().empty()) {
        EventsPush::pushRateLevel(self, AMQT::getQueueFrontPop());
    }
}

//TODO:
void processChestEvent(CCScene* self) {
    /*if (!chestQueue.empty()) {
        auto data = chestQueue.front();
        chestQueue.pop();
        auto dailyChest = new DailyChest();
        dailyChest->getRewards(0);
    }*/
}


void EventHandler::checkForEvent() {
    auto director = CCDirector::sharedDirector();
    if (!director) return;
    auto scene = CCDirector::sharedDirector()->getRunningScene();
    if (!scene) return;
    if (scene->getChildrenCount() == 0) return;
    if (AMQT::genConnected()) {
        AMQT::setGenConnected(false);
        if (Mod::get()->template getSettingValue<bool>("server-notification")) {
            Notification::create("Connected to Rate Server!", NotificationIcon::Success)->show();
        }
    }
    auto layer = scene->getChildren()->objectAtIndex(0);
    std::string layerName = getNodeNameA(layer);
    if (layerName == "cocos2d::CCLayerColor") return;
    if (layerName == "LoadingLayer") return;
    if (m_currentLayer != layerName) {
        m_currentLayer = layerName;
        EventsPush::eventCompletedCallback(scene);
        bool everywhereElse = Mod::get()->template getSettingValue<bool>("everywhereElse");
        if ((layerName != "LevelEditorLayer" && layerName != "PlayLayer") && !everywhereElse) return;
        if ((layerName != "LevelEditorLayer" && layerName != "PlayLayer") && everywhereElse) {
            processChestEvent(scene);
            processEvent(scene);
        }
    }
    if (AMQT::getQueue().empty()) return;
    bool everywhereElse = Mod::get()->getSettingValue<bool>("everywhereElse");
    bool inLevels = Mod::get()->getSettingValue<bool>("inLevels");
    bool inEditor = Mod::get()->getSettingValue<bool>("inEditor");
    bool inPlatformers = Mod::get()->getSettingValue<bool>("inPlatformers");
    bool pushEvent = true;
    if (layerName == "PlayLayer") {
        if (!inLevels) pushEvent = false;
    }
    if (layerName == "LevelEditorLayer" && !inEditor) {
        pushEvent = false;
    }
    if ((layerName != "LevelEditorLayer" && layerName != "PlayLayer") && !everywhereElse) {
        pushEvent = false;
    }
    if (pushEvent) processEvent(scene);
}

void EventHandler::consumer(arc::mpsc::Receiver<std::string> rx, CCScene* scene) {

}

$execute {
    /*Loader::get()->queueInMainThread([]{
        CCScheduler::get()->scheduleSelector(schedule_selector(EventHandler::check), EventHandler::create(), 1.0F, false);
        //CCScheduler::get()->scheduleUpdateForTarget(EventHandler::create(), Priority::Last, false);
    });*/
}
