#pragma once

#include "modules/module.h"

#include "../util/logger.h"

#include <map>
#include <string>
#include <memory>
#include <functional>

class HomeController;

struct ModuleInfo {
    ModuleInfo(std::function<Module*()> constructor, std::string allowedMethods, bool authRequired)
        : m_constructor(constructor), m_allowedMethods(allowedMethods), m_authRequired(authRequired)
    {}

    std::function<Module*()> m_constructor;

    std::string m_allowedMethods;
    bool m_authRequired;
};

class HTTPModuleManager {
    public:
        static void init();
        static std::unique_ptr<Module> getModule(const std::string& name);
    private:
        static std::unique_ptr<Logger> _logger;

        static std::map<std::string, ModuleInfo> _modules;
};