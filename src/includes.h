#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <sio_client.h>
#include <variant>
#include <mutex>
#include <thread>
#ifdef GEODE_IS_WINDOWS
#include <synchapi.h>
#endif

class misc {
    public:
    // theres no including Geode Util class funcs so, https://github.com/geode-sdk/DevTools
    static std::string getNodeName(cocos2d::CCObject* node) {
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
    static CCObject* findNode(std::string name) {
        auto scene = CCDirector::sharedDirector()->getRunningScene();
        auto sceneChildren = scene->getChildren();
        for (unsigned int i = 0; i < scene->getChildrenCount(); i++) {
            auto node = sceneChildren->objectAtIndex(i);
            if (node != nullptr) {
                std::string layerName = misc::getNodeName(node);
                std::cout << layerName << std::endl;
                if (layerName == name) {
                    return node;
                }
            }
        }
        return nullptr;
    }
};
