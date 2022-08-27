#include "handler.h"

Handler::Handler() {}
Handler::~Handler() {}

HTTPResponse Handler::handleAction(DevicePtr& device, const std::string& action, const rapidjson::Document& params) {
    return HTTPResponse();
}