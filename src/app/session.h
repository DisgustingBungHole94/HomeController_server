#pragma once

#include <homecontroller/net/ssl/tls_connection.h>

enum class session_upgrade_type {
    NONE, HTTP, WS, DEVICE
};

class session {
    public:
        session() 
            : m_finished(false), m_upgrade_type(session_upgrade_type::NONE)
        {}

        ~session() {}

        virtual void init();
        virtual void on_data(const hc::net::ssl::connection_ptr& conn_ptr);

        void set_finished() { m_finished = true; }
        bool finished() { return m_finished; }

        void set_upgrade_type(session_upgrade_type type) { m_upgrade_type = type; }
        session_upgrade_type upgrade_type() { return m_upgrade_type; }

    private:
        bool m_finished;
        session_upgrade_type m_upgrade_type;
};