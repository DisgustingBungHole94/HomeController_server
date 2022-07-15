#pragma once

#include <websocketpp/config/core.hpp>
#include <websocketpp/server.hpp>

namespace ws {
    struct ServerConfig : public websocketpp::config::core {
        typedef websocketpp::transport::iostream::endpoint<websocketpp::config::core::transport_config> transport_type;
    };

    typedef websocketpp::server<ServerConfig> server;
}