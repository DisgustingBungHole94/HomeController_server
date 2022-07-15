#pragma once

#include "../util/logger.h"
#include "../net/tls_server.h"
#include "ws_server_config.h"
#include "../thread/job.h"

#include <vector>
#include <mutex>

class HomeController;

class Device;
typedef std::shared_ptr<Device> DevicePtr;
typedef std::weak_ptr<Device> DeviceWeakPtr;

class WebSocketSession : public Job {
    public:
        WebSocketSession(HomeController* controller, ws::server* serverPtr, ws::server::connection_ptr connectionPtr, std::unique_ptr<TLSClient> tlsClient, std::string upgradeRequest);
        ~WebSocketSession();

        void run();
        void stop();

    private:
        std::error_code onVectorWrite(websocketpp::connection_hdl hdl, std::vector<websocketpp::transport::buffer> const& bufs);
        std::error_code onWrite(websocketpp::connection_hdl hdl, const char* data, std::size_t len);

        void onOpen(websocketpp::connection_hdl hdl);
        void onFail(websocketpp::connection_hdl hdl);
        void onClose(websocketpp::connection_hdl hdl);
        void onMessage(websocketpp::connection_hdl hdl, ws::server::message_ptr msg);

        uint8_t registerDevice(const std::string& msg);
        DevicePtr getDevice();

        void sendMessage(websocketpp::connection_hdl hdl, const std::vector<uint8_t>& msg);
        void sendMessage(websocketpp::connection_hdl hdl, const std::string& msg);

        void close(websocketpp::connection_hdl hdl, const std::string& reason);

        Logger m_logger;

        HomeController* m_controller;

        ws::server* m_serverPtr;
        ws::server::connection_ptr m_connectionPtr;

        std::unique_ptr<TLSClient> m_tlsClient;

        std::string m_upgradeRequest;

        bool m_running;

        bool m_registered;

        DeviceWeakPtr m_device;

        std::mutex m_mutex;
};