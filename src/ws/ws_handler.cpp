#include "ws_handler.h"

hc::ws::ws_server ws_handler::_server;

void ws_handler::init(const hc::net::ssl::server_conn_hdl& conn_hdl, const std::string& upgrade_request) {
    m_ws_conn = _server.connect(conn_hdl, upgrade_request);

    m_ws_conn->set_message_callback(std::bind(&ws_handler::on_message, this, std::placeholders::_1));
    m_ws_conn->start();

    if (m_ws_conn->finished()) {
        set_finished();
    }
}

void ws_handler::on_data(const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& data) {    
    m_ws_conn->read_data(data);

    if (m_ws_conn->finished()) {
        set_finished();
    }
}

void ws_handler::on_message(const std::string& msg) {
    std::cout << msg << std::endl;
    
    m_ws_conn->send(msg);
}