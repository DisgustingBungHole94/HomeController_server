#include "http_session.h"

#include "http_module_manager.h"
#include "../util/string.h"
#include "../util/exception/socket_close_exception.h"
#include "../util/exception/socket_timeout_exception.h"

#include <llhttp.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include <vector>

HTTPSession::HTTPSession(HomeController* controller, std::unique_ptr<TLSClient> client)
    : m_logger("HS@" + client->getIp()), m_controller(controller), m_tlsClient(std::move(client)), m_sessionFinished(false)
{}

HTTPSession::~HTTPSession() {}

void HTTPSession::run() {
    try {
        parseRequest();
    } catch(hc::exception& e) {
        if (!m_sessionFinished) {
            m_logger.err("Client error: " + e.what() + " (" + e.func() + ")");
        }
    }

    stop();

    m_tlsClient.reset();
}

void HTTPSession::stop() {
    if (!m_sessionFinished) {
        m_sessionFinished = true;

        m_tlsClient->stop();

        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsedTime = endTime - m_tlsClient->getStartTime();

        m_logger.dbg("Client session exited, total lifetime: " + std::to_string(elapsedTime.count()) + "s");
    }
}

void HTTPSession::parseRequest() {
    // initialize ParserData ptr, will contain HTTP request data
    std::unique_ptr<HTTPSession::ParserData> data = std::make_unique<HTTPSession::ParserData>();

    llhttp_t parser;
    llhttp_settings_t settings;

    llhttp_init(&parser, HTTP_REQUEST, &settings);

    parser.data = data.get();

    llhttp_settings_init(&settings);

    // store request URL
    settings.on_url = [](llhttp_t* parser, const char* at, std::size_t len) -> int {
        HTTPSession::ParserData* dataPtr = (HTTPSession::ParserData*)parser->data;
        dataPtr->m_url = std::string(at, len);

        return 0;
    };

    // store request body
    settings.on_body = [](llhttp_t* parser, const char* at, std::size_t len) -> int {
        HTTPSession::ParserData* dataPtr = (HTTPSession::ParserData*)parser->data;
        dataPtr->m_body = std::string(at, len);

        return 0;
    };

    // record header field name
    settings.on_header_field = [](llhttp_t* parser, const char* at, std::size_t len) -> int {
        HTTPSession::ParserData* dataPtr = (HTTPSession::ParserData*)parser->data;

        std::string field = std::string(at, len);
        dataPtr->m_lastHeaderField = Util::toLowerCase(field);

        return 0;
    };

    // store header field name and value
    settings.on_header_value = [](llhttp_t* parser, const char* at, std::size_t len) -> int {
        HTTPSession::ParserData* dataPtr = (HTTPSession::ParserData*)parser->data;

        if (dataPtr->m_lastHeaderField != "") {
            dataPtr->m_headers.insert(std::make_pair(dataPtr->m_lastHeaderField, std::string(at, len)));
            dataPtr->m_lastHeaderField = "";
        }

        return 0;
    };

    // mark HTTP request as fully received
    settings.on_message_complete = [](llhttp_t* parser) -> int {
        HTTPSession::ParserData* dataPtr = (HTTPSession::ParserData*)parser->data;
        dataPtr->m_finished = true;

        return 0;
    };

    // determines whether client connection connection be kept alive
    // true when request is sent in multiple TCP messages and
    // when request has Connection: keep-alive header
    bool keepAlive;

    // number of requests handled for a connection that has been kept alive
    int requestsHandled = 0;

    do {
        // reset connection keep-alive status
        keepAlive = false;
        // reset HTTP request retrieval status
        data->m_finished = false;

        // string containing raw HTTP request
        std::string request;

        int requestLen;

        //  receive data from client
        try {
            request = m_tlsClient->recv();
            requestLen = request.length();
        } catch(SocketCloseException& e) {
            // thrown when connection is closed by client
            m_logger.dbg("Socket closed by client!");
            continue;
        } catch(SocketTimeoutException& e) {
            // thrown when client hasn't sent data for some time
            m_logger.dbg("Socket timed out!");
            continue;
        }

        // parse raw HTTP request and obtain its data
        enum llhttp_errno err = llhttp_execute(&parser, request.c_str(), requestLen);
        if (err == HPE_PAUSED_UPGRADE) {
            //m_logger.log("UPGRADE HTTP -> WebSocket");

            auto mit = data->m_headers.find("upgrade");
            if (mit == data->m_headers.end()) {
                m_logger.err("Failed to upgrade HTTP request, no Upgrade header.");
                m_response = HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-10000,\"error_msg\":\"bad http request\"}");

                m_tlsClient->send(m_response.str());

                continue;
            }

            if (mit->second == "websocket") {
                m_logger.log("UPGRADE HTTP -> WebSocket");

                m_controller->getWSServer()->startSession(std::move(m_tlsClient), request);
            } else if (mit->second == "hcdevice") {
                m_logger.log("UPGRADE HTTP -> HCDevice");

                m_response = HTTPResponse("101 Switching Protocols", "", false);
                m_response.addHeader("Upgrade", "hcdevice");
                m_response.addHeader("Connection", "Upgrade");

                m_tlsClient->send(m_response.str());

                m_controller->getDeviceServer()->startSession(std::move(m_tlsClient));
            } else {
                m_logger.err("Failed to upgrade HTTP request, unsupported protocol.");
                m_response = HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-10000,\"error_msg\":\"bad http request\"}");

                m_tlsClient->send(m_response.str());

                continue;
            }

            m_sessionFinished = true;
            continue;
        } else if (err != HPE_OK) {
            m_logger.err("Failed to parse HTTP request: " + std::string(llhttp_errno_name(err)) + " " + std::string(parser.reason));
            m_response = HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-10000,\"error_msg\":\"bad http request\"}");

            m_tlsClient->send(m_response.str());

            continue;
        }

        // if HTTP request is sent in separate TCP messages, receive more data
        if (!data->m_finished) {
            m_logger.dbg("Partial HTTP request parsed, continuing...");

            keepAlive = true;
            continue;
        }

        data->m_method = std::string(llhttp_method_name((llhttp_method_t)parser.method));

        m_logger.dbg("Finished parsing HTTP request.");

        // parse body of HTTP request
        handleRequest(data.get());
        
        // check HTTP request for Connection: keep-alive header
        auto mit = data->m_headers.find("connection");
        if (mit != data->m_headers.end()) {
            if (Util::toLowerCase(mit->second) == "keep-alive") {
                // will cause loop to continue
                keepAlive = true;
            }
        }
        if (keepAlive) {
            m_logger.dbg("Connection will be kept alive, " + std::to_string(++requestsHandled) + " request(s) handled so far.");
            m_response.addHeader("Connection", "keep-alive");
        } else {
            m_logger.dbg("Connection set to close.");
        }

        m_logger.log("ACCESS " + data->m_url + " (" + data->m_method + ") -> " + m_response.getStatus());

        // convert HTTP response to string and send to client
        m_tlsClient->send(m_response.str());
    } while(keepAlive);
}

