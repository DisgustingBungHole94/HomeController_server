#include "login_module.h"

#include "../../util/string.h"

LoginModule::LoginModule() {}
LoginModule::~LoginModule() {}

HTTPResponse LoginModule::execute(const std::string& method, const std::vector<std::string>& path, const rapidjson::Document& jsonDoc) {
    if (Util::toLowerCase(method) == "post") {
        if (! (jsonDoc.HasMember("username") && jsonDoc["username"].IsString()
            && jsonDoc.HasMember("password") && jsonDoc["password"].IsString()))
        {
            return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-35000,\"error_msg\":\"missing required value or wrong type\"}");
        }

        std::string username = jsonDoc["username"].GetString();
        std::string password = jsonDoc["password"].GetString();

        std::string sessionId;
        AuthManager::Error err = m_controller->getAuthManager()->createSession(sessionId, username, password, m_ip);

        if (err != AuthManager::Error::NONE) {
            switch(err) {
                case AuthManager::Error::USER_NOT_FOUND:
                    return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-40000,\"error_msg\":\"user does not exist\"}");
                    break;
                case AuthManager::Error::INCORRECT_PASSWORD:
                    return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-45000,\"error_msg\":\"incorrect password\"}");
                    break;
                default:
                    return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-5000,\"error_msg\":\"unknown error\"}");
                    break;
            }
        }

        return HTTPResponse("200 OK", "{\"success\":true,\"token\":\"" + sessionId + "\"}");
    }
    
    return HTTPResponse("405 Method Not Allowed", "{\"success\":false,\"error_code\":-30000,\"error_msg\":\"http method not allowed\"}");
}