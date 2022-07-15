#include "logger.h"

#include <iostream>

#include <ctime>
#include <sstream>
#include <iomanip>

const std::string Logger::_MASTER_PREFIX = "[HomeController]";

const std::string Logger::_LOG_PREFIX = "[LOG]";
const std::string Logger::_ERR_PREFIX = "[ERROR]";
const std::string Logger::_CSH_PREFIX = "[FATAL]";
const std::string Logger::_DBG_PREFIX = "[DEBUG]";

bool Logger::_DEBUG_ENABLED = false;

Logger::Logger(const std::string& name) 
    : m_name(name)
{
    if (m_name.length() + 2 >= 16) {
        m_tabStr = "\t";
    } else if (m_name.length() + 2 >= 8) {
        m_tabStr = "\t\t";
    } else {
        m_tabStr = "\t\t\t";
    }
}

Logger::~Logger() {}

void Logger::log(const std::string msg, bool nl) {
    print(_LOG_PREFIX, msg, nl);
}

void Logger::err(const std::string msg, bool nl) {
    print(_ERR_PREFIX, msg, nl, "\033[1;33m");
}

void Logger::csh(const std::string msg, bool nl) {
    print(_CSH_PREFIX, msg, nl, "\033[1;31m");
}

void Logger::dbg(const std::string msg, bool nl) {
    if (_DEBUG_ENABLED) {
        print(_DBG_PREFIX, msg, nl, "\033[0;36m");
    }
}

std::string Logger::timestamp() {
    auto t = std::time(nullptr);

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&t), "%m-%d-%y %H:%M:%S");

    return oss.str();
}

void Logger::print(const std::string prefix, const std::string msg, bool nl, std::string color) {
    std::ostringstream ss;
    ss << _MASTER_PREFIX << " " << timestamp() << "\t[" << m_name << "]" << m_tabStr << prefix << "\t\t" << msg;
    if (nl) ss << "\n";

    std::cout << color + ss.str() + "\033[0m";
}
