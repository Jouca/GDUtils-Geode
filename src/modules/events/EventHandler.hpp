#pragma once
#include <defs/geode.hpp>
#include <arc/sync/mpsc.hpp>

class EventHandler : public CCObject {
    protected:
        std::string m_currentLayer;
    public:
        static EventHandler* create() {
            auto pRet = new EventHandler();
            if (pRet) {
                pRet->autorelease();
                return pRet;
            }
            CC_SAFE_DELETE(pRet);
            return nullptr;
        }
        void checkForEvent();
        static void aaa();
#if 0
        void checkForFiles() {
            #ifdef GEODE_IS_WINDOWS
            auto scene = CCDirector::sharedDirector()->getRunningScene();
            if (!scene) return;
            if (scene->getChildrenCount() == 0) return;
            auto layer = scene->getChildren()->objectAtIndex(0);
            if (layer == nullptr) return;
            std::string layerName = misc::getNodeName(layer);
            if (layerName == "cocos2d::CCLayerColor") return;
            if (layerName == "LoadingLayer") return;
            if (layerName == "LevelEditorLayer") return;
            if (layerName == "PlayLayer") return;
            if (layerName == "LoadingLayer") return;
            bool levelsProtocol = Mod::get()->template getSettingValue<bool>("levelsProtocol");
            if (!levelsProtocol) return;
            auto glm = GameLevelManager::sharedState();
            try {
                for (const auto & entry : std::filesystem::directory_iterator("gdutils")) {
                    if (entry.path().extension() == ".flag") {
                        std::string path = entry.path().string();
                        std::string filename = entry.path().filename().string();
                        std::string levelName = filename.substr(0, filename.length() - 5);

                        try {
                            std::filesystem::remove("gdutils/" + filename);
                        } catch (const std::exception& e) {
                            break;
                        }

                        std::string url = "https://www.boomlings.com/database/getGJLevels21.php";
                        std::string fields = fmt::format("secret=Wmfd2893gb7&type=0&str={}", levelName);

                        geode::utils::web::WebRequest request = web::WebRequest();
                        request.userAgent("");
                        request.header("Content-Type", "application/x-www-form-urlencoded");
                        const std::lock_guard<std::mutex> lock(lock_var);
                        RUNNING_REQUESTS.emplace(
                            "@loaderLevelProtocolURL",
                            request.bodyString(fields).post(url).map(
                                [](web::WebResponse* response) {
                                    const std::lock_guard<std::mutex> lock(lock_var);
                                    if (response->ok()) {
                                        if (response->data().empty()) {
                                            FLAlertLayer::create(nullptr,
                                                "Error (GDUtils)",
                                                "An error has occured with <cy>Robtop's servers</c>. The response returned empty.",
                                                "OK",
                                                nullptr,
                                                180.0F
                                            )->show();
                                        } else {
                                            auto data = response->string().unwrapOrDefault();
                                            if (data != "-1") {
                                                auto scene = CCScene::create();

                                                GJGameLevel* gjgl = EventsPush::convertLevelToJSON(data);

                                                auto layer = LevelInfoLayer::create(gjgl, false);
                                                layer->downloadLevel();
                                                scene->addChild(layer);
                                                CCDirector::sharedDirector()->pushScene(cocos2d::CCTransitionFade::create(0.5f, scene));
                                            } else {
                                                FLAlertLayer::create(nullptr,
                                                    "Error",
                                                    "Level not found.",
                                                    "OK",
                                                    nullptr,
                                                    180.0F
                                                )->show();
                                            }
                                        }
                                    } else {
                                        FLAlertLayer::create(nullptr,
                                        "Error (GDUtils)",
                                            "An error has occured with <cy>Robtop's servers</c>. The reason could be because you are either ratelimited, blocked, or another issue.",
                                            "OK",
                                            nullptr,
                                            180.0F
                                        )->show();
                                    }

                                    RUNNING_REQUESTS.erase("@loaderLevelProtocolURL");
                                    return *response;
                                }
                            )
                        );
                    }
                }
            } catch (const std::exception& e) {}
            #endif
        }
#endif
        void check(float dt) {
            checkForEvent();
            //checkForFiles();
        }
        static void consumer(arc::mpsc::Receiver<std::string> rx, CCScene* scene);
};

