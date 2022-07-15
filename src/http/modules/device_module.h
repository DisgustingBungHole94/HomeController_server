#pragma once

#include "module.h"

class Module;

class DeviceModule : public Module {
    public:
        DeviceModule();
        ~DeviceModule();

        HTTPResponse execute(const std::string& method, const std::vector<std::string>& path, const rapidjson::Document& jsonDoc);

    private:
        HTTPResponse handleLightStrip(DevicePtr& device, const rapidjson::Document& jsonDoc);

        uint8_t sendMessage(DevicePtr& device, const std::vector<uint8_t>& msg);
};