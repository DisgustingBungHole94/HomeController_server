#pragma once

#include <memory>
#include <iostream>

#include "../util/logger.h"
#include "../net/tls_server.h"
#include "ws_server_config.h"
#include "../ws/ws_session.h"
#include "../thread/thread_pool.h"

class HomeController;

class WebSocketServer {
    public:
        WebSocketServer();
        ~WebSocketServer();

        void init(HomeController* controller, int maxConnections);
        void startSession(std::unique_ptr<TLSClient> tlsClient, std::string upgradeRequest);

        void stop();
    private:
        Logger m_logger;

        HomeController* m_controller;

        ws::server m_server;

        ThreadPool<WebSocketSession> m_threadPool;
};