// GDUtils Geode Port
// Ported with help by Firee
// Mod made by Jouca & Firee

#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/CCSprite.hpp>
#include <Geode/modify/CCScale9Sprite.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/loader/Log.hpp>
#include <Geode/utils/web.hpp>
#include "includes.h"
#include "Settings/CustomSettings.hpp"
#include "Notifications/EventsPush.h"
#include "ProcessLambdas.h"
#include <cstdint>
#include <fmt/format.h>
#include <chrono>
#include <memory>
#include <random>
#include <thread>
#include <queue>
#include <unordered_map>
#include <string>
#include "Notifications/DailyChest.h"
#include <codecvt>

bool show_connected = false;

#ifdef GEODE_IS_WINDOWS // stop the dumb compiler errors!
#include <WinSock2.h>
#include <WS2tcpip.h>
//#include <synchapi.h>
#endif

//#include <uv.h>
//#include <amqpcpp.h>
//#include <amqpcpp/libuv.h>

// ah yes, lets use the raw version of the library instead of the c++ one because IT DOESNT IMPLEMENT SOCKETS
#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>

//#include <amqpcpp/connectionimpl.h>

std::queue<std::string> dataQueue;
std::queue<int> chestQueue;

std::queue<std::string> msgQueue;

std::string AMQReplyToString(amqp_response_type_enum val) {
    switch (val) {
        case AMQP_RESPONSE_NONE:
        default:
            return "AMQP_RESPONSE_NONE";
        case AMQP_RESPONSE_NORMAL:
            return "AMQP_RESPONSE_NORMAL";
        case AMQP_RESPONSE_LIBRARY_EXCEPTION:
            return "AMQP_RESPONSE_LIBRARY_EXCEPTION";
        case AMQP_RESPONSE_SERVER_EXCEPTION:
            return "AMQP_RESPONSE_SERVER_EXCEPTION";
    }
}
std::string AMQErrorToString(amqp_rpc_reply_t reply) {
    std::string errorStr;
    switch (reply.reply_type) {
        case AMQP_RESPONSE_NONE: errorStr = "No response"; break;
        case AMQP_RESPONSE_LIBRARY_EXCEPTION: errorStr = amqp_error_string2(reply.library_error); break;
        case AMQP_RESPONSE_SERVER_EXCEPTION:
            if (reply.reply.id == AMQP_CONNECTION_CLOSE_METHOD) {
                amqp_connection_close_t* m = (amqp_connection_close_t*) reply.reply.decoded;
                errorStr = (char*)m->reply_text.bytes;
            } else {
                errorStr = "Unknown Error";
            }
            break;
    }
    return fmt::format("{} ({})", errorStr, AMQReplyToString(reply.reply_type));
}

class AMQT {
    private:
    amqp_connection_state_t m_connection;
    //std::unique_ptr<amqp_socket_t> m_socket;
    amqp_socket_t* m_socket;
    std::string client_id;

    std::atomic<int> m_remainingAttempts = {10};
    std::atomic<bool> m_connected{false};

    std::mutex m_mutex;
    std::condition_variable m_cv;

