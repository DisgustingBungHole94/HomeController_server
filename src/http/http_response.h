#pragma once

#include <string>

#include <ctime>
#include <sstream>
#include <iomanip>
#include <iostream>

class HTTPResponse {
    public:
        HTTPResponse()
            : HTTPResponse("", "")
        {} 

        HTTPResponse(const std::string& status, const std::string& body, bool json = true)
            : m_headers(""), m_body(body)
        {
            m_status = "HTTP/1.1 " + status;

            addHeader("Date", timestamp());
            addHeader("Server", "HomeController/2.0.0 (Debian)");

            addHeader("Access-Control-Allow-Origin", "*");
            addHeader("Access-Control-Max-Age", "86400");
            addHeader("Access-Control-Allow-Headers", "Authorization");

            if (json) addHeader("Content-Type", "application/json");
        }

        ~HTTPResponse() {};

        void addHeader(const std::string& key, const std::string& value) {
            m_headers += key + ": " + value + "\r\n";
        }

        void allowedMethods(const std::string& methods) {
            addHeader("Access-Control-Allow-Methods", methods);
        }

        const std::string& getStatus() { return m_status; }

        std::string str() {
            addHeader("Content-Length", std::to_string(m_body.length()));

            // one \r\n after headers because second is added with last header
            return m_status + "\r\n" + m_headers + "\r\n" + m_body;
        }

    private:
        std::string m_status;
        std::string m_headers;
        std::string m_body;

        std::string timestamp() {
            auto t = std::time(nullptr);

            std::ostringstream oss;
            oss << std::put_time(std::localtime(&t), "%a, %d %b %Y %H:%M:%S %Z");

            return oss.str();
        }
};