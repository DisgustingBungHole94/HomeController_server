#pragma once

#include "module.h"

class register_device_module : public module {
    public:
        register_device_module(homecontroller* controller)
            : module(controller, true, "POST, OPTIONS")
        {}

        ~register_device_module() {}

        hc::http::http_response execute(const std::string& method, const std::vector<std::string>& path, const std::string& body);
};