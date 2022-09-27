#include "session.h"

#include "../http/http_handler.h"
#include "../ws/ws_handler.h"

void session::on_data(const std::string& data) {
    auto conn_ptr = hc::net::ssl::server_connection::conn_from_hdl(m_conn_hdl);
    m_handler->on_data(conn_ptr, data);
    
    if (m_handler->finished()) {
        handler_type upgrade_type = m_handler->get_upgrade_type();

        if (upgrade_type != handler_type::NONE) {
            set_type(upgrade_type);
        } else {
            conn_ptr->close();
        }
    }

    conn_ptr->unlock();
}

void session::set_type(handler_type type) {
    switch(type) {
        case handler_type::HTTP:
            {
                http_handler* h = new http_handler();
                h->init();

                m_handler.reset(h);
            }
            break;
        case handler_type::WS:
            {
                ws_handler* h = new ws_handler();
                h->init(m_conn_hdl, m_handler->get_upgrade_request());

                m_handler.reset(h);
            }
            break;
        case handler_type::DEVICE:
            break;
        case handler_type::NONE:
        default:
            m_handler.reset(new handler());
            break;
    }
}