#pragma once

#include "module.h"

class login_module : public module {
    public:
        login_module(homecontroller* controller)
            : module(controller, false, "POST, OPTIONS")
        {}

        ~login_module() {}

        hc::http::http_response execute(const std::string& method, const std::vector<std::string>& path, const std::string& body);
};