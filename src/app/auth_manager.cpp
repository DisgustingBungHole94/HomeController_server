#include "auth_manager.h"

#include "../homecontroller.h"

#include <homecontroller/util/logger.h>
#include <homecontroller/exception/exception.h>
#include <homecontroller/util/id.h>

#include <algorithm>

auth_result auth_manager::create_session(const std::string& username, const std::string& password, const std::string& ip, std::string& session_id_ref) {
    std::lock_guard<std::mutex> lock(m_mutex);

    std::shared_ptr<hc::user> user_ptr = m_controller->get_user_manager()->get_user(username);
    if (user_ptr == nullptr) {
        return auth_result::NO_USER;
    }

    if (user_ptr->get_password() != password) {
        return auth_result::INC_PASS;
    }

    hc::util::id session_id;
    session_id.generate();

    std::shared_ptr<hc::session> session_ptr = std::make_shared<hc::session>(session_id.str(), ip, user_ptr);
    m_sessions.insert(std::make_pair(session_ptr->get_id(), session_ptr));

    hc::util::logger::dbg("session created: " + session_ptr->get_id());

    session_id_ref = session_ptr->get_id();
    return auth_result::OK;
}

bool auth_manager::delete_session(const std::string& id) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto mit = m_sessions.find(id);
    if (mit == m_sessions.end()) {
        return false;
    }

    m_sessions.erase(mit);

    hc::util::logger::dbg("session deleted: " + id);

    return true;
}

void auth_manager::delete_sessions_by_username(const std::string& username) {
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto mit = m_sessions.begin(); mit != m_sessions.end(); ) {
        std::shared_ptr<hc::user> user_ptr = mit->second->get_user();
        if (user_ptr != nullptr && user_ptr->get_username() == username) {
            hc::util::logger::dbg("session deleted: " + mit->first);
            mit = m_sessions.erase(mit);
        } else {
            ++mit;
        }
    }
}

std::shared_ptr<hc::session> auth_manager::get_session(const std::string& id) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto mit = m_sessions.find(id);
    if (mit == m_sessions.end()) {
        return nullptr;
    }

    return mit->second;
}