#pragma once

#include "../net/handler.h"

#include <homecontroller/ws/connection/server_connection.h>
#include <homecontroller/ws/ws_server.h>

class ws_handler : public handler {
    public:
        ws_handler() {}
        ~ws_handler() {}

        void init(const hc::net::ssl::server_conn_hdl& conn_hdl, const std::string& upgrade_request);
        void on_data(const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& data);

        void on_message(const std::string& msg);

    private:
        hc::ws::server_conn_ptr m_ws_conn;

        // server object for generating ws connections
        static hc::ws::ws_server _server;
};