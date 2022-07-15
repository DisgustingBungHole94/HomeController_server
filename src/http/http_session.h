#pragma once

#include <memory>
#include <string>
#include <map>

#include "../thread/job.h"
#include "http_response.h"
#include "../util/logger.h"
#include "../net/tls_server.h"

class HomeController;

class HTTPSession : public Job {
    public:
        HTTPSession(HomeController* controller, std::unique_ptr<TLSClient> client);
        ~HTTPSession();

        void run();
        void stop();

    private:
        Logger m_logger;

        HomeController* m_controller;

        std::unique_ptr<TLSClient> m_tlsClient;

        HTTPResponse m_response;

        struct ParserData {
            std::string m_method = "";
            std::string m_url = "";
            std::string m_body = "";

            std::string m_lastHeaderField = "";
            std::map<std::string, std::string> m_headers;

            bool m_finished = false;
        };

        void parseRequest();
        void handleRequest(HTTPSession::ParserData* data);

        bool m_sessionFinished;
};
