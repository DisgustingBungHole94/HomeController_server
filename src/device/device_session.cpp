#include "device_session.h"

#include "../util/exception/general_exception.h"
//#include "../util/string.h"
#include "../util/timer.h"

#include "../homecontroller.h"

DeviceSession::DeviceSession(HomeController* controller, std::unique_ptr<TLSClient> tlsClient) 
    : m_logger("DS@" + tlsClient->getIp()), m_controller(controller), m_tlsClient(std::move(tlsClient)), m_running(false), m_registered(false)
{}

DeviceSession::~DeviceSession() {}

void DeviceSession::run() {
    try {
        std::string handshakeMsg = m_tlsClient->recv();

        uint8_t res = registerDevice(handshakeMsg);

        std::string handshakeRes;
        handshakeRes.resize(1);
        handshakeRes.at(0) = res;

        m_tlsClient->send(handshakeRes);

        if (res == 0x00) {
            m_running = true;
        } else {
            m_logger.err("Device handshake failed: " + std::to_string(res));
        }
    } catch(GeneralException& e) {
        m_logger.err("Device handshake failed: " + e.what());
    }

    Timer timer(std::chrono::milliseconds(3000));
    while(m_running) {
        if(!timer.isFinished()) continue;

        std::lock_guard<std::mutex> lock(m_mutex);

        std::string ping, pong;
        ping.resize(1);
        ping.at(0) = 0x00;

        try {
            m_tlsClient->send(ping);
            pong = m_tlsClient->recv();
        } catch(GeneralException& e) {
            m_running = false;
            m_logger.dbg("Failed to ping device: " + e.what());
            continue;
        }

        if (pong.size() != 1 || pong[0] != 0x00) {
            m_running = false;
            m_logger.dbg("Failed to ping device!");
            continue;
        }

        timer.reset();
    }

    m_tlsClient->stop();

    if (m_registered) {
        try {
            getDevice()->disconnect();
            m_logger.log("Device [" + getDevice()->getName() + "] disconnected.");
        } catch(GeneralException& e) {
            m_logger.err("Failed to disconnect device: " + e.what());
        }
    }
}

void DeviceSession::stop() {
    try {
        m_tlsClient->send(std::to_string(0xFF));
    } catch(GeneralException& e) {
        m_logger.err("Failed to send going away message: " + e.what());
    }

    m_running = false;
}

std::string DeviceSession::sendMessage(const std::string& msg) {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::string response;

    try {
        m_tlsClient->send(msg);
        response = m_tlsClient->recv();
    } catch(GeneralException& e) {
        if (m_running) {
            m_logger.err("Client error: " + e.what() + " (" + e.func() + ")");

            m_running = false;
        }
    }

    return response;
}

uint8_t DeviceSession::registerDevice(const std::string& msg) {
    if (msg.size() < 64) {
        return 0x01;
    }

    std::string sessionId(msg.begin(), msg.begin() + 32);

    SessionPtr session = m_controller->getAuthManager()->getSession(sessionId);
    if (session == nullptr) {
        return 0x02;
    }

    std::string deviceId(msg.begin() + 32, msg.begin() + 64);

    DevicePtr device = session->getUser()->getDevice(deviceId);
    if (device == nullptr) {
        return 0x04;
    }
    
    if (device->connected()) {
        m_logger.dbg("Failed to register, device is already connected.");
        return 0x06;
    }

    device->connect(m_controller->getDeviceServer()->getPool()->getJob(m_id));

    m_registered = true;
    m_device = device;

    m_logger.log("Device [" + device->getName() + "] connected!");

    return 0x00;
}

DevicePtr DeviceSession::getDevice() {
    if (m_device.expired()) {
        throw GeneralException("Device deleted.", "DeviceSession::getDevice");
    }

    return m_device.lock();
}