#pragma once

#include <homecontroller/http/http_response.h>
#include <homecontroller/app/session.h>

#include <string>
#include <vector>

class homecontroller;

class module {
    public:
        module(homecontroller* controller, bool auth_required, std::string allowed_methods) 
            : m_controller(controller), m_auth_required(auth_required), m_allowed_methods(allowed_methods), m_session_ptr(nullptr)
        {}

        ~module() {}

        virtual hc::http::http_response execute(const std::string& method, const std::vector<std::string>& path, const std::string& body);

        bool auth_required() { return m_auth_required; }
        const std::string& get_allowed_methods() { return m_allowed_methods; }

        void set_session_ptr(std::shared_ptr<hc::session> session_ptr) { m_session_ptr = session_ptr; }
        std::shared_ptr<hc::session> get_session_ptr() { return m_session_ptr; }

    protected:
        homecontroller* m_controller;

    private:
        bool m_auth_required;
        std::string m_allowed_methods;

        std::shared_ptr<hc::session> m_session_ptr;
};