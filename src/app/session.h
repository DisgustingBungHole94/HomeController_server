#pragma once

#include "handler.h"

#include <homecontroller/net/ssl/tls_server.h>

class session {
    private:
        enum class session_type {
            HTTP, WS, DEVICE
        } m_type;

    public:
        session();

        ~session() {}

        void changeType(session_type type);

        bool onReady(hc::net::ssl::tls_server::connection_ptr& conn);

    private:
        std::unique_ptr<handler> m_handler;
};