#include "AMQClient.hpp"
#include "../../include/RateEvent.hpp"

bool show_connected = false;
std::queue<std::string> dataQueue;
static std::optional<arc::mpsc::Sender<std::string>> s_tx;

void AMQT::setTx(arc::mpsc::Sender<std::string> tx) {
    s_tx = std::move(tx);
}

bool AMQT::sendHeartbeat() {
    auto now = std::chrono::steady_clock::now();
    auto lastDuration = std::chrono::duration_cast<std::chrono::seconds>(now - m_lastHeartbeatTime).count();
    // every 10 seconds
    if (lastDuration >= 10) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (isConnected()) {
            // because WHY CANT I USE amqp_heartbeat_send!?!? we will decide to manually construct it...
            amqp_frame_t frame;
            frame.channel = 0; // or 1? idk... amqp_socket.c says 0
            frame.frame_type = AMQP_FRAME_HEARTBEAT;
            auto res = amqp_send_frame(m_connection, &frame);
            if (AMQP_STATUS_OK != res) {
                log::warn("Failed to send heartbeat: {}", amqp_error_string2(res));
                return false;
            } else {
                //log::debug("Sent heartbeat!");
                m_lastHeartbeatTime = now;
                return true;
            }
        }
    }
    return true;
}

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
                errorStr = fmt::format("connection closed: {} (code {})", std::string((char*)m->reply_text.bytes, m->reply_text.len), m->reply_code);
            } else if (reply.reply.id == AMQP_CHANNEL_CLOSE_METHOD) {
                amqp_channel_close_t* m = (amqp_channel_close_t*) reply.reply.decoded;
                errorStr = fmt::format("channel closed: {} (code {})", std::string((char*)m->reply_text.bytes, m->reply_text.len), m->reply_code);
            } else {
                errorStr = fmt::format("Unknown Error (method id {})", reply.reply.id);
            }
            break;
        case AMQP_RESPONSE_NORMAL: break;
    }
    return fmt::format("{} ({})", errorStr, AMQReplyToString(reply.reply_type));
}
void AMQT::setupChannel() {
    {
        amqp_channel_open(m_connection, 1);
        auto reply = amqp_get_rpc_reply(m_connection);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            log::error("Error opening channel: {}", AMQErrorToString(reply));
            return;
        }
    }
    // Shared stream (one append-only log for every client) instead of a queue
    // per client: no more fanout duplication per rate event, and no per-client
    // queue left behind on unclean disconnects. Consumed over plain AMQP 0-9-1
    // via x-stream-offset -- there's no mature Stream-protocol C client that's
    // proven to build on Android/iOS, so we keep using rabbitmq-c.
    //
    // The stream, and its binding to amq.topic/rate.#, are created and owned
    // server-side (management UI), not here: the gd user only has read on this
    // queue, so we go straight to consume instead of declare/bind.
    {
        // required for streams over AMQP 0-9-1: without a prefetch, no messages are delivered
        amqp_basic_qos(m_connection, 1, 0, 100, 0);
        auto reply = amqp_get_rpc_reply(m_connection);
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            log::error("Error setting QoS: {}", AMQErrorToString(reply));
            return;
        }
    }
    {
        // only events published after we connect, never replay stream history
        amqp_table_entry_t consume_arg_entries[1];
        consume_arg_entries[0] = (amqp_table_entry_t) {
            .key = amqp_cstring_bytes("x-stream-offset"),
            .value = {.kind = AMQP_FIELD_KIND_UTF8, .value = {.bytes = amqp_cstring_bytes("next")}}
        };
        amqp_table_t consume_arg_table = {.num_entries = 1, .entries = consume_arg_entries};

        // stream queues don't support auto-ack (no_ack=1): messages must be acked manually below
        auto res = amqp_basic_consume(m_connection, 1, amqp_cstring_bytes(AMQ_STREAM_NAME), amqp_empty_bytes, 0, 0, 0, consume_arg_table);
        auto reply = amqp_get_rpc_reply(m_connection);

        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
            log::error("Error consuming from stream: {}", AMQErrorToString(reply));
            return;
        }
        if (res) {
            std::string data((char*)res->consumer_tag.bytes, res->consumer_tag.len);
            GDUtils::Events::OnServerConnect().send(true);
            log::info("Subscribed to Rate Notifications! (Consumer Tag: {})", data);
        }
    }

    bool forceReconnection = false;
    auto lastReconnectTime = std::chrono::steady_clock::now();

    struct timeval timeout = {10, 0};

    while (isConnected()) {
        // this is just an extra so we're sure that we're always connected
        if (!sendHeartbeat()) {
            log::error("Couldn't send heartbeat, disconnecting...");
            break;
        }
        if (forceReconnection) {
            auto now = std::chrono::steady_clock::now();
            auto lastDuration = std::chrono::duration_cast<std::chrono::seconds>(now - lastReconnectTime).count();
            if (lastDuration >= 600) {
                log::info("Forcing a reconnection...");
                break;
            }
        }
        amqp_envelope_t envelope;
        amqp_maybe_release_buffers(m_connection);
        auto ret = amqp_consume_message(m_connection, &envelope, &timeout, 0);
        if (AMQP_RESPONSE_NORMAL != ret.reply_type) {
            if (AMQP_RESPONSE_LIBRARY_EXCEPTION == ret.reply_type && AMQP_STATUS_HEARTBEAT_TIMEOUT == ret.library_error) {
                log::error("Heartbeat failed, initiating a reconnection!");
                break;
            } else if (AMQP_RESPONSE_LIBRARY_EXCEPTION == ret.reply_type && AMQP_STATUS_UNEXPECTED_STATE == ret.library_error) {
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
                            if (s_tx) {
                                (void)s_tx->trySend(std::move(data));
                            }
                            amqp_destroy_message(&message);
                        }
                        continue;
                    } else {
                        log::error("(Frame ID: {}) Error consuming message: {}", frame.payload.method.id, AMQErrorToString(ret));
                        break;
                    }
                } else {
                    log::error("Response wasn't normal! {}", AMQErrorToString(ret));
                }
            } else if (AMQP_RESPONSE_LIBRARY_EXCEPTION == ret.reply_type && AMQP_STATUS_TIMEOUT == ret.library_error) {
                // honestly how has no one gotten this issue!?
                continue;
            } else {
                log::error("Error consuming message: {}", AMQErrorToString(ret));
                break;
            }
        } else {
            log::info("call rate event");
            std::string data((char*)envelope.message.body.bytes, envelope.message.body.len);
            if (s_tx) {
                (void)s_tx->trySend(std::move(data));
            }
            amqp_basic_ack(m_connection, envelope.channel, envelope.delivery_tag, 0);
            amqp_destroy_envelope(&envelope);
        }
    }
    GDUtils::Events::OnServerConnect().send(false);
    m_connected = false;
    m_cv.notify_all();
}

