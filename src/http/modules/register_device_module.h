#pragma once

#include "module.h"

class Module;

class RegisterDeviceModule : public Module {
    public:
        RegisterDeviceModule();
        ~RegisterDeviceModule();

        HTTPResponse execute(const std::string& method, const std::vector<std::string>& path, const rapidjson::Document& jsonDoc);
};