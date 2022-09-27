#include "auth_manager.h"

#include "../homecontroller.h"

#include <homecontroller/exception/exception.h>

std::string auth_manager::create_session(const std::string& username, const std::string& password, const std::string& ip) {
    std::lock_guard<std::mutex> lock(m_mutex);

}

void auth_manager::delete_session(const std::string& id) {
    std::lock_guard<std::mutex> lock(m_mutex);


}

void auth_manager::delete_sessions_by_username(const std::string& username) {
    std::lock_guard<std::mutex> lock(m_mutex);

    
}