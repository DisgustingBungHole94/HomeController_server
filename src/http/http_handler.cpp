#include "http_handler.h"

#include <homecontroller/exception/exception.h>
#include <homecontroller/util/string.h>

#include <iostream>

void http_handler::init() {
    m_parser.init(hc::http::http_parser::parser_type::REQUEST);
}

void http_handler::on_data(const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& data) {
    try {
        if (conn_ptr->closed()) {
            m_logger.dbg("socket closed");

            set_finished();
            return;
        }
        
        if (!m_parser.parse(data)) {
            m_logger.dbg("partial HTTP request parsed, waiting for more data...");
            return;
        }

        hc::http::http_request req = m_parser.get_request();
        hc::http::http_response res;

        if (req.should_upgrade()) {
            m_logger.log("received upgrade request");

            std::string upgrade_header_value;
            if (!req.get_header("upgrade", upgrade_header_value)) {
                m_logger.dbg("request missing upgrade header");

                hc::http::http_response res("400 Bad Request", "{\"success\":false,\"error_code\":-10000,\"error_msg\":\"bad http request\"}");
                conn_ptr->send(res.str());

                return;
            }

            if (upgrade_header_value == "websocket") {
                m_logger.log("UPGRADE HTTP -> WebSocket");
                set_upgrade_type(handler_type::WS);
            } else if (upgrade_header_value == "hcdevice") {
                m_logger.log("UPGRADE HTTP -> HCDevice");
                set_upgrade_type(handler_type::DEVICE);
            } else {
                m_logger.dbg("request tried to upgrade to unsupported protocol");

                hc::http::http_response res("400 Bad Request", "{\"success\":false,\"error_code\":-10000,\"error_msg\":\"bad http request\"}");
                conn_ptr->send(res.str());

                return;
            }

            set_upgrade_request(data);
            set_finished();

            return;
        }
 
        std::string connection_header_value;
        if (req.get_header("connection", connection_header_value) && 
            hc::util::str::to_lower_case(connection_header_value) == "keep-alive") 
        {
            m_logger.dbg("connection will be kept alive");
            res.add_header("Connection", "keep-alive");
        } else {
            m_logger.dbg("connection will be closed");
            set_finished();
        }

        m_logger.log("request method: " + req.get_method());
        m_logger.log("request url: " + req.get_url());

        res.set_status("200 OK");
        res.set_body("test");

        res.add_header("Content-Length", "4");
        res.add_header("Content-Type", "text/html");

        conn_ptr->send(res.str());

        m_logger.dbg("request processed in " + std::to_string(conn_ptr->time_since_create()) + "ms");
        conn_ptr->reset_timer();
    } catch(hc::exception& e) {
        m_logger.err("client [" + conn_ptr->get_ip() + "] error: " + e.what() + " (" + e.func() + ")");

        set_finished();
        return;
    }
}