    public:
    AMQT(const std::string& client_id) : client_id(client_id) {};
    bool isConnected() {
        return m_connected.load();
    }
    void setupChannel() {
        //bool showPastNotifs = Mod::get()->template getSettingValue<bool>("past-notifications");
        bool showPastNotifs = false;
        std::string queueName = fmt::format("amqp-queue-{}", client_id);
        {
            amqp_channel_open(m_connection, 1);
            auto reply = amqp_get_rpc_reply(m_connection);
            if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
                log::error("Error opening channel: {}", AMQErrorToString(reply));
                return;
            }
        }
        // https://stackoverflow.com/questions/72237234/declare-a-queue-with-x-max-length-programmatically-using-rabbitmq-c
        amqp_queue_declare_ok_t *r;
        if (showPastNotifs) {
            // this is absolutely cursed
            /*amqp_table_entry_t q_arg_n_entries[2];
            q_arg_n_entries[0] = (amqp_table_entry_t) {
                .key = amqp_cstring_bytes("x-queue-type"),
                .value = {.kind = AMQP_FIELD_KIND_UTF8, .value = {.bytes = amqp_cstring_bytes("classic")}}
            };

            //86400000
            // 2 days
            q_arg_n_entries[1] = (amqp_table_entry_t) {
                .key = amqp_cstring_bytes("x-expires"),
                .value = {.kind = AMQP_FIELD_KIND_I32, .value = {.i32 = 172800000}}
            };
            amqp_table_t q_arg_table = {.num_entries = 2, .entries = q_arg_n_entries};*/

            amqp_table_entry_t q_arg_n_entries[1];
            // 1 day
            q_arg_n_entries[0] = (amqp_table_entry_t) {
                .key = amqp_cstring_bytes("x-expires"),
                .value = {.kind = AMQP_FIELD_KIND_I32, .value = {.i32 = 86400000}}
            };
            amqp_table_t q_arg_table = {.num_entries = 1, .entries = q_arg_n_entries};
            r = amqp_queue_declare(
                m_connection, 1,
                amqp_cstring_bytes(queueName.c_str()),
                0, 1, 0, 0,
                q_arg_table
            );
        } else {
            amqp_queue_delete(m_connection, 1, amqp_cstring_bytes(queueName.c_str()), 0, 0);
            amqp_get_rpc_reply(m_connection);
            r = amqp_queue_declare(
                m_connection, 1,
                amqp_cstring_bytes(queueName.c_str()),
                0, 0, 0, 1,
                amqp_empty_table
            );
        }
        {
            auto reply = amqp_get_rpc_reply(m_connection);
            if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
                log::error("Error declaring queue: {}", AMQErrorToString(reply));
                return;
            }
        }
        amqp_bytes_t queueBytes = amqp_bytes_malloc_dup(r->queue);
        if (queueBytes.bytes == NULL) {
            log::error("Couldn't copy queue name (Out of memory?)");
            return;
        }
        {
            amqp_queue_bind(m_connection, 1,
                queueBytes, amqp_cstring_bytes("amq.topic"),
                amqp_cstring_bytes("rate.#"), amqp_empty_table
            );
            auto reply = amqp_get_rpc_reply(m_connection);
            if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
                log::error("Error binding queue: {}", AMQErrorToString(reply));
                return;
            }
        }
        {
            auto res = amqp_basic_consume(m_connection, 1, queueBytes, amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
            auto reply = amqp_get_rpc_reply(m_connection);
            
            if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
                log::error("Error consuming in queue: {}", AMQErrorToString(reply));
                return;
            }
            if (res) {
                std::string data((char*)res->consumer_tag.bytes, res->consumer_tag.len);
                show_connected = true;
                log::info("Subscribed to Rate Notifications! (Consumer Tag: {})", data);
            }
        }

        while (isConnected()) {
            amqp_envelope_t envelope;
            amqp_maybe_release_buffers(m_connection);
            //struct timeval timeout = {1, 0};
            auto ret = amqp_consume_message(m_connection, &envelope, NULL, 0);
            if (AMQP_RESPONSE_NORMAL != ret.reply_type) {
                log::error("Response wasn't normal! {}", AMQErrorToString(ret));
                if (AMQP_RESPONSE_LIBRARY_EXCEPTION == ret.reply_type && AMQP_STATUS_UNEXPECTED_STATE == ret.library_error) {
                    amqp_frame_t frame;
                    if (AMQP_STATUS_OK != amqp_simple_wait_frame(m_connection, &frame)) {
                        log::error("Failed to wait for frame after unexpected state: {}", AMQErrorToString(ret));
                        break;
                    }
                    if (AMQP_FRAME_METHOD == frame.frame_type) {
                        if (frame.payload.method.id == AMQP_BASIC_ACK_METHOD) continue;
                        if (frame.payload.method.id == AMQP_BASIC_RETURN_METHOD) {
                            {
                                log::info("Got frame payload, re-reading message based on channel...");
                                amqp_message_t message;
                                ret = amqp_read_message(m_connection, frame.channel, &message, 0);
                                if (AMQP_RESPONSE_NORMAL != ret.reply_type) {
                                    log::error("Error reading returned message: {}", AMQErrorToString(ret));
                                    break;
                                }
                                std::string data((char*)message.body.bytes, message.body.len);
                                msgQueue.push(data);
                                amqp_destroy_message(&message);
                            }
                            continue;
                        } else {
                            log::error("(Frame ID: {}) Error consuming message: {}", frame.payload.method.id, AMQErrorToString(ret));
                            break;
                        }
                    }
                } else {
                    log::error("Error consuming message: {}", AMQErrorToString(ret));
                    break;
                }
            } else {
                log::info("call rate event");
                std::string data((char*)envelope.message.body.bytes, envelope.message.body.len);
                msgQueue.push(data);
                amqp_destroy_envelope(&envelope);
            }
        }
        m_connected = false;
        m_cv.notify_all();
    }
    void connect() {
        m_remainingAttempts++;
        while (m_remainingAttempts-- > 0) {
            log::info("Starting AMQP...");
            if (m_remainingAttempts != 10) {
                if (m_connection) {
                    amqp_destroy_connection(m_connection);
                    m_connection = nullptr;
                }
                int delayMs = 3000 * ((m_remainingAttempts + 1) - m_remainingAttempts);
                //TODO: fix it being stuck on 3 seconds
                log::warn("Disconnected from server. Attempting to reconnect... ({} left, reconnecting in {} seconds)", m_remainingAttempts, (float)((delayMs / 1000.F)));
                std::this_thread::sleep_for(std::chrono::seconds(delayMs / 1000));
            }
            m_connection = amqp_new_connection();
            amqp_socket_t* socket = amqp_tcp_socket_new(m_connection);
            if (!socket) {
                log::error("Couldn't create TCP connection");
                continue;
            }
            int status;
            status = amqp_socket_open(socket, "gdutils.clarifygdps.com", 5672);
            if (status != AMQP_STATUS_OK) {
                log::error("Error opening socket! {}", amqp_error_string2(status));
                continue;
            }
            if (socket && m_connection) {
                amqp_rpc_reply_t reply = amqp_login(m_connection, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN, "gd", "GeometryDashisahorizontalrunnerstylegamedevelopedandpublishedbyRobTopGames");
                if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
                    const char* errorStr = nullptr;
                    log::error("Couldn't login to AMQP server! {}", AMQErrorToString(reply));
                    continue;
                }
                m_connected = true;
                setupChannel();
                std::unique_lock<std::mutex> lock(m_mutex);
                if (!m_cv.wait_for(lock, std::chrono::seconds(10), [this]{
                    return m_connection; // change thispls
                })) {
                    log::error("Connection timeout, assuming I can't connect!");
                }
                while (isConnected()) {
                    m_cv.wait_for(lock, std::chrono::seconds(1), [this] { return !isConnected(); });
                }
                
            }
        }
        log::error("Maximum reconnection attempts reached, cannot reconnect. Restart the game to reconnect to the MQTT server.");
    }
};

