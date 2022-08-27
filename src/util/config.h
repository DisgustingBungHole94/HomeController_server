#pragma once

#include <string>
#include <map>

#include <homecontroller/exception/exception.h>

class ConfigValue {
    public:
        ConfigValue(const std::string& name) 
            : m_name(name), m_str(""), m_int(0), m_isString(false), m_isInt(false)
        {}

        ~ConfigValue() {}

        void setString(const std::string& val) {
            m_str = val;
            m_isString = true;
        }

        void setInt(int val) {
            m_int = val;
            m_isInt = true;
        }

        const std::string& getString() {
            if (!m_isString) {
                throw hc::exception("Variable " + m_name + " is not a string.", "ConfigValue::getString");
            }

            return m_str;
        }

        const int getInt() {
            if (!m_isInt) {
                throw hc::exception("Variable " + m_name + " is not an integer.", "ConfigValue::getInt");
            }

            return m_int;
        }

        bool isString() { return m_isString; }
        bool isInt() { return m_isInt; }

    private:
        std::string m_name;

        std::string m_str;
        int m_int;

        bool m_isString;
        bool m_isInt;
};

class Config {
    public:
        Config();
        ~Config();

        void load(const std::string& path);

        ConfigValue& operator[](const std::string& var);

    private:
        std::map<std::string, ConfigValue> m_vars;
};