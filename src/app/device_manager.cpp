#include "device_manager.h"

#include "../util/exception/general_exception.h"
#include "../util/id.h"

DeviceManager::DeviceManager() {}
DeviceManager::~DeviceManager() {}

void DeviceManager::init() {

}

Device DeviceManager::registerDevice(const std::string& name, const std::string& type) {
    ID id;
    id.generate();

    return Device(id.str(), name, type);
}

void DeviceManager::unregisterDevice(const std::string& id) {

}

Device DeviceManager::getDevice(const std::string& id) {
    if (id == "baaaaaaaaaaaaaaaaaaaaaaaaaaaaaab") {
        return Device("baaaaaaaaaaaaaaaaaaaaaaaaaaaaaab", "Test Device", "light_strip");
    }

    throw GeneralException("Device not found!", "DeviceManager::getDevice");

    return Device();
}