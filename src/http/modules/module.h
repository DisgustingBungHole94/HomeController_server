#pragma once

#include "../http_response.h"
#include "../../homecontroller.h"
#include "../../app/auth_manager.h"

#include <rapidjson/document.h>

#include <vector>
#include <string>
#include <memory>

class HomeController;

class Module {
    public:
        Module();
        virtual ~Module();

        void setController(HomeController* controller) { m_controller = controller; }
        void setSession(SessionPtr session) { m_session = session; }

        void setIp(const std::string& ip) { m_ip = ip; }

        void setAllowedMethods(const std::string& allowedMethods) { m_allowedMethods = allowedMethods; }
        const std::string& getAllowedMethods() { return m_allowedMethods; }

        void setAuthRequired(bool authRequired) { m_authRequired = authRequired; }
        bool authRequired() { return m_authRequired; }

        virtual HTTPResponse execute(const std::string& method, const std::vector<std::string>& path, const rapidjson::Document& jsonDoc);
    protected:
        SessionPtr getSession();

        HomeController* m_controller = nullptr;
        SessionWeakPtr m_session;

        std::string m_ip;

        bool m_authRequired;

        std::string m_allowedMethods;
};