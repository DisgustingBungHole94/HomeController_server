#include "ws_session.h"

#include "../util/exception/general_exception.h"
//#include "../util/string.h"

#include "../app/device_manager.h"
#include "../homecontroller.h"

#include <streambuf>
#include <system_error>

WebSocketSession::WebSocketSession(HomeController* controller, ws::server* serverPtr, ws::server::connection_ptr connectionPtr, std::unique_ptr<TLSClient> tlsClient, std::string upgradeRequest)
    : m_logger("WS@" + tlsClient->getIp()), m_controller(controller), m_serverPtr(serverPtr), m_connectionPtr(connectionPtr), m_tlsClient(std::move(tlsClient)), m_upgradeRequest(upgradeRequest), m_running(false), m_registered(false)
{}

WebSocketSession::~WebSocketSession() {}

void WebSocketSession::run() {
    m_tlsClient->setTimeout(0);

    m_connectionPtr->set_vector_write_handler(std::bind(&WebSocketSession::onVectorWrite, this, std::placeholders::_1, std::placeholders::_2));
    m_connectionPtr->set_write_handler(std::bind(&WebSocketSession::onWrite, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    m_connectionPtr->set_open_handler(std::bind(&WebSocketSession::onOpen, this, std::placeholders::_1));
    m_connectionPtr->set_fail_handler(std::bind(&WebSocketSession::onFail, this, std::placeholders::_1));
    m_connectionPtr->set_close_handler(std::bind(&WebSocketSession::onClose, this, std::placeholders::_1));
    m_connectionPtr->set_message_handler(std::bind(&WebSocketSession::onMessage, this, std::placeholders::_1, std::placeholders::_2));

    m_connectionPtr->start();
    
    m_running = true;

    m_connectionPtr->read_all(m_upgradeRequest.c_str(), m_upgradeRequest.length());

    while(m_running) {
        try {
            std::string data = m_tlsClient->recv();
            m_connectionPtr->read_all(data.c_str(), data.length());
        } catch(GeneralException& e) {
            if (m_running) {
                m_logger.err("Client error: " + e.what() + " (" + e.func() + ")");

                m_running = false;
            }
            break;
        }
    }

    std::lock_guard<std::mutex> lock(m_mutex);

    while(m_connectionPtr->get_state() == websocketpp::session::state::closing);
    m_tlsClient->stop();

    m_logger.dbg("Client session exited.");
}

void WebSocketSession::stop() {
    if (m_running) {
       close(m_connectionPtr->get_handle(), "server closed");
    }
}

std::error_code WebSocketSession::onVectorWrite(websocketpp::connection_hdl hdl, std::vector<websocketpp::transport::buffer> const& bufs) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_running) {
        return std::make_error_code(std::errc::connection_aborted);
    }

    std::string data;
    
    for (int i = 0; i < bufs.size(); i++) {
        data.append(bufs[i].buf, bufs[i].len);
    }

    try {
        m_tlsClient->send(data);
    } catch(GeneralException& e) {
        m_logger.err("Client error: " + e.what() + " (" + e.func() + ")");

        m_running = false;
        return std::make_error_code(std::errc::io_error);
    }

    return std::error_code();
}

std::error_code WebSocketSession::onWrite(websocketpp::connection_hdl hdl, const char* data, std::size_t len) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_running) {
        return std::make_error_code(std::errc::connection_aborted);
    }

    try {
        m_tlsClient->send(std::string(data, len));
    } catch(GeneralException& e) {
        m_logger.err("Client error: " + e.what() + " (" + e.func() + ")");

        m_running = false;
        return std::make_error_code(std::errc::io_error);
    }

    return std::error_code();
}

void WebSocketSession::onOpen(websocketpp::connection_hdl hdl) {
    m_logger.dbg("WebSocket session opened.");
}

void WebSocketSession::onFail(websocketpp::connection_hdl hdl) {
    m_logger.err("Failed to start WebSocket session.");

    m_running = false;
}

void WebSocketSession::onClose(websocketpp::connection_hdl hdl) {
    m_logger.dbg("WebSocket session closed.");

    m_running = false;
}

void WebSocketSession::onMessage(websocketpp::connection_hdl hdl, ws::server::message_ptr msg) {    
    if (msg->get_opcode() != websocketpp::frame::opcode::binary) return;

    std::string message = msg->get_payload();

    if (message.size() < 5) return;

    std::vector<uint8_t> response;
    response.push_back(message[0]);
    response.push_back(message[1]);
    response.push_back(message[2]);
    response.push_back(message[3]);

    if (!m_registered) {
        char resultByte = registerDevice(message);

        response.push_back(resultByte);
        sendMessage(hdl, response);
        
        if (resultByte != 0x00) {
            close(hdl, "handshake failed");
        } else {
            m_registered = true;
        }
        return;
    }

    try {
        if (!getDevice()->connected()) {
            response.push_back(0xFF);
            sendMessage(hdl, response);

            close(hdl, "device disconnected");

            m_logger.dbg("Device disconnected! Session will close.");
        } else {
            std::string deviceRes = getDevice()->getConnection()->sendMessage(message);
            sendMessage(hdl, deviceRes);
        }
    } catch(GeneralException& e) {
        m_logger.err("Failed to communicate with device: " + e.what());
    }
}

uint8_t WebSocketSession::registerDevice(const std::string& msg) {
    if (msg.size() < 68) {
        return 0x01;
    }

    std::string sessionId(msg.begin() + 4, msg.begin() + 36);

    SessionPtr session = m_controller->getAuthManager()->getSession(sessionId);
    if (session == nullptr) {
        return 0x02;
    }

    std::string deviceId(msg.begin() + 36, msg.begin() + 68);

    DevicePtr device = session->getUser()->getDevice(deviceId);
    if (device == nullptr) {
        return 0x03;
    }

    if (!device->connected()) {
        m_logger.dbg("Failed to register, device is offline.");
        return 0x04;
    }

    m_registered = true;
    m_device = device;

    m_logger.dbg("Connected to device [" + device->getName() + "]!");

    return 0x00;
}

DevicePtr WebSocketSession::getDevice() {
    if (m_device.expired()) {
        throw GeneralException("Device deleted.", "WebSocketSession::getDevice");
    }

    return m_device.lock();
}

void WebSocketSession::sendMessage(websocketpp::connection_hdl hdl, const std::vector<uint8_t>& msg) {
    try {
        m_serverPtr->send(hdl, &msg[0], msg.size(), websocketpp::frame::opcode::binary);
    } catch(std::exception& e) {
        m_logger.err("Failed to send message: " + std::string(e.what()));
    }
}

void WebSocketSession::sendMessage(websocketpp::connection_hdl hdl, const std::string& msg) {
    try {
        m_serverPtr->send(hdl, msg, websocketpp::frame::opcode::binary);
    } catch(std::exception& e) {
        m_logger.err("Failed to send message: " + std::string(e.what()));
    }
}

void WebSocketSession::close(websocketpp::connection_hdl hdl, const std::string& reason) {
    try {
        m_serverPtr->close(hdl, websocketpp::close::status::going_away, reason);
    } catch(std::exception& e) {
        m_logger.err("Failed to close client: " + std::string(e.what()));
    }
}