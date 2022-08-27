#include "http_handler.h"

#include <homecontroller/exception/exception.h>

void http_handler::init() {
    m_parser.init(hc::http::http_parser::parser_type::REQUEST);
}

void http_handler::onReady(hc::net::ssl::tls_server::connection_ptr& conn) {
    std::string data;
    
    try {
        data = conn->recv();

        if (conn->closed()) {
            m_logger.log("socket closed");
            
            return;
        }
    } catch(hc::exception& e) {
        m_logger.err("client [" + conn->get_ip() + "] error: " + std::string(e.what()) + " (" + std::string(e.func()) + ")");
        setFinished();

        return;
    }

    try {
        if (!m_parser.parse(data)) {
            m_logger.dbg("partial HTTP request parsed, waiting for more data...");
            return;
        }
    } catch(hc::exception& e) {
        m_logger.err("client [" + conn->get_ip() + "] error: " + std::string(e.what()) + " (" + std::string(e.func()) + ")");
        setFinished();

        return;
    }

    hc::http::http_request req = m_parser.getRequest();

    m_logger.log("request method: " + req.getMethod());
    m_logger.log("request url: " + req.getUrl());

    hc::http::http_response res("200 OK", "test");
    res.addHeader("Content-Length", "4");

    try {
        conn->send(res.toString());
    } catch(hc::exception& e) {
        m_logger.err("client [" + conn->get_ip() + "] error: " + std::string(e.what()) + " (" + std::string(e.func()) + ")");
        setFinished();

        return;
    }
}