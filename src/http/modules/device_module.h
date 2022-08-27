#pragma once

#include "module.h"

class Module;

class DeviceModule : public Module {
    public:
        DeviceModule();
        ~DeviceModule();

        HTTPResponse execute(const std::string& method, const std::vector<std::string>& path, const rapidjson::Document& jsonDoc);
};