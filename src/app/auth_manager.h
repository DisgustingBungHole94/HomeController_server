#pragma once

#include <homecontroller/app/session.h>

#include <mutex>

class homecontroller;

enum class auth_result {
    OK, NO_USER, INC_PASS
};

class auth_manager {
    public:
        auth_manager(homecontroller* controller) 
            : m_controller(controller)
        {}

        ~auth_manager() {}

        auth_result create_session(const std::string& username, const std::string& password, const std::string& ip, std::string& session_id_ref);

        bool delete_session(const std::string& id);
        void delete_sessions_by_username(const std::string& username);

        std::shared_ptr<hc::session> get_session(const std::string& id);

    private:
        homecontroller* m_controller;

        std::map<std::string, std::shared_ptr<hc::session>> m_sessions;

        std::mutex m_mutex;
};