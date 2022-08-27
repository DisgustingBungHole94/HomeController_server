#include "session.h"

#include "../http/http_handler.h"

#include <homecontroller/exception/exception.h>

session::session() {
    changeType(session_type::HTTP);
}

void session::changeType(session_type type) {
    m_type = type;
    
    switch(m_type) {
        case session_type::HTTP:
            m_handler.reset(new http_handler());
            break;
        case session_type::WS:
        case session_type::DEVICE:
        default:
            throw hc::exception("unimplemented", "");
            break;
    }

    m_handler->init();
}

bool session::onReady(hc::net::ssl::tls_server::connection_ptr& conn) {
    m_handler->onReady(conn);

    return !m_handler->finished();
}