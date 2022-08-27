#pragma once

#include <homecontroller/net/ssl/tls_server.h>

class handler {
    public:
        handler() 
            : m_finished(false)
        {}

        ~handler() {}

        virtual void init();

        virtual void onReady(hc::net::ssl::tls_server::connection_ptr& conn);

        void setFinished() { m_finished = true; }
        bool finished() { return m_finished; }

    private:
        bool m_finished;
};