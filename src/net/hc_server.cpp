#include "hc_server.h"

#include <homecontroller/exception/exception.h>

void hc_server::start_threads() {
    m_thread_pool.start(std::thread::hardware_concurrency());
}

void hc_server::stop_threads() {
    m_thread_pool.stop();
}

void hc_server::on_connect(hc::net::ssl::connection_hdl conn_hdl) {
    std::unique_ptr<http_session> s = std::make_unique<http_session>();
    s->init();
    
    m_sessions.insert(std::make_pair(conn_hdl, std::move(s)));
}

void hc_server::on_data(hc::net::ssl::connection_hdl conn_hdl) {
    m_thread_pool.add_job(std::bind(&hc_server::worker_thread, this, conn_hdl));
}

void hc_server::on_disconnect(hc::net::ssl::connection_hdl conn_hdl) {
    auto mit = m_sessions.find(conn_hdl);
    if (mit == m_sessions.end()) {
        m_logger.err("disconnect error: connection is not associated with session");
        return;
    }

    m_sessions.erase(mit);
}

void hc_server::worker_thread(hc::net::ssl::connection_hdl conn_hdl) {
    auto mit = m_sessions.find(conn_hdl);
    if (mit == m_sessions.end()) {
        m_logger.err("on_data error: connection is not associated with session");
        return;
    }

    try {
        auto conn = hc::net::ssl::tls_connection::conn_from_hdl(conn_hdl);

        mit->second->on_data(conn);

        if (mit->second->finished()) {
            session_upgrade_type upgrade_type = mit->second->upgrade_type();
            if (upgrade_type != session_upgrade_type::NONE) {
                switch(upgrade_type) {
                    case session_upgrade_type::WS:
                        break;
                    case session_upgrade_type::DEVICE:
                        break;
                    default:
                        break;
                }
            } else {
                conn->close();
            }

            m_sessions.erase(mit);
        }
    } catch(hc::exception& e) {
        m_logger.err("client error: " + std::string(e.what()) + " (" + std::string(e.func()) + ")");
    }
}