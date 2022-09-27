#pragma once

#include <homecontroller/app/device.h>

#include <mutex>

class device_manager {
    public:
        device_manager() {}
        ~device_manager() {}

        void load_devices();

        std::string register_device(const hc::device& device);
        void unregister_device(const std::string& id);
};