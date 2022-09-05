#pragma once

#include "net/hc_server.h"

#include <homecontroller/util/logger.h>
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
        hc::util::logger m_logger;

        homecontroller_status m_status;

        hc_server m_server;
};