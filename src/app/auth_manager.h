#pragma once

#include "user_manager.h"
#include "session.h"

#include "../util/exception/general_exception.h"

#include <string>
#include <map>
#include <mutex>
#include <chrono>
#include <memory>

class AuthManager {
    public:
        enum class Error {
            NONE, USER_NOT_FOUND, INCORRECT_PASSWORD
        };

        AuthManager();
        ~AuthManager();

        void init(UserManager* userManager, int sessionExpireTime);

        Error createSession(std::string& sessionIdRef, const std::string& username, const std::string& password, const std::string& ip);

        bool deleteSession(const std::string& id);
        void deleteSessionsByUsername(const std::string& username);

        SessionPtr getSession(const std::string& id);

    private:
        UserManager* m_userManager;

        std::map<const std::string, SessionPtr> m_sessions;
        std::chrono::milliseconds m_sessionExpireTime;

        std::mutex m_mutex;
};