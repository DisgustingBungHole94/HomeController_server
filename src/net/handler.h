#pragma once

#include <homecontroller/net/ssl/connection/server_connection.h>

enum class handler_type {
    NONE, HTTP, WS, DEVICE
};

class handler {
    public:
        handler() 
            : m_finished(false), m_upgrade_type(handler_type::NONE)
        {}

        ~handler() {}

        virtual void on_data(const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& data);

        void set_finished() { m_finished = true; }
        bool finished() { return m_finished; }

        void set_upgrade_type(handler_type type) { m_upgrade_type = type; }
        handler_type get_upgrade_type() { return m_upgrade_type; }

        void set_upgrade_request(const std::string& request) { m_upgrade_request = request; }
        const std::string& get_upgrade_request() { return m_upgrade_request; }

    private:
        bool m_finished;

        handler_type m_upgrade_type;
        std::string m_upgrade_request;
};