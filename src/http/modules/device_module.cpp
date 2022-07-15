#include "device_module.h"

#include "../../util/string.h"

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

            if (! (jsonDoc.HasMember("action") && jsonDoc["action"].IsString()
                && jsonDoc.HasMember("params") && jsonDoc["params"].IsObject())) {
                return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-35000,\"error_msg\":\"missing required value or wrong type\"}");
            }

            if (device->getType() == "light_strip") {
                return handleLightStrip(device, jsonDoc);
            } else {
                return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-85000,\"error_msg\":\"device not connected\"}");
            }
        } catch(GeneralException& e) {
            return HTTPResponse("500 Internal Server Error", "{\"success\":false,\"error_code\":-70000,\"error_msg\":\"server error\"}");
        }
    }

    return HTTPResponse("405 Method Not Allowed", "{\"success\":false,\"error_code\":-30000,\"error_msg\":\"http method not allowed\"}");
}

HTTPResponse DeviceModule::handleLightStrip(DevicePtr& device, const rapidjson::Document& jsonDoc) {
    std::string action = jsonDoc["action"].GetString();
    auto params = jsonDoc["params"].GetObject();

    std::vector<uint8_t> msg = { 0x00, 0x00, 0x00, 0x00 };

    if (action == "set_color") {
        if (! (params.HasMember("r") && params["r"].IsInt()
            && params.HasMember("g") && params["g"].IsInt()
            && params.HasMember("b") && params["b"].IsInt()))
        {
            return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-35000,\"error_msg\":\"missing required value or wrong type\"}");
        }

        int r = params["r"].GetInt();
        int g = params["g"].GetInt();
        int b = params["b"].GetInt();

        if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
            return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-95000,\"error_msg\":\"color values out of range\"}");
        }

        msg.push_back(0x01);
        msg.push_back(r);
        msg.push_back(g);
        msg.push_back(b);
    } else if (action == "set_speed") {
        if (!(params.HasMember("speed") && params["speed"].IsInt())) {
            return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-35000,\"error_msg\":\"missing required value or wrong type\"}");
        }

        int speed = params["speed"].GetInt();

        if (speed < 25 || speed > 400) {
            return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-95000,\"error_msg\":\"speed out of range\"}");
        }

        msg.push_back(0x02);
        msg.push_back(((uint16_t)speed >> 0) & 0xFF);
        msg.push_back(((uint16_t)speed >> 8) & 0xFF);
    } else if(action == "start_program") {
        if (!(params.HasMember("id") && params["id"].IsInt())) {
            return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-35000,\"error_msg\":\"missing required value or wrong type\"}");
        }

        int id = params["id"].GetInt();

        msg.push_back(0x03);
        msg.push_back(id);
    } else if (action == "stop_program") {
        msg.push_back(0x04);  
    } else {
        return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-95000,\"error_msg\":\"unknown action\"}");
    }

    uint8_t res = sendMessage(device, msg);

    return HTTPResponse("200 OK", "{\"success\":true,\"device_response\":" + std::to_string(res) + "}");
}

uint8_t DeviceModule::sendMessage(DevicePtr& device, const std::vector<uint8_t>& msg) {
    std::string deviceRes = device->getConnection()->sendMessage(std::string(msg.begin(), msg.end()));
    
    if (deviceRes.size() < 5) {
        throw GeneralException("Invalid response from device.", "DeviceModule::sendMessage");
    }

    return deviceRes[4];
}