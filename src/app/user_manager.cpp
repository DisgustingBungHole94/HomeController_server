#include "user_manager.h"

#include "../util/exception/general_exception.h"

UserManager::UserManager() {}
UserManager::~UserManager() {}

void UserManager::init(DeviceManager* deviceManager) {
    m_deviceManager = deviceManager;

    UserPtr user = std::make_shared<User>("Test", "1234", 1);
    user->addDevice(m_deviceManager->getDevice("baaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"));

    m_users.insert(std::make_pair(user->getUsername(), user));
}

void UserManager::createUser(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(m_mutex);
}

void UserManager::deleteUser(const std::string& username) {
    std::lock_guard<std::mutex> lock(m_mutex);
}

UserPtr UserManager::getUser(const std::string& username) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto mit = m_users.find(username);
    if (mit != m_users.end()) {
        return mit->second;
    }

    return nullptr;
}