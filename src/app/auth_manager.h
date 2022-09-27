#pragma once

#include <homecontroller/app/session.h>

#include <mutex>

class homecontroller;

class auth_manager {
    public:
        auth_manager(homecontroller* controller)
            : m_controller(controller)
        {}

        ~auth_manager() {}

        std::string create_session(const std::string& username, const std::string& password, const std::string& ip);

        void delete_session(const std::string& id);
        void delete_sessions_by_username(const std::string& username);

    private:
        homecontroller* m_controller;

        std::map<std::string, std::shared_ptr<hc::session>> m_sessions;

        std::mutex m_mutex;
};