#pragma once

#include "../net/tls_server.h"
#include "../thread/thread_pool.h"

#include <memory>

class HomeController;
class HTTPSession;

class HTTPServer {
    public:
        HTTPServer();
        ~HTTPServer();

        void init(HomeController* controller, int port, const std::string& certFile, const std::string& privKeyFile, int maxConnections);

        void run();
        void stop();

    private:
        Logger m_logger;

        HomeController* m_controller;

        TLSServer m_tlsServer;
        int m_port;

        ThreadPool<HTTPSession> m_threadPool;

        bool m_running;
};