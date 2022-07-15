#include "http_server.h"

#include "../http/http_session.h"

HTTPServer::HTTPServer()
    : m_logger("HTTPServer"), m_threadPool("HTTPThreadPool"), m_running(false)
{}

HTTPServer::~HTTPServer() {}

void HTTPServer::init(HomeController* controller, int port, const std::string& certFile, const std::string& privKeyFile, int maxConnections) {
    m_controller = controller;

    m_tlsServer.init(port, certFile, privKeyFile);
    m_threadPool.start(maxConnections);

    m_running = true;

    m_logger.log("HTTP server intialized.");
}

void HTTPServer::run() {
    while(m_running) {
        try {
            std::unique_ptr<TLSClient> client = m_tlsServer.accept();
            if (client == nullptr) continue;

            client->setTimeout(5);

            m_threadPool.addJob(std::make_unique<HTTPSession>(m_controller, std::move(client)));
        } catch(GeneralException& e) {
            m_logger.err("Client error: " + e.what() + " (" + e.func() + ")");
        }
    }
}

void HTTPServer::stop() {
    m_running = false;

    m_tlsServer.stop();

    m_logger.log(std::to_string(m_threadPool.numActiveJobs()) + " active HTTPS sessions. Closing...");
    m_threadPool.stop();
}