#include "login_module.h"

#include "../../homecontroller.h"

#include <homecontroller/util/string.h>
#include <homecontroller/api/json/request/login_api_request.h>
#include <homecontroller/api/json/response/login_api_response.h>
#include <homecontroller/http/common_responses.h>

hc::http::http_response login_module::execute(const std::string& method, const std::vector<std::string>& path, const std::string& body) {
    if (hc::util::str::to_lower_case(method) == "post") {
        hc::api::json::login_api_request req;
        if (!req.parse(body)) {
            return hc::http::common::bad_json();
        }

        std::string session_id;

        auth_result result = 
            m_controller->get_auth_manager()->create_session(
                req.get_username(), 
                req.get_password(), 
                "",
                session_id
            );

        switch(result) {
            case auth_result::NO_USER:
                return hc::http::common::user_not_found();
                break;
            case auth_result::INC_PASS:
                return hc::http::common::incorrect_password();
                break;
        }

        hc::api::json::login_api_response res;
        res.set_token(session_id);

        return hc::http::http_response("200 OK", res.str());
    }

    return hc::http::common::method_not_allowed();
}