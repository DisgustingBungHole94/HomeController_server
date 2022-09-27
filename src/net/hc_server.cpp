#include "hc_server.h"

#include <homecontroller/exception/exception.h>

#include <iostream>

void hc_server::start_threads() {
    m_thread_pool.start(std::thread::hardware_concurrency());
}

void hc_server::stop_threads() {
    m_thread_pool.stop();
}

void hc_server::on_connect(hc::net::ssl::server_conn_hdl conn_hdl) {
    std::shared_ptr<session> s = std::make_shared<session>(conn_hdl, handler_type::HTTP);    
    m_sessions.insert(std::make_pair(conn_hdl, s));
}

void hc_server::on_data(hc::net::ssl::server_conn_hdl conn_hdl) {
    auto mit = m_sessions.find(conn_hdl);
    if (mit == m_sessions.end()) {
        m_logger.err("on_data error: connection is not associated with session");
        return;
    }
    
    hc::net::ssl::server_conn_ptr conn_ptr;
    std::string data;

    try {
        conn_ptr = hc::net::ssl::server_connection::conn_from_hdl(conn_hdl);
        data = conn_ptr->recv();
    } catch(hc::exception& e) {
        m_logger.err("on_data error: " + std::string(e.what()) + " (" + std::string(e.func()) + ")");
        return;
    }

    // multithread safety
    conn_ptr->lock();

    m_thread_pool.add_job(std::bind(&hc_server::worker_thread, this, mit->second, data));
}

void hc_server::on_disconnect(hc::net::ssl::server_conn_hdl conn_hdl) {
    auto mit = m_sessions.find(conn_hdl);
    if (mit == m_sessions.end()) {
        m_logger.err("disconnect error: connection is not associated with session");
        return;
    }

    m_sessions.erase(mit);
}

void hc_server::worker_thread(std::shared_ptr<session> session, const std::string data) {
    try {
        session->on_data(data);
    } catch(hc::exception& e) {
        m_logger.err("client error: " + std::string(e.what()) + " (" + std::string(e.func()) + ")");
    }
}