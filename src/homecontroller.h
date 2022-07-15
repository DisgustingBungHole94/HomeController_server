#pragma once

#include <iostream>
#include <memory>
#include <mutex>

#include "app/auth_manager.h"
#include "app/user_manager.h"

#include "app/device_manager.h"

#include "http/http_server.h"
#include "ws/ws_server.h"
#include "device/device_server.h"

#include "util/logger.h"

class Program;

enum class HomeControllerStatus {
    RUNNING, STOPPED
};

struct HomeControllerConfig {
    int m_serverPort;
    std::string m_tlsCertFile;
    std::string m_tlsPrivKeyFile;

    int m_maxHTTPConnections;
    int m_maxWSConnections;
    int m_maxDevices;

    int m_sessionExpireTime;

    std::string m_logMode;
};

class HomeController {
    public:
        HomeController();
        ~HomeController();

        bool start();
        void loop();
        void shutdown();

        AuthManager* getAuthManager() { return &m_authManager; }
        UserManager* getUserManager() { return &m_userManager; }

        DeviceManager* getDeviceManager() { return &m_deviceManager; }

        WebSocketServer* getWSServer() { return &m_wsServer; }
        DeviceServer* getDeviceServer() { return &m_deviceServer; }

    private:
        Logger m_logger;

        AuthManager m_authManager;
        UserManager m_userManager;

        DeviceManager m_deviceManager;

        HTTPServer m_httpServer;
        WebSocketServer m_wsServer;
        DeviceServer m_deviceServer;

        HomeControllerStatus m_status;

        std::mutex m_mutex;
};