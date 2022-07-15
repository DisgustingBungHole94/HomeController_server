#include "device_server.h"

DeviceServer::DeviceServer() 
    : m_logger("DeviceServer"), m_threadPool("DeviceThreadPool")
{}

DeviceServer::~DeviceServer() {}

void DeviceServer::init(HomeController* controller, int maxConnections) {
    m_controller = controller;

    m_threadPool.start(maxConnections);

    m_logger.log("Device server intialized.");
}

void DeviceServer::startSession(std::unique_ptr<TLSClient> tlsClient) {
    m_logger.dbg("Starting Device session for client [" + tlsClient->getIp() + "]");

    std::unique_ptr<DeviceSession> client = std::make_unique<DeviceSession>(m_controller, std::move(tlsClient));
    m_threadPool.addJob(std::move(client));
}

void DeviceServer::stop() {
    m_logger.log(std::to_string(m_threadPool.numActiveJobs()) + " connected devices. Closing...");
    m_threadPool.stop();
}