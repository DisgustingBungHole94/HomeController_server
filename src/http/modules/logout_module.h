#pragma once

#include "module.h"

class logout_module : public module {
    public:
        logout_module(homecontroller* controller)
            : module(controller, true, "GET, OPTIONS")
        {}

        ~logout_module() {}

        hc::http::http_response execute(const std::string& method, const std::vector<std::string>& path, const std::string& body);
};