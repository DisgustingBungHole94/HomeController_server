#pragma once

#include "module.h"

class Module;

class LoginModule : public Module {
    public:
        LoginModule();
        ~LoginModule();

        HTTPResponse execute(const std::string& method, const std::vector<std::string>& path, const rapidjson::Document& jsonDoc);
};