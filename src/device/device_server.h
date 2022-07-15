#pragma once

#include <memory>

#include "../util/logger.h"
#include "../net/tls_server.h"
#include "device_session.h"
#include "../thread/thread_pool.h"

class HomeController;

class DeviceServer {
    public:
        DeviceServer();
        ~DeviceServer();

        void init(HomeController* controller, int maxConnections);
        
        void startSession(std::unique_ptr<TLSClient> tlsClient);
        void stop();

        ThreadPool<DeviceSession>* getPool() { return &m_threadPool; }

    private:
        Logger m_logger;

        HomeController* m_controller;

        ThreadPool<DeviceSession> m_threadPool;
};