#pragma once

#include "app/session.h"

#include <homecontroller/util/logger.h>
#include <homecontroller/net/ssl/tls_server.h>
#include <homecontroller/exception/exception.h>
#include <homecontroller/thread/thread_pool.h>

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
            : m_logger("main"), m_status(homecontroller_status::STOPPED)
        {}

        ~homecontroller() {}

        bool start();
        void loop();
        void shutdown();

        void signal_interrupt(int s);
        void signal_pipe(int s);
        void signal_segv(int s);

    private:
        void on_connect(hc::net::ssl::tls_server::connection_hdl hdl);
        void on_ready(hc::net::ssl::tls_server::connection_hdl hdl);
        void on_disconnect(hc::net::ssl::tls_server::connection_hdl hdl);

        void run_session(hc::net::ssl::tls_server::connection_hdl hdl);

        hc::util::logger m_logger;

        hc::net::ssl::tls_server m_server;
        std::map<hc::net::ssl::tls_server::connection_hdl, std::unique_ptr<session>, std::owner_less<hc::net::ssl::tls_server::connection_hdl>> m_sessions;

        hc::thread::thread_pool m_thread_pool;

        homecontroller_status m_status;

        std::mutex m_mutex;
};