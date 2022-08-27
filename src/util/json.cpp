#include "json.h"

#include "exception/general_exception.h"

namespace Util {
    template<typename T>
    bool findJSONValue(const rapidjson::Document& jsonDoc, const std::string& name, T& valueRef) {
        throw hc::exception("Unsupported JSON value type!", "Util::findJSONValue");
        return false;
    }

    template<>
    bool findJSONValue<std::string>(const rapidjson::Document& jsonDoc, const std::string& name, std::string& valueRef) {
        rapidjson::Value::ConstMemberIterator itr = jsonDoc.FindMember(name.c_str());

        if (itr == jsonDoc.MemberEnd()) {
            return false;
        } else if (!itr->value.IsString()) {
            return false;
        }

        valueRef = itr->value.GetString();
        return true;
    }

    template<>
    bool findJSONValue<int>(const rapidjson::Document& jsonDoc, const std::string& name, int& valueRef) {
        rapidjson::Value::ConstMemberIterator itr = jsonDoc.FindMember(name.c_str());

        if (itr == jsonDoc.MemberEnd()) {
            return false;
        } else if (!itr->value.IsInt()) {
            return false;
        }

        valueRef = itr->value.GetInt();
        return true;
    }

    template<>
    bool findJSONValue<double>(const rapidjson::Document& jsonDoc, const std::string& name, double& valueRef) {
        rapidjson::Value::ConstMemberIterator itr = jsonDoc.FindMember(name.c_str());

        if (itr == jsonDoc.MemberEnd()) {
            return false;
        } else if (!itr->value.IsDouble()) {
            return false;
        }

        valueRef = itr->value.GetDouble();
        return true;
    }

    template<>
    bool findJSONValue<rapidjson::Document>(const rapidjson::Document& jsonDoc, const std::string& name, rapidjson::Document& valueRef) {
        rapidjson::Document doc;
        doc.CopyFrom(jsonDoc, doc.GetAllocator());

        rapidjson::Value::MemberIterator itr = doc.FindMember(name.c_str());

        if (itr == jsonDoc.MemberEnd()) {
            return false;
        } else if (!itr->value.IsObject()) {
            return false;
        }

        valueRef.Swap(itr->value);
        return true;
    }
}