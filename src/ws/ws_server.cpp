#include "ws_server.h"

#include "../util/exception/general_exception.h" 

WebSocketServer::WebSocketServer() 
    : m_logger("WSServer"), m_threadPool("WSThreadPool")
{}

WebSocketServer::~WebSocketServer() {}

void WebSocketServer::init(HomeController* controller, int maxConnections) {
    m_controller = controller;

    m_threadPool.start(maxConnections);

    try {
        m_server.set_access_channels(websocketpp::log::alevel::none);
        m_server.clear_access_channels(websocketpp::log::alevel::frame_payload);
        m_server.set_error_channels(websocketpp::log::elevel::none);
    } catch(std::exception& e) {
        throw hc::exception("Failed to initialize WebSocket server: " + std::string(e.what()), "WSSServer::init");
    }

    m_logger.log("WebSocket server intialized.");
}

void WebSocketServer::startSession(std::unique_ptr<TLSClient> tlsClient, std::string upgradeRequest) {
    try {
        m_logger.dbg("Starting WebSocket session for client [" + tlsClient->getIp() + "]");

        ws::server::connection_ptr con = m_server.get_connection();
        std::unique_ptr<WebSocketSession> client = std::make_unique<WebSocketSession>(m_controller, &m_server, con, std::move(tlsClient), upgradeRequest);

        m_threadPool.addJob(std::move(client));
    } catch(std::exception& e) {
        throw hc::exception("Failed to initialize connection.", "WSSServer::addConnection");
    }
}

void WebSocketServer::stop() {
    m_logger.log(std::to_string(m_threadPool.numActiveJobs()) + " active WebSocket sessions. Closing...");
    m_threadPool.stop();
}