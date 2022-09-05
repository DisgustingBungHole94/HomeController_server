#pragma once

#include "../http/http_session.h"

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

        void on_connect(hc::net::ssl::connection_hdl conn_hdl);
        void on_data(hc::net::ssl::connection_hdl conn_hdl);
        void on_disconnect(hc::net::ssl::connection_hdl conn_hdl);

    private:
        void worker_thread(hc::net::ssl::connection_hdl conn_hdl);

        hc::util::logger m_logger;

        hc::thread::thread_pool m_thread_pool;

        std::map<hc::net::ssl::connection_hdl, std::unique_ptr<session>, std::owner_less<hc::net::ssl::connection_hdl>> m_sessions;
};