void HTTPSession::handleRequest(HTTPSession::ParserData* data) {
    std::vector<std::string> path = Util::splitString(data->m_url, '/');
    if (path.size() < 2 || path[1] == "") {
        m_response = HTTPResponse("404 Not Found", "{\"success\":false,\"error_code\":-15000,\"error_msg\":\"no module selected\"}");
        return;
    }

    rapidjson::Document document;
    if (data->m_body != "") {
        rapidjson::ParseResult res = document.Parse(data->m_body.c_str());

        if (!res) {
            m_logger.dbg("JSON parse error: " + std::string(rapidjson::GetParseError_En(res.Code())) + " (" + std::to_string(res.Offset()) + ")");
            m_response = HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-20000,\"error_msg\":\"json parse error\"}");
            return;
        }

        m_logger.dbg("JSON parse successful.");
    } else {
        document.Parse("{}");
    }

    std::unique_ptr<Module> module = HTTPModuleManager::getModule(path[1]);
    if (module == nullptr) {
        m_response = HTTPResponse("404 Not Found", "{\"success\":false,\"error_code\":-25000,\"error_msg\":\"unknown module\"}");
        return;
    }

    if (Util::toLowerCase(data->m_method) == "options") {
        m_response = HTTPResponse("204 No Content", "", false);
        m_response.allowedMethods(module->getAllowedMethods());
    } else {
        module->setController(m_controller);

        if (module->authRequired()) {
            auto mit = data->m_headers.find("authorization");
            if (mit == data->m_headers.end()) {
                m_response = HTTPResponse("401 Unauthorized", "{\"success\":false,\"error_code\":-55000,\"error_msg\":\"missing session token\"}");
                m_response.addHeader("WWW-Authenticate", "HomeController");
                return;
            }

            std::vector<std::string> authHeader = Util::splitString(mit->second, ' ');
            if (authHeader.size() < 2 || authHeader[0] != "HomeController") {
                m_response = HTTPResponse("401 Unauthorized", "{\"success\":false,\"error_code\":-60000,\"error_msg\":\"bad authorization header\"}");
                m_response.addHeader("WWW-Authenticate", "HomeController");
                return;
            }

            SessionPtr session = m_controller->getAuthManager()->getSession(authHeader[1]);
            if (session == nullptr) {
                m_response = HTTPResponse("401 Unauthorized", "{\"success\":false,\"error_code\":-65000,\"error_msg\":\"bad session token\"}");
                return;
            }

            module->setSession(session);
        }

        m_response = module->execute(data->m_method, path, document);
    }
}