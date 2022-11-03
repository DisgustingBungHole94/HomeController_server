#include "register_device_module.h"

#include "../../homecontroller.h"

#include <homecontroller/util/string.h>
#include <homecontroller/util/id.h>
#include <homecontroller/api/json/request/register_device_api_request.h>
#include <homecontroller/api/json/response/register_device_api_response.h>
#include <homecontroller/http/common_responses.h>

hc::http::http_response register_device_module::execute(const std::string& method, const std::vector<std::string>& path, const std::string& body) {
    if (hc::util::str::to_lower_case(method) == "post") {
        hc::api::json::register_device_api_request req;
        if (!req.parse(body)) {
            return hc::http::common::bad_json();
        }

        hc::util::id device_id;
        device_id.generate();

        hc::device device;
        device.set_id(device_id.str());
        device.set_name(req.get_device_name());
        device.set_type(req.get_device_type());

        try {
            get_session_ptr()->get_user()->add_device(device);
        } catch(hc::exception& e) {
            return hc::http::common::server_error();
        }

        hc::api::json::register_device_api_response res;
        res.set_device_id(device.get_id());

        return hc::http::http_response("200 OK", res.str());
    }

    return hc::http::common::method_not_allowed();
}