void AMQT::connect() {
    m_remainingAttempts++;
    while (m_remainingAttempts-- > 0) {
        log::info("Starting AMQP...");
        if (m_remainingAttempts != MAX_RECONNECT_ATTEMPTS) {
            int currentAttempt = MAX_RECONNECT_ATTEMPTS - m_remainingAttempts + 1;
            int delayMs = 3000 * std::pow(2, currentAttempt - 1);
            log::warn("Disconnected from server. Attempting to reconnect... ({} left, reconnecting in {} seconds)", m_remainingAttempts, (float)((delayMs / 1000.F)));
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        }
        m_connection = amqp_new_connection();
        amqp_socket_t* socket = amqp_tcp_socket_new(m_connection);
        if (!socket) {
            log::error("Couldn't create TCP connection");
            continue;
        }
        int status;
        status = amqp_socket_open(socket, AMQ_HOST, AMQ_PORT);
        if (status != AMQP_STATUS_OK) {
            log::error("Error opening socket! {}", amqp_error_string2(status));
            continue;
        }
        if (socket && m_connection) {
            // heartbeat every 10 seconds!
            amqp_rpc_reply_t reply = amqp_login(m_connection, "/", 0, 131072, 10, AMQP_SASL_METHOD_PLAIN, AMQ_USER, AMQ_PASS);
            if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
                const char* errorStr = nullptr;
                log::error("Couldn't login to AMQP server! {}", AMQErrorToString(reply));
                continue;
            }
            m_connected = true;
            setupChannel();
            disconnect();
        }
    }
    log::error("Maximum reconnection attempts reached, cannot reconnect. Restart the game to reconnect to the MQTT server.");
}
bool AMQT::genConnected() {
    return show_connected;
}
void AMQT::setGenConnected(bool value) {
    show_connected = value;
}
std::queue<std::string> AMQT::getQueue() {
    return dataQueue;
}
std::string AMQT::getQueueFrontPop() {
    std::string value = std::move(dataQueue.front());
    dataQueue.pop();
    return value;
}
void AMQT::sendToTx(const std::string data) {
    if (s_tx) {
        (void)s_tx->trySend(std::move(data));
    }
}