static std::unordered_map<std::string, web::WebTask> RUNNING_REQUESTS {};
static std::mutex lock_var;

std::string xorEncrypt(const std::string& input, const std::string& key) {
    std::string result;
    for (size_t i = 0; i < input.size(); ++i) {
        result += input[i] ^ key[i % key.size()];
    }
    return result;
}

// Daily chests notifications
/*void dailyChestThread() {
    while (true) {
        chestQueue.push(1);
        std::this_thread::sleep_for(std::chrono::minutes(1));
    }
}*/

void processEvent(CCScene* self) {
    if (!dataQueue.empty()) {
        auto data = dataQueue.front();
        dataQueue.pop();
        EventsPush::pushRateLevel(self, data);
    }
}

void processChestEvent(CCScene* self) {
    if (!chestQueue.empty()) {
        auto data = chestQueue.front();
        chestQueue.pop();
        auto dailyChest = new DailyChest();
        dailyChest->getRewards(0);
    }
}

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
        void checkForEvent() {
            auto director = CCDirector::sharedDirector();
            if (!director) return;
            bool waitUntilExit = false;
            auto scene = CCDirector::sharedDirector()->getRunningScene();
            if (!scene) return;
            if (scene->getChildrenCount() == 0) return;
            if (show_connected) {
                show_connected = false;
                if (Mod::get()->template getSettingValue<bool>("server-notification")) {
                    Notification::create("Connected to Rate Server!", NotificationIcon::Success)->show();
                }
            }
            auto layer = scene->getChildren()->objectAtIndex(0);
            if (ProcessLambdas::shouldProcessMenuHandler()) {
                ProcessLambdas::processMenuHandler();
            }
            std::string layerName = misc::getNodeName(layer);
            if (layerName == "cocos2d::CCLayerColor") return;
            if (layerName == "LoadingLayer") return;
            if (m_currentLayer != layerName) {
                m_currentLayer = layerName;
                EventsPush::stopNow(scene);
                bool everywhereElse = Mod::get()->template getSettingValue<bool>("everywhereElse");
                if ((layerName != "LevelEditorLayer" && layerName != "PlayLayer") && !everywhereElse) return;
                if ((layerName != "LevelEditorLayer" && layerName != "PlayLayer") && everywhereElse) {
                    processChestEvent(scene);
                    processEvent(scene);
                }
            }
            if (msgQueue.empty()) return;
            bool everywhereElse = Mod::get()->template getSettingValue<bool>("everywhereElse");
            bool inLevels = Mod::get()->template getSettingValue<bool>("inLevels");
            bool inEditor = Mod::get()->template getSettingValue<bool>("inEditor");
            bool inPlatformers = Mod::get()->template getSettingValue<bool>("inPlatformers");

            bool pushEvent = true;
            if (layerName == "PlayLayer") {
                if (!inLevels) pushEvent = false;

                PlayLayer* playLayer = reinterpret_cast<PlayLayer*>(layer);
                GJGameLevel* level = playLayer->m_level;
                if (level->isPlatformer() && !inPlatformers) {
                    pushEvent = false;
                }
            }
            if (layerName == "LevelEditorLayer" && !inEditor) {
                pushEvent = false;
            }
            if ((layerName != "LevelEditorLayer" && layerName != "PlayLayer") && !everywhereElse) {
                pushEvent = false;
            }
            dataQueue.push(msgQueue.front());
            msgQueue.pop();
            if (pushEvent) processEvent(scene);
        }
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
        void check(float dt) {
            checkForEvent();
            checkForFiles();
        }
};

