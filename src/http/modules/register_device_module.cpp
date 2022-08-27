#include "register_device_module.h"

#include "../../util/string.h"
#include "../../util/json.h"

RegisterDeviceModule::RegisterDeviceModule() {}
RegisterDeviceModule::~RegisterDeviceModule() {}

HTTPResponse RegisterDeviceModule::execute(const std::string& method, const std::vector<std::string>& path, const rapidjson::Document& jsonDoc) {
    if (Util::toLowerCase(method) == "post") {
        std::string deviceName;
        std::string deviceType;

        if (!Util::findJSONValue(jsonDoc, "device_name", deviceName) ||
            !Util::findJSONValue(jsonDoc, "device_type", deviceType))
        {
            return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-35000,\"error_msg\":\"missing required value or wrong type\"}");
        }

        Device device;

        try {
            device = m_controller->getDeviceManager()->registerDevice(deviceName, deviceType);
            getSession()->getUser()->addDevice(device);
        } catch(hc::exception& e) {
            return HTTPResponse("500 Internal Server Error", "{\"success\":false,\"error_code\":-70000,\"error_msg\":\"server error\"}");
        }

        return HTTPResponse("200 OK", "{\"success\":true,\"device_id\":\"" + device.getId() + "\"}");
    }

    return HTTPResponse("405 Method Not Allowed", "{\"success\":false,\"error_code\":-30000,\"error_msg\":\"http method not allowed\"}");
}