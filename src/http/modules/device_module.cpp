#include "device_module.h"

#include "../../device/device_handler_manager.h"

#include "../../util/string.h"
#include "../../util/json.h"

DeviceModule::DeviceModule() {}
DeviceModule::~DeviceModule() {}

HTTPResponse DeviceModule::execute(const std::string& method, const std::vector<std::string>& path, const rapidjson::Document& jsonDoc) {
    if (path.size() < 3) {
        return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-75000,\"error_msg\":\"path argument missing\"}");
    }

    if (Util::toLowerCase(method) == "post") {
        try {
            DevicePtr device = getSession()->getUser()->getDevice(path[2]);
            if (device == nullptr) {
                return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-80000,\"error_msg\":\"device not found\"}");
            }

            if (!device->connected()) {
                return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-85000,\"error_msg\":\"device not connected\"}");
            }

            std::string action;
            rapidjson::Document params;

            if (!Util::findJSONValue(jsonDoc, "action", action)) {
                return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-35000,\"error_msg\":\"missing required value or wrong type\"}");
            }

            if (!Util::findJSONValue(jsonDoc, "params", params)) {
                params.Parse("{}");
            }

            std::unique_ptr<Handler> handler = DeviceHandlerManager::getHandler(device->getType());
            if (handler == nullptr) {
                return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-90000,\"error_msg\":\"unsupported device type\"}");
            }

            return handler->handleAction(device, action, params);
        } catch(hc::exception& e) {
            return HTTPResponse("500 Internal Server Error", "{\"success\":false,\"error_code\":-70000,\"error_msg\":\"server error\"}");
        }
    }

    return HTTPResponse("405 Method Not Allowed", "{\"success\":false,\"error_code\":-30000,\"error_msg\":\"http method not allowed\"}");
}