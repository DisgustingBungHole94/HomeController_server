#include "module.h"

Module::Module() {}
Module::~Module() {}

HTTPResponse Module::execute(const std::string& method, const std::vector<std::string>& path, const rapidjson::Document& jsonDoc) {
    return HTTPResponse();
}

SessionPtr Module::getSession() {
    if (m_session.expired()) {
        throw hc::exception("Session deleted.", "Module::getSession");
    }

    return m_session.lock();
}