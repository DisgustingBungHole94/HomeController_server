#pragma once

#include "device.h"

#include <string>
#include <memory>
#include <map>
#include <mutex>

class User {
    public:
        User(const std::string& username, const std::string& passwordHash, int permissionLevel)
            : m_username(username), m_passwordHash(passwordHash), m_permissionLevel(permissionLevel)
        {}

        ~User() {}

        std::string getUsername() { return m_username; }
        bool checkPassword(const std::string& passwordHash) { return m_passwordHash == passwordHash; }

        int getPermissionLevel() { return m_permissionLevel; }

        void addDevice(const Device& device);
        void removeDevice(const std::string& device);

        DevicePtr getDevice(const std::string& id);

    private:
        std::string m_username;
        std::string m_passwordHash;

        int m_permissionLevel;

        std::map<const std::string, DevicePtr> m_devices;

        std::mutex m_mutex;
};

typedef std::shared_ptr<User> UserPtr;
typedef std::weak_ptr<User> UserWeakPtr;