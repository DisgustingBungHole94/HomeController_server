#include "user_manager.h"

void user_manager::load_users() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
}

void user_manager::create_user(const hc::user& user) {
    std::lock_guard<std::mutex> lock(m_mutex);

}

void user_manager::delete_user(const std::string& username) {
    std::lock_guard<std::mutex> lock(m_mutex);

}

std::shared_ptr<hc::user> user_manager::get_user(const std::string& username) {
    std::lock_guard<std::mutex> lock(m_mutex);

}