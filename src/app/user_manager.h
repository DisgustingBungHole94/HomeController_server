#pragma once

#include <string>
#include <map>

#include "user.h"
#include "device_manager.h"

class UserManager {
    public:
        UserManager();
        ~UserManager();

        void init(DeviceManager* deviceManager);

        void createUser(const std::string& username, const std::string& password);
        void deleteUser(const std::string& username);

        UserPtr getUser(const std::string& username);

    private:
        DeviceManager* m_deviceManager;

        std::map<std::string, UserPtr> m_users;

        std::mutex m_mutex;
};