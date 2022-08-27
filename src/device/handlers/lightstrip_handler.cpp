#include "lightstrip_handler.h"

#include "../../device/device_session.h"
#include "../../util/json.h"

#include <vector>

LightStripHandler::LightStripHandler() {}
LightStripHandler::~LightStripHandler() {}

HTTPResponse LightStripHandler::handleAction(DevicePtr& device, const std::string& action, const rapidjson::Document& params) {
    std::vector<uint8_t> msg = { 0x00, 0x00, 0x00, 0x00 };

    if (action == "set_color") {
        int r, g, b;

        if (!Util::findJSONValue(params, "r", r) ||
            !Util::findJSONValue(params, "g", g) ||
            !Util::findJSONValue(params, "b", b))
        {
            return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-35000,\"error_msg\":\"missing required value or wrong type\"}");
        }

        if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255) {
            return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-95000,\"error_msg\":\"color values out of range\"}");
        }

        msg.push_back(0x01);
        msg.push_back(r);
        msg.push_back(g);
        msg.push_back(b);
    } else if (action == "set_speed") {
        int speed;

        if (!Util::findJSONValue(params, "speed", speed)) {
            return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-35000,\"error_msg\":\"missing required value or wrong type\"}");
        }

        if (speed < 25 || speed > 400) {
            return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-95000,\"error_msg\":\"speed out of range\"}");
        }

        msg.push_back(0x02);
        msg.push_back(((uint16_t)speed >> 0) & 0xFF);
        msg.push_back(((uint16_t)speed >> 8) & 0xFF);
    } else if (action == "start_program") {
        int id;

        if (!Util::findJSONValue(params, "id", id)) {
            return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-35000,\"error_msg\":\"missing required value or wrong type\"}");
        }

        msg.push_back(0x03);
        msg.push_back(id);
    } else if (action == "stop_program") {
        msg.push_back(0x04);
    } else {
        return HTTPResponse("400 Bad Request", "{\"success\":false,\"error_code\":-95000,\"error_msg\":\"unknown action\"}");
    }

    std::string deviceResponse = device->getConnection()->sendMessage(std::string(msg.begin(), msg.end()));

    if (deviceResponse.size() < 5) {
        return HTTPResponse("500 Internal Server Error", "{\"success\":false,\"error_code\":-70000,\"error_msg\":\"invalid response from device\"}");
    }

    return HTTPResponse("200 OK", "{\"success\":true,\"device_response\":" + std::to_string(deviceResponse[4]) + "}");
}