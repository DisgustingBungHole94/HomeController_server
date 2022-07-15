#pragma once

#include "module.h"

class Module;

class LogoutModule : public Module {
    public:
        LogoutModule();
        ~LogoutModule();

        HTTPResponse execute(const std::string& method, const std::vector<std::string>& path, const rapidjson::Document& jsonDoc);
};