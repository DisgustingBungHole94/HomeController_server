#pragma once

#include "../app/handler.h"

#include <homecontroller/util/logger.h>
#include <homecontroller/http/http_parser.h>

class http_handler : public handler {
    public:
        http_handler() 
            : m_logger("http_handler"), m_continueParsing(false)
        {}

        ~http_handler() {}

        virtual void init();

        virtual void onReady(hc::net::ssl::tls_server::connection_ptr& conn);

    private:
        hc::util::logger m_logger;

        hc::http::http_parser m_parser;
        bool m_continueParsing;
};