#include "http_handler.h"

#include "../homecontroller.h"

#include "modules/login_module.h"
#include "modules/logout_module.h"
#include "modules/register_device_module.h"

#include <homecontroller/util/logger.h>
#include <homecontroller/exception/exception.h>
#include <homecontroller/util/string.h>
#include <homecontroller/http/common_responses.h>
#include <homecontroller/app/info.h>

#include <iostream>
#include <unordered_map>
#include <functional>

void http_handler::init() {
    m_parser.init(hc::http::http_parser::parser_type::REQUEST);
}

void http_handler::on_data(const hc::net::ssl::server_conn_ptr& conn_ptr, const std::string& data) {    
    try {
        if (conn_ptr->closed()) {
            hc::util::logger::dbg("socket closed");

            set_finished();
            return;
        }
        
        if (!m_parser.parse(data)) {
            hc::util::logger::dbg("partial HTTP request parsed, waiting for more data...");
            return;
        }

        hc::http::http_request req = m_parser.get_request();
        hc::http::http_response res;

        if (req.should_upgrade()) {
            hc::util::logger::log("received upgrade request");

            std::string upgrade_header_value;
            if (!req.get_header("upgrade", upgrade_header_value)) {
                hc::util::logger::dbg("request missing upgrade header");

                res = hc::http::common::bad_http_request();
                add_headers(res);

                conn_ptr->send(res.str());

                return;
            }

            if (upgrade_header_value == "websocket") {
                hc::util::logger::log("UPGRADE HTTP -> WebSocket");
                set_upgrade_type(handler_type::WS);
            } 
            
            else if (upgrade_header_value == "hcdevice") {
                hc::util::logger::log("UPGRADE HTTP -> HCDevice");
                set_upgrade_type(handler_type::DEVICE);
            } 
            
            else {
                hc::util::logger::dbg("request tried to upgrade to unsupported protocol");

                res = hc::http::common::bad_http_request();
                add_headers(res);

                conn_ptr->send(res.str());

                return;
            }

            set_upgrade_request(data);
            set_finished();

            return;
        }
 
        std::string connection_header_val;
        if (req.get_header("connection", connection_header_val) && 
            hc::util::str::to_lower_case(connection_header_val) == "keep-alive") 
        {
            hc::util::logger::dbg("connection will be kept alive");
            res.add_header("Connection", "keep-alive");
        } else {
            hc::util::logger::dbg("connection will be closed");
            set_finished();
        }

        handle_request(req, res);
        add_headers(res);        

        hc::util::logger::log(req.get_method() + " " + req.get_url() + " -> " + res.get_status());

        conn_ptr->send(res.str());

        hc::util::logger::dbg("request processed in " + std::to_string(conn_ptr->time_since_create()) + "ms");
        conn_ptr->reset_timer();

    } catch(hc::exception& e) {
        hc::util::logger::err("client [" + conn_ptr->get_ip() + "] error: " + e.what() + " (" + e.func() + ")");

        set_finished();
        return;
    }
}

void http_handler::handle_request(const hc::http::http_request& request, hc::http::http_response& response_ref) {
    static const std::unordered_map<std::string, std::function<module*()>> modules = {
        { "login",              [this]() { return new login_module(m_controller); } },
        { "logout",             [this]() { return new logout_module(m_controller); } },
        { "register_device",    [this]() { return new register_device_module(m_controller); }}
    };

    std::vector<std::string> path = hc::util::str::split_string(request.get_url(), '/');
    if (path.size() < 2 || path[1] == "") {
        response_ref = hc::http::common::not_found();
    } else {
        std::unique_ptr<module> module_ptr = nullptr;
        
        auto mit = modules.find(path[1]);
        if (mit == modules.end()) {
            response_ref = hc::http::common::not_found();
            return;
        }

        module_ptr.reset(mit->second());
        
        if(hc::util::str::to_lower_case(request.get_method()) == "options") {
            response_ref = hc::http::http_response("204 No Content", "");
            response_ref.add_header("Access-Control-Allow-Methods", module_ptr->get_allowed_methods());
        }

        else {
            if (module_ptr->auth_required()) {
                std::string auth_header_val;
                
                if (!request.get_header("authorization", auth_header_val)) {
                    response_ref = hc::http::common::need_auth();   
                    response_ref.add_header("WWW-Authenticate", hc::info::NAME);
                    return;
                }
                
                std::vector<std::string> auth_header_split = 
                    hc::util::str::split_string(auth_header_val, ' ');
                if (auth_header_split.size() < 2 || auth_header_split[0] != hc::info::NAME) {
                    response_ref = hc::http::common::need_auth();   
                    response_ref.add_header("WWW-Authenticate", hc::info::NAME);
                    return;
                }

                std::shared_ptr<hc::session> session_ptr = 
                        m_controller->get_auth_manager()->get_session(auth_header_split[1]);
                if (session_ptr == nullptr) {
                    response_ref = hc::http::common::need_auth();
                    return;
                }
                
                module_ptr->set_session_ptr(session_ptr);
            }

            try {
                response_ref = module_ptr->execute(request.get_method(), path, request.get_body());
                response_ref.add_header("Content-Type", "application/json");
            } catch(hc::exception& e) {
                response_ref = hc::http::common::server_error();
            }
        }
    }
}

void http_handler::add_headers(hc::http::http_response& response_ref) {
        response_ref.add_header("Content-Length", std::to_string(response_ref.get_body().length()));

        response_ref.add_header("Server", hc::info::NAME + "/" + hc::info::VERSION + " (" + hc::info::OS + ")");

        response_ref.add_header("Access-Control-Allow-Origin", "*");
        response_ref.add_header("Access-Control-Max-Age", "86400");
        response_ref.add_header("Access-Control-Allow-Headers", "Authorization, Content-Type");
}