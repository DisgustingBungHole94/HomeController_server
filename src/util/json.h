#pragma once

#include <rapidjson/document.h>
#include <string>

namespace Util {
    template<typename T>
    extern bool findJSONValue(const rapidjson::Document& jsonDoc, const std::string& name, T& valueRef);

    extern bool findJSONObject(rapidjson::Document& jsonDoc, const std::string& name, rapidjson::Document& objRef);
};