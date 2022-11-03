#include "user_manager.h"

#include "../homecontroller.h"

#include <homecontroller/exception/exception.h>

void user_manager::load_users() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // TODO: load users from MongoDB database

    std::shared_ptr<hc::user> user_ptr = std::make_shared<hc::user>("Test", "1234", 0);

    hc::device device("test", "Light Strip", "lightstrip");
    user_ptr->add_device(device);

    m_users.insert(std::make_pair(user_ptr->get_username(), user_ptr));
}

void user_manager::create_user(const hc::user& user) {
    std::lock_guard<std::mutex> lock(m_mutex);

    // TODO: add user to database

    std::shared_ptr<hc::user> user_ptr = std::make_shared<hc::user>(user);
    m_users.insert(std::make_pair(user_ptr->get_username(), user_ptr));
}

void user_manager::delete_user(const std::string& username) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto mit = m_users.find(username);
    if (mit == m_users.end()) {
        throw hc::exception("user does not exist", "user_manager::delete_user");
    }

    m_users.erase(mit);
}

std::shared_ptr<hc::user> user_manager::get_user(const std::string& username) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto mit = m_users.find(username);
    if (mit == m_users.end()) {
        return nullptr;
    }

    return mit->second;
}