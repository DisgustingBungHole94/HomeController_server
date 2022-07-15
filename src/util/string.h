#pragma once

#include <string>
#include <vector>

namespace Util {
    extern std::vector<std::string> splitString(std::string str, char delim);
    extern std::string toLowerCase(std::string str);
    extern std::vector<uint8_t> stringToBytes(const std::string& str);
};