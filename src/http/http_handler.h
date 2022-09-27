#pragma once

#include "../net/handler.h"

#include <homecontroller/util/logger.h>
#include <homecontroller/http/http_parser.h>

#include <iostream>

class http_handler : public handler {
    public:
        http_handler() 
            : m_logger("http_session")
        {}
        
        ~http_handler() {}

        void init();
        void on_data(const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& data);

    private:
        hc::util::logger m_logger;

        hc::http::http_parser m_parser;
        bool m_continue_parsing;
};