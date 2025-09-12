#pragma once
#include <Geode/Geode.hpp>
#include <random>
using namespace geode::prelude;

#include <variant>
#include <mutex>
#include <thread>
#include <algorithm>

#define OPT(expr) if (auto __opt__ = expr) __opt__

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
    template<typename T>
    static std::vector<std::vector<T>> paginate(const std::vector<T>& arr, size_t size) {
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

    static std::string genClientID() {
        // this is just for statistics ok
        #ifdef GEODE_IS_WINDOWS 
        std::string platform = "Windows";
        #elif defined(GEODE_IS_ANDROID)
        std::string platform = "Android";
        #elif defined(GEODE_IS_MACOS)
        std::string platform = "Mac";
        #elif defined(GEODE_IS_IOS)
        std::string platform = "iOS";
        #else 
        std::string platform = "Unknown";
        #endif

        const std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, charset.size() - 1);

        std::string randomStr;
        for (size_t i = 0; i < 8; ++i) {
            randomStr.push_back(charset[dist(gen)]);
        }

        auto now = std::chrono::system_clock::now();
        auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        int accountID = 0;
        if (auto gjam = GJAccountManager::get()) {
            accountID = gjam->m_accountID;
        }
        return fmt::format("gdutils-{}-{}-{}-{}", accountID, platform, now_ms, randomStr);
    }
};
