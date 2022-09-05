#include "http_session.h"

#include <homecontroller/exception/exception.h>

void http_session::init() {
    m_parser.init(hc::http::http_parser::parser_type::REQUEST);
}

void http_session::on_data(const hc::net::ssl::connection_ptr& conn_ptr) {
    std::string data;

    try {
        data = conn_ptr->recv();
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
                set_upgrade_type(session_upgrade_type::WS);
            } else if (upgrade_header_value == "hcdevice") {
                m_logger.log("UPGRADE HTTP -> HCDevice");
                set_upgrade_type(session_upgrade_type::DEVICE);

                hc::http::http_response res("101 Switching Protocols", "");
                res.add_header("Upgrade", "hcdevice");
                res.add_header("Connection", "Upgrade");

                conn_ptr->send(res.str());
            } else {
                m_logger.dbg("request tried to upgrade to unsupported protocol");

                hc::http::http_response res("400 Bad Request", "{\"success\":false,\"error_code\":-10000,\"error_msg\":\"bad http request\"}");
                conn_ptr->send(res.str());

                return;
            }

            set_finished();
        }

        m_logger.log("request method: " + req.get_method());
        m_logger.log("request url: " + req.get_url());

        hc::http::http_response res("200 OK", "test");
        res.add_header("Content-Length", "4");

        conn_ptr->send(res.str());
    } catch(hc::exception& e) {
        m_logger.err("client [" + conn_ptr->get_ip() + "] error: " + e.what() + " (" + e.func() + ")");

        set_finished();
        return;
    }
}