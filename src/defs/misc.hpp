#pragma once
#include <string>
#include <vector>
#include "geode.hpp"

namespace misc {
    // theres no including Geode Util class funcs so, https://github.com/geode-sdk/DevTools
    /*static 
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
    }*/
    template<typename T>
    std::vector<std::vector<T>> paginate(const std::vector<T>& arr, size_t size) {
        std::vector<std::vector<T>> paginated;
        for (size_t i = 0; i < arr.size(); i += size) {
            std::vector<T> page;
            for (size_t j = i; j < std::min(i + size, arr.size()); ++j) {
                page.push_back(arr[j]);
            }
            paginated.push_back(page);
        }
        return paginated;
    }
};
