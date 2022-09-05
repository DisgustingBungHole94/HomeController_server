#pragma once

#include "../app/session.h"

#include <homecontroller/util/logger.h>
#include <homecontroller/http/http_parser.h>

class http_session : public session {
    public:
        http_session() 
            : m_logger("http_session")
        {}
        
        ~http_session() {}

        void init();
        void on_data(const hc::net::ssl::connection_ptr& conn_ptr);

    private:
        hc::util::logger m_logger;

        hc::http::http_parser m_parser;
        bool m_continue_parsing;
};