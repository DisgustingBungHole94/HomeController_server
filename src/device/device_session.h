#pragma once

#include "../util/logger.h"
#include "../net/tls_server.h"
#include "../thread/job.h"

#include <memory>
#include <vector>
#include <mutex>

class HomeController;

class Device;
typedef std::shared_ptr<Device> DevicePtr;
typedef std::weak_ptr<Device> DeviceWeakPtr;

class DeviceSession : public Job {
    public:
        DeviceSession(HomeController* controller, std::unique_ptr<TLSClient> tlsClient);
        ~DeviceSession();

        void run();
        void stop();

        std::string sendMessage(const std::string& msg);

    private:
        uint8_t registerDevice(const std::string& msg);
        DevicePtr getDevice();

        Logger m_logger;

        HomeController* m_controller;

        std::unique_ptr<TLSClient> m_tlsClient;

        bool m_running;

        DeviceWeakPtr m_device;
        bool m_registered;

        std::mutex m_mutex;
};