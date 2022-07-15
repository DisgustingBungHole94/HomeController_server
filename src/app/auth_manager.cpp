#include "auth_manager.h"

#include "../util/id.h"
#include "../util/exception/general_exception.h"

#include <algorithm>

AuthManager::AuthManager() {}
AuthManager::~AuthManager() {}

void AuthManager::init(UserManager* userManager, int sessionExpireTime) {
    m_userManager = userManager;
    m_sessionExpireTime = std::chrono::milliseconds(sessionExpireTime);
}

AuthManager::Error AuthManager::createSession(std::string& sessionIdRef, const std::string& username, const std::string& password, const std::string& ip) {
    std::lock_guard<std::mutex> lock(m_mutex);

    UserPtr user = m_userManager->getUser(username);
    if (user == nullptr) {
        return Error::USER_NOT_FOUND;
    }

    if (!user->checkPassword(password)) {
        return Error::INCORRECT_PASSWORD;
    }

    ID id;
    id.generate();

    std::string sessionId = id.str();

    SessionPtr session = std::make_shared<Session>(sessionId, ip, user);
    m_sessions.insert(std::make_pair(sessionId, session));

    sessionIdRef = sessionId;

    return Error::NONE;
}

bool AuthManager::deleteSession(const std::string& id) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto mit = m_sessions.find(id);
    if (mit == m_sessions.end()) {
        return false;
    }

    m_sessions.erase(mit);
    return true;
}

void AuthManager::deleteSessionsByUsername(const std::string& username) {
    std::lock_guard<std::mutex> lock(m_mutex);

    for (auto it = m_sessions.begin(); it != m_sessions.end(); ) {
        if (it->second->getUser()->getUsername() == username) {
            it = m_sessions.erase(it);
        } else {
            ++it;
        }
    }
}

SessionPtr AuthManager::getSession(const std::string& id) {
    std::lock_guard<std::mutex> lock(m_mutex);

    auto mit = m_sessions.find(id);
    if (mit == m_sessions.end()) {
        return nullptr;
    }

    /*std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - mit->second->getLastUse()) > m_sessionExpireTime) {
        mit->second->setExpired();
        m_sessions.erase(mit);

        return nullptr;
    } else {
        mit->second->updateLastUse();
    }*/

    mit->second->updateLastUse();

    return mit->second;
}