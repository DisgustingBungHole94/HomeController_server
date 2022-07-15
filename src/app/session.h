#pragma once

#include "user.h"
#include "../util/exception/general_exception.h"

#include <string>
#include <memory>
#include <chrono>

class Session {
    public:
        Session() {}

        Session(const std::string& id, const std::string& ip, UserWeakPtr user)
            : m_id(id), m_ip(ip), m_user(user)
        {
            updateLastUse();
        }

        ~Session() {}

        const std::string getId() { return m_id; }

        std::string getIp() { return m_ip; }

        void updateLastUse() { m_lastUse = std::chrono::high_resolution_clock::now(); }
        std::chrono::time_point<std::chrono::high_resolution_clock> getLastUse() { return m_lastUse; }

        UserPtr getUser() {
            if (m_user.expired()) {
                throw GeneralException("User deleted.", "Session::getUser");
            }

            return m_user.lock();
        }
    private:
        const std::string m_id;

        std::string m_ip;
        std::chrono::time_point<std::chrono::high_resolution_clock> m_lastUse;

        UserWeakPtr m_user;
};

typedef std::shared_ptr<Session> SessionPtr;
typedef std::weak_ptr<Session> SessionWeakPtr;