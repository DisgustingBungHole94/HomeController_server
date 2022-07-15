#include "http_module_manager.h"

#include "modules/login_module.h"
#include "modules/logout_module.h"
#include "modules/register_device_module.h"
#include "modules/device_module.h"

std::unique_ptr<Logger> HTTPModuleManager::_logger = std::make_unique<Logger>("HTTPModuleManager");

std::map<std::string, ModuleInfo> HTTPModuleManager::_modules;

void HTTPModuleManager::init() {
    // register modules here

    _modules.insert(std::make_pair("login", ModuleInfo(
        []() -> Module* { return new LoginModule(); },
        "OPTIONS, POST",
        false
    )));

    _modules.insert(std::make_pair("logout", ModuleInfo(
        []() -> Module* { return new LogoutModule(); },
        "OPTIONS, GET",
        true
    )));

    _modules.insert(std::make_pair("register", ModuleInfo(
        []() -> Module* { return new RegisterDeviceModule(); },
        "OPTIONS, POST",
        true
    )));

    _modules.insert(std::make_pair("device", ModuleInfo(
        []() -> Module* { return new DeviceModule(); },
        "OPTIONS, POST",
        true
    )));

    ////

    _logger->log("Loaded HTTP modules: ", false);
    for(auto& x : _modules) {
        std::cout << x.first << " ";
    }
    std::cout << std::endl;
}

std::unique_ptr<Module> HTTPModuleManager::getModule(const std::string& name) {
    std::unique_ptr<Module> module;
    
    auto mit = _modules.find(name);
    if (mit != _modules.end()) {
        module.reset(mit->second.m_constructor());
        
        module->setAllowedMethods(mit->second.m_allowedMethods);
        module->setAuthRequired(mit->second.m_authRequired);

        return std::move(module);
    }

    return nullptr;
}