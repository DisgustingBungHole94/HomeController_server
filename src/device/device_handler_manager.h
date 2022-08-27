#pragma once

#include "handlers/handler.h"

#include "../util/logger.h"

#include <map>
#include <string>
#include <functional>
#include <memory>

class HomeController;

struct HandlerInfo {
    HandlerInfo(std::function<Handler*()> constructor, const std::string& deviceTitle, const std::string& deviceDescription)
        : m_constructor(constructor), m_deviceTitle(deviceTitle), m_deviceDescription(deviceDescription)
    {}

    std::function<Handler*()> m_constructor;

    std::string m_deviceTitle;
    std::string m_deviceDescription;
};

class DeviceHandlerManager {
    public:
        static void init();
        static std::unique_ptr<Handler> getHandler(const std::string& name);
    private:
        static Logger _logger;

        static std::map<std::string, HandlerInfo> _handlers;
};