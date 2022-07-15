#pragma once

#include "device.h"

#include <string>
#include <memory>
#include <map>
#include <mutex>

class DeviceManager {
    public:
        DeviceManager();
        ~DeviceManager();

        void init();

        Device registerDevice(const std::string& name, const std::string& type);
        void unregisterDevice(const std::string& id);

        Device getDevice(const std::string& id);
};