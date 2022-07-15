#pragma once

#include <string>
#include <memory>

#include "../util/exception/general_exception.h"

class DeviceSession;

class Device {
    public:
        enum class Type {
            LIGHT_STRIP
        };

        Device() {}

        Device(const std::string& id, const std::string& name, const std::string& type)
            : m_id(id), m_name(name), m_type(type), m_connected(false)
        {}

        ~Device() {}

        std::string getId() { return m_id; }
        std::string getName() { return m_name; }
        std::string getType() { return m_type; }

        std::shared_ptr<DeviceSession> getConnection() {
            if (m_connection.expired()) {
                throw GeneralException("Connection deleted.", "Device::getConnection");
            }

            return m_connection.lock();
        }

        void connect(std::shared_ptr<DeviceSession> connection) {
            m_connection = connection;
            m_connected = true;
        }

        void disconnect() {
            m_connection.reset();
            m_connected = false;
        }

        bool connected() { return m_connected; }

    private:
        std::string m_id;

        std::string m_name;
        std::string m_type;

        std::weak_ptr<DeviceSession> m_connection;
        bool m_connected;
};

typedef std::shared_ptr<Device> DevicePtr;
typedef std::weak_ptr<Device> DeviceWeakPtr;