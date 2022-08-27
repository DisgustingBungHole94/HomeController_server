#pragma once

#include "handler.h"

class LightStripHandler : public Handler {
    public:
        LightStripHandler();
        ~LightStripHandler();

        HTTPResponse handleAction(DevicePtr& device, const std::string& action, const rapidjson::Document& params);
};