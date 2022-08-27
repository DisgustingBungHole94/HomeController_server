#pragma once

#include "../../http/http_response.h"
#include "../../app/device.h"

#include <string>
#include <rapidjson/document.h>

class Handler {
    public:
        Handler();
        virtual ~Handler();

        void setDeviceTitle(const std::string& title) { m_deviceTitle = title; }
        const std::string& getDeviceTitle() { return m_deviceTitle; }

        void setDeviceDescription(const std::string& description) { m_deviceDescription = description; }
        const std::string& getDeviceDescription() { return m_deviceDescription; }

        virtual HTTPResponse handleAction(DevicePtr& device, const std::string& action, const rapidjson::Document& params);
    protected:
        std::string m_deviceTitle;
        std::string m_deviceDescription;
};