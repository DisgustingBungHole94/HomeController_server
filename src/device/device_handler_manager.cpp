#include "device_handler_manager.h"

#include "handlers/lightstrip_handler.h"

#include <iostream>

Logger DeviceHandlerManager::_logger = Logger("DeviceHandlerManager");

std::map<std::string, HandlerInfo> DeviceHandlerManager::_handlers;

void DeviceHandlerManager::init() {
    // register handlers here

    _handlers.insert(std::make_pair("light_strip", HandlerInfo(
        []() -> Handler* { return new LightStripHandler(); },
        "Light Strip",
        "Strip of RGB LEDs."
    )));

    _logger.log("Loaded devices: ", false);
    for (auto& x : _handlers) {
        std::cout << x.first <<  " ";
    }
    std::cout << std::endl;
}

std::unique_ptr<Handler> DeviceHandlerManager::getHandler(const std::string& name) {
    std::unique_ptr<Handler> handler;

    auto mit = _handlers.find(name);
    if (mit != _handlers.end()) {
        handler.reset(mit->second.m_constructor());

        handler->setDeviceTitle(mit->second.m_deviceTitle);
        handler->setDeviceDescription(mit->second.m_deviceDescription);

        return std::move(handler);
    }

    return nullptr;
}