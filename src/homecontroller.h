#pragma once

#include "app/user_manager.h"
#include "app/auth_manager.h"

#include "net/hc_server.h"

#include <homecontroller/exception/exception.h>

#include <iostream>
#include <memory>
#include <mutex>

enum class homecontroller_status {
    RUNNING, STOPPED
};

struct homecontroller_config {
    int m_server_port;
    std::string m_tls_cert_file;
    std::string m_tls_priv_key_file;

    int m_connection_expire_time;
    int m_session_expire_time;

    std::string m_log_mode;
};

class homecontroller {
    public:
        homecontroller() 
            : m_status(homecontroller_status::STOPPED),
            m_server(this),
            m_auth_manager(this),
            m_user_manager(this)
        {}

        ~homecontroller() {}

        bool start();
        void loop();
        void shutdown();

        void signal_interrupt(int s);
        void signal_pipe(int s);
        void signal_segv(int s);

        user_manager* get_user_manager() { return &m_user_manager; }
        auth_manager* get_auth_manager() { return &m_auth_manager; }

    private:
        homecontroller_status m_status;

        hc_server m_server;

        user_manager m_user_manager;
        auth_manager m_auth_manager;
};