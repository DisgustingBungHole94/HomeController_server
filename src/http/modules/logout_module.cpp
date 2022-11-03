#include "logout_module.h"

#include "../../homecontroller.h"

#include <homecontroller/util/string.h>
#include <homecontroller/http/common_responses.h>

hc::http::http_response logout_module::execute(const std::string& method, const std::vector<std::string>& path, const std::string& body) {
    if (hc::util::str::to_lower_case(method) == "get") {
        try {
            m_controller->get_auth_manager()->delete_session(
                get_session_ptr()->get_id()
            );
        } catch(hc::exception& e) {
            return hc::http::common::server_error();
        }

        return hc::http::common::ok();
    }

    return hc::http::common::method_not_allowed();
}