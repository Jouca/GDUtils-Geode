#pragma once
#include <defs/geode.hpp>
#ifdef GEODE_IS_WINDOWS // stop the dumb compiler errors!
#include <WinSock2.h>
#include <WS2tcpip.h>
//#include <synchapi.h>
#endif
#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>
#include <queue>
#include <config.hpp>
#include <arc/sync/mpsc.hpp>

class AMQT {
    private:
    amqp_connection_state_t m_connection;
    amqp_socket_t* m_socket;
    std::string client_id;

    std::atomic<int> m_remainingAttempts = {MAX_RECONNECT_ATTEMPTS};
    std::atomic<bool> m_connected{false};

    std::mutex m_mutex;
    std::condition_variable m_cv;

    std::chrono::steady_clock::time_point m_lastActivityTime;
    std::chrono::steady_clock::time_point m_lastHeartbeatTime;

    public:
    AMQT(const std::string& client_id) : client_id(std::move(client_id)), m_lastActivityTime(std::chrono::steady_clock::now()), m_lastHeartbeatTime(m_lastActivityTime) {};
    void updateActivity() {
        m_lastActivityTime = std::chrono::steady_clock::now();
    }
    // since the current heartbeat does NOT work!!
    bool sendHeartbeat();
    bool isConnected() {
        return m_connected.load();
    }
    void setupChannel();
    void connect();
    void disconnect() {
        if (m_connection) {
            m_connected = false;
            amqp_destroy_connection(m_connection);
            m_connection = nullptr;
        }
    }
    static bool genConnected();
    static void setGenConnected(bool value);
    static std::queue<std::string> getQueue();
    static std::string getQueueFrontPop();
    static void sendToTx(const std::string);
    static void setTx(arc::mpsc::Sender<std::string> tx);
};

