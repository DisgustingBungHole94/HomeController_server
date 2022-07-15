#include "user.h"

#include "../util/exception/general_exception.h"

void User::addDevice(const Device& device) {
    std::lock_guard<std::mutex> lock(m_mutex);

    DevicePtr ptr = std::make_shared<Device>(device);
    m_devices.insert(std::make_pair(ptr->getId(), ptr));
}

void User::removeDevice(const std::string& device) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto mit = m_devices.find(device);
    if (mit == m_devices.end()) {
        throw GeneralException("Device does not exist.", "User::removeDevice");
    }

    m_devices.erase(mit);
}

DevicePtr User::getDevice(const std::string& id) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto mit = m_devices.find(id);
    if (mit == m_devices.end()) {
        return nullptr;
    }

    return mit->second;
}