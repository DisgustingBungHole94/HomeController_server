#pragma once

#include "../net/handler.h"

#include <homecontroller/http/http_parser.h>

#include <iostream>

class http_handler : public handler {
    public:
        http_handler(homecontroller* controller) 
            : handler(controller)
        {}
        
        ~http_handler() {}

        void init();
        void on_data(const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& data);

    private:
        enum class module_type {
            NONE, LOGIN, LOGOUT, REGISTER_DEVICE
        };

        void handle_request(const hc::http::http_request& request, hc::http::http_response& response_ref);
        void add_headers(hc::http::http_response& response_ref);

        hc::http::http_parser m_parser;
        bool m_continue_parsing;
};