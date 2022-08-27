#include "logout_module.h"

#include "../../util/string.h"

LogoutModule::LogoutModule() {}
LogoutModule::~LogoutModule() {}

HTTPResponse LogoutModule::execute(const std::string& method, const std::vector<std::string>& path, const rapidjson::Document& jsonDoc) {
    if (Util::toLowerCase(method) == "get") {
        try {
            if (!m_controller->getAuthManager()->deleteSession(getSession()->getId())) {
                return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-65000,\"error_msg\":\"bad session token\"}");
            }
        } catch(hc::exception& e) {
            return HTTPResponse("500 Internal Server Error", "{\"success\":false,\"error_code\":-70000,\"error_msg\":\"server error\"}");
        }

        return HTTPResponse("200 OK", "{\"success\":true}");
    }

    return HTTPResponse("405 Method Not Allowed", "{\"success\":false,\"error_code\":-30000,\"error_msg\":\"http method not allowed\"}");
}