#pragma once

#include <homecontroller/http/http_response.h>

#include <string>
#include <vector>

class homecontroller;

class module {
    public:
        module(homecontroller* controller) 
            : m_controller(controller)
        {}
        ~module() {}

        virtual hc::http::http_response execute(const std::string& method, const std::vector<std::string>& path, const std::string& body);

    protected:
        homecontroller* m_controller;

        bool m_auth_required;
        std::string m_allowed_methods;
};