// Backgrounds
class $modify(CCSprite) {
    static CCSprite* create(char const* name) {
        auto ret = CCSprite::create(name);
        if (ret == nullptr) return ret;
        // better code, thanks camila!
        if (!strcmp(name, "GJ_gradientBG.png") && Mod::get()->template getSettingValue<bool>("activate-background")) {
	    ret->retain();
            Loader::get()->queueInMainThread([=] {
                if (ret != nullptr) {
                    if (ret->getColor() == ccc3(0, 102, 255)) {
                        auto scene = CCDirector::sharedDirector()->getRunningScene();
                        if (scene->getChildrenCount() == 0) return;
                        auto layer = scene->getChildren()->objectAtIndex(0);
                        std::string layerName = misc::getNodeName(layer);
                        ret->setColor(Mod::get()->template getSettingValue<ccColor3B>("background"));
                    }
                    ret->release();
                }
            });

        }
        return ret;
    }
};

// Child background
class $modify(CCScale9Sprite) {
    static cocos2d::extension::CCScale9Sprite* create(char const* name, CCRect rect) {
        auto ret = cocos2d::extension::CCScale9Sprite::create(name, rect);
        if (ret == nullptr) return ret;
        // better code, thanks camila!
        if ((!strcmp(name, "square02b_001.png") || !strcmp(name, "square02b_small.png")) && Mod::get()->template getSettingValue<bool>("activate-background")) {
	    ret->retain();
            Loader::get()->queueInMainThread([=] {
                if (ret != nullptr) {
                    // jouca waht
                    if (ret->getColor() == ccc3(0, 56, 141)) {
                        ccColor3B new_color = Mod::get()->template getSettingValue<ccColor3B>("child-background");
                        if (Mod::get()->template getSettingValue<bool>("syncBGcolor")) {
                            new_color = Mod::get()->template getSettingValue<ccColor3B>("background");
                        }
                        if (Mod::get()->template getSettingValue<bool>("addChildBDGradient")) {
                            new_color = ccc3(new_color.r * 0.79, new_color.g * 0.79, new_color.b * 0.79);
                        }
                        if (Mod::get()->template getSettingValue<bool>("bgchild-transparency")) {
                            int opacity = Mod::get()->template getSettingValue<int64_t>("bgchild-transparency-scroll");
                            ret->setOpacity(opacity);
                        }
                        ret->setColor(new_color);
                    } else if (ret->getColor() == ccc3(0, 39, 98)) {
                        ccColor3B new_color = Mod::get()->template getSettingValue<ccColor3B>("child-background");
                        if (Mod::get()->template getSettingValue<bool>("syncBGcolor")) {
                            new_color = Mod::get()->template getSettingValue<ccColor3B>("background");
                        }
                        if (Mod::get()->template getSettingValue<bool>("addChildBDGradient")) {
                            new_color = ccc3(new_color.r * 0.71, new_color.g * 0.71, new_color.b * 0.71);
                        }
                        if (Mod::get()->template getSettingValue<bool>("bgchild-transparency")) {
                            int opacity = Mod::get()->template getSettingValue<int64_t>("bgchild-transparency-scroll");
                            ret->setOpacity(opacity);
                        }
                        ret->setColor(new_color);
                    } else if (ret->getColor() == ccc3(0, 46, 117)) {
                        ccColor3B new_color = Mod::get()->template getSettingValue<ccColor3B>("child-background");
                        if (Mod::get()->template getSettingValue<bool>("syncBGcolor")) {
                            new_color = Mod::get()->template getSettingValue<ccColor3B>("background");
                        }
                        if (Mod::get()->template getSettingValue<bool>("addChildBDGradient")) {
                            new_color = ccc3(new_color.r * 0.74, new_color.g * 0.74, new_color.b * 0.74);
                        }
                        if (Mod::get()->template getSettingValue<bool>("bgchild-transparency")) {
                            int opacity = Mod::get()->template getSettingValue<int64_t>("bgchild-transparency-scroll");
                            ret->setOpacity(opacity);
                        }
                        ret->setColor(new_color);
                    } else if (ret->getColor() == ccc3(0, 36, 91)) {
                        ccColor3B new_color = Mod::get()->template getSettingValue<ccColor3B>("child-background");
                        if (Mod::get()->template getSettingValue<bool>("syncBGcolor")) {
                            new_color = Mod::get()->template getSettingValue<ccColor3B>("background");
                        }
                        if (Mod::get()->template getSettingValue<bool>("addChildBDGradient")) {
                            new_color = ccc3(new_color.r * 0.70, new_color.g * 0.70, new_color.b * 0.70);
                        }
                        if (Mod::get()->template getSettingValue<bool>("bgchild-transparency")) {
                            int opacity = Mod::get()->template getSettingValue<int64_t>("bgchild-transparency-scroll");
                            ret->setOpacity(opacity);
                        }
                        ret->setColor(new_color);
                    } else if (ret->getColor() == ccc3(0, 31, 79)) {
                        ccColor3B new_color = Mod::get()->template getSettingValue<ccColor3B>("child-background");
                        if (Mod::get()->template getSettingValue<bool>("syncBGcolor")) {
                            new_color = Mod::get()->template getSettingValue<ccColor3B>("background");
                        }
                        if (Mod::get()->template getSettingValue<bool>("addChildBDGradient")) {
                            new_color = ccc3(new_color.r * 0.67, new_color.g * 0.67, new_color.b * 0.67);
                        }
                        if (Mod::get()->template getSettingValue<bool>("bgchild-transparency")) {
                            int opacity = Mod::get()->template getSettingValue<int64_t>("bgchild-transparency-scroll");
                            ret->setOpacity(opacity);
                        }
                        ret->setColor(new_color);
                    }
                    ret->release();
                }
            });
        }
        return ret;
    }
};

//bool is_dailychest_ready = false;
bool is_socketserver_started = false;
class $modify(MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        /*if (!is_dailychest_ready) {
            std::thread hThread(dailyChestThread);
            hThread.detach();
            is_dailychest_ready = true;
        }*/
        if (!is_socketserver_started) {
            bool startSocketServer = Mod::get()->template getSettingValue<bool>("socketServer");
            if (startSocketServer) {
                if (!Mod::get()->hasSavedValue("clientId")) {
                    Mod::get()->setSavedValue("clientId", misc::genClientID());
                }
                if (!Mod::get()->hasSavedValue("clientId")) {
                    Notification::create("Couldn't connect to server, invalid Client ID", NotificationIcon::Error)->show();
                    return true;
                }
                std::thread hThread([]() {
                    AMQT handler(Mod::get()->getSavedValue<std::string>("clientId"));
                    handler.connect();
                });
                hThread.detach();
            }
            is_socketserver_started = true;
        }
        return true;
    }
};

$execute {
    (void)Mod::get()->registerCustomSettingType("credit-buttons", &SettingCreditsValue::parse);
    //(void)Mod::get()->registerCustomSettingType("notificationPlacement", &SettingPosValue::parse);
    auto ret = Mod::get()->registerCustomSettingType("notification-placement", &SettingPosValue::parse);
    if (!ret) {
        log::error("Unable to register setting type: {}", ret.unwrapErr());
    }
    Loader::get()->queueInMainThread([]{
        CCScheduler::get()->scheduleSelector(schedule_selector(EventHandler::check), EventHandler::create(), 1.0F, false);
        //CCScheduler::get()->scheduleUpdateForTarget(EventHandler::create(), Priority::Last, false);
    });
}

// When the socket connection is made
$on_mod(Loaded) {
    log::info("GDUtils Mod Loaded");

    chestQueue.push(1);

    //Discord::init(); for next update ;)
    // also for whatever reason, discord rpc doesnt work on vanilla gd, instead it shows absolutes MH icon for some reason even though I dont own MH. can someone explain that

    #ifdef GEODE_IS_WINDOWS
    #include <Windows.h>
    #include <iostream>

    std::array<WCHAR, MAX_PATH> buffer;
    GetModuleFileNameW(NULL, buffer.data(), MAX_PATH);
    auto aaaa = buffer.data();
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string nameStr = fmt::format("cmd /k cd \"{}\\gdutils\" && \"gd_protocol.bat\" %1 && exit", dirs::getGameDir());

    std::wstring fullPath = std::wstring(buffer.data());
    size_t lastBackslashPos = fullPath.find_last_of(L"\\");
    std::wstring exeName;
    if (lastBackslashPos != std::wstring::npos) {
        exeName = fullPath.substr(lastBackslashPos + 1);
    }

    // Create the key for the custom protocol in user registry
    HKEY hCurrentUserKey;
    if (RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Classes", 0, KEY_SET_VALUE, &hCurrentUserKey) == ERROR_SUCCESS) {
        HKEY hKey;
        if (RegCreateKeyEx(hCurrentUserKey, "gdutils", 0, NULL, 0, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
            // Define the default value for the protocol key
            if (RegSetValueEx(hKey, NULL, 0, REG_SZ, (BYTE*)"URL:gdutils Protocol", sizeof("URL:gdutils Protocol")) == ERROR_SUCCESS) {
                // Create the key "shell\open\command" for specify the exe path
                HKEY hCommandKey;
                if (RegCreateKeyEx(hKey, "shell\\open\\command", 0, NULL, 0, KEY_SET_VALUE, NULL, &hCommandKey, NULL) == ERROR_SUCCESS) {
                    if (RegSetValueExA(hCommandKey, NULL, 0, REG_SZ, (const BYTE*)nameStr.c_str(), nameStr.length() + 1) == ERROR_SUCCESS) {
                        log::info("Le protocole a été enregistré avec succès pour cet utilisateur.");
                    } else {
                        log::error("Erreur lors de la définition de la valeur du chemin de l'exécutable.");
                    }
                    RegCloseKey(hCommandKey);
                } else {
                    log::error("Erreur lors de la création de la clé shell\\open\\command.");
                }

                RegSetValueEx(hKey, "URL Protocol", 0, REG_SZ, NULL, NULL);
            } else {
                log::error("Erreur lors de la définition de la valeur par défaut du protocole.");
            }
            RegCloseKey(hKey);
        } else {
            log::error("Erreur lors de la création de la clé du protocole.");
        }
        RegCloseKey(hCurrentUserKey);
    } else {
        log::error("Erreur lors de l'ouverture de la clé du registre utilisateur.");
    }

    std::filesystem::create_directory("gdutils");
    if (!std::filesystem::exists("gdutils/gd_protocol.bat")) {
        std::ofstream file("gdutils/gd_protocol.bat");
        file << R"(
@echo off
setlocal enabledelayedexpansion

        )";
        file << fmt::format("set \"appName={}\"", converter.to_bytes(exeName));
        file << R"(
set "argument=%1"
set "number="

REM Check if the argument contains /
echo !argument! | find "/" > nul
if not errorlevel 1 (
    REM If / is found, extract the number part after the /
    for /f "tokens=2 delims=/" %%a in ("!argument!") do (
        set "number=%%a"
    )
) else (
    REM If / is not found, the argument is in the format gdutils:123
    for /f "tokens=2 delims=:" %%a in ("!argument!") do (
        set "number=%%a"
    )
)

if not defined number (
    echo Invalid argument format: %1
    exit /b 1
)

set "flagFile=%number%.flag"

REM Check if the application is already running
tasklist /FI "IMAGENAME eq %appName%" | find /i "%appName%" > nul
if errorlevel 1 (
    REM If not running, start the application with the provided argument
    cd ..
    start "" "%appName%"
    cd gdutils
    type nul > "%flagFile%"
) else (
    REM If running, create an empty file based on the extracted number
    type nul > "%flagFile%"
)

endlocal
        )";
        file.close();
    }

    #endif
}
