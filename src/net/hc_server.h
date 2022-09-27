#pragma once

#include "session.h"

#include <homecontroller/net/ssl/tls_server.h>
#include <homecontroller/thread/thread_pool.h>
#include <homecontroller/util/logger.h>

class hc_server : public hc::net::ssl::tls_server {
    public:
        hc_server()
            : m_logger("hc_server") 
        {}

        ~hc_server() {}

        void start_threads();
        void stop_threads();

        void on_connect(hc::net::ssl::server_conn_hdl conn_hdl);
        void on_data(hc::net::ssl::server_conn_hdl conn_hdl);
        void on_disconnect(hc::net::ssl::server_conn_hdl conn_hdl);

    private:
        void worker_thread(std::shared_ptr<session> session, const std::string data);

        hc::util::logger m_logger;

        hc::thread::thread_pool m_thread_pool;

        std::map<hc::net::ssl::server_conn_hdl, std::shared_ptr<session>, std::owner_less<hc::net::ssl::server_conn_hdl>> m_sessions;
};