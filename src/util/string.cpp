#include "string.h"

#include <algorithm>
#include <cctype>

namespace Util {
    std::vector<std::string> splitString(std::string str, char delim) {
        std::size_t pos = 0;
        std::vector<std::string> tokens;

        while((pos = str.find(delim)) != std::string::npos) {
            tokens.push_back(str.substr(0, pos));
            str.erase(0, pos + 1);
        }
        tokens.push_back(str);

        return tokens;
    }

    std::string toLowerCase(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
            return std::tolower(c);
        });

        return str;
    }

    std::vector<uint8_t> stringToBytes(const std::string& str) {
        std::vector<uint8_t> bytes;
        bytes.reserve(str.size());

        std::transform(std::begin(str), std::end(str), std::back_inserter(bytes), [](char c) {
            return (uint8_t)c;
        });

        return bytes;
    }
};