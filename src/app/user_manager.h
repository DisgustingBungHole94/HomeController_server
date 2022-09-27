#pragma once

#include <homecontroller/app/user.h>

#include <mutex>

class homecontroller;

class user_manager {
    public:
        user_manager(homecontroller* controller)
            : m_controller(controller)
        {}

        ~user_manager() {}

        void load_users();

        void create_user(const hc::user& user);
        void delete_user(const std::string& username);

        std::shared_ptr<hc::user> get_user(const std::string& username);

    private:
        homecontroller* m_controller;

        std::map<std::string, std::shared_ptr<hc::user>> m_users;

        std::mutex m_mutex;
};