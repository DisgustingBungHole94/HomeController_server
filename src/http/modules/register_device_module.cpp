#include "register_device_module.h"

#include "../../util/string.h"

RegisterDeviceModule::RegisterDeviceModule() {}
RegisterDeviceModule::~RegisterDeviceModule() {}

HTTPResponse RegisterDeviceModule::execute(const std::string& method, const std::vector<std::string>& path, const rapidjson::Document& jsonDoc) {
    if (Util::toLowerCase(method) == "post") {
        if (! (jsonDoc.HasMember("device_name") && jsonDoc["device_name"].IsString()
            && jsonDoc.HasMember("device_type") && jsonDoc["device_type"].IsString()))
        {
            return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-35000,\"error_msg\":\"missing required value or wrong type\"}");
        }

        std::string deviceName = jsonDoc["device_name"].GetString();
        std::string deviceType = jsonDoc["device_type"].GetString();

        Device device;

        try {
            device = m_controller->getDeviceManager()->registerDevice(deviceName, deviceType);
            getSession()->getUser()->addDevice(device);
        } catch(GeneralException& e) {
            return HTTPResponse("500 Internal Server Error", "{\"success\":false,\"error_code\":-70000,\"error_msg\":\"server error\"}");
        }

        return HTTPResponse("200 OK", "{\"success\":true,\"device_id\":\"" + device.getId() + "\"}");
    }

    return HTTPResponse("405 Method Not Allowed", "{\"success\":false,\"error_code\":-30000,\"error_msg\":\"http method not allowed\"}");
}