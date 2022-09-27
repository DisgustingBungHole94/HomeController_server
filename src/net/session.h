#pragma once

#include "handler.h"

#include <atomic>

class session {
    public:
        session(hc::net::ssl::server_conn_hdl conn_hdl, handler_type type) 
            : m_conn_hdl(conn_hdl)
        {
            set_type(type);
        }

        ~session() {}

        void on_data(const std::string& data);

        void set_type(handler_type type);
        handler_type get_type() { return m_type; }

    private:
        std::unique_ptr<handler> m_handler;
        handler_type m_type;

        hc::net::ssl::server_conn_hdl m_conn_hdl;
};