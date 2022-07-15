#include "module.h"

Module::Module() {}
Module::~Module() {}

HTTPResponse Module::execute(const std::string& method, const std::vector<std::string>& path, const rapidjson::Document& jsonDoc) {
    return HTTPResponse();
}

SessionPtr Module::getSession() {
    if (SessionPtr sptr = m_session.lock()) {
        return sptr;
    }

    return nullptr;
}