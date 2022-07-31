#include "homecontroller.h"

#include <thread>

#include "util/exception/general_exception.h"
#include "util/string.h"
#include "util/config.h"
#include "util/timer.h"
#include "http/http_module_manager.h"

HomeController::HomeController() 
    : m_logger("Main"), m_status(HomeControllerStatus::STOPPED)
{}

HomeController::~HomeController() {}

bool HomeController::start() {
    m_logger.log("HomeController v2 by Josh Dittmer. Initializing...");

    HomeControllerConfig controllerConf;

    try {
        Config confFile;
        confFile.load("./conf/homecontroller.conf");

        controllerConf.m_serverPort = confFile["ServerPort"].getInt();
        controllerConf.m_tlsCertFile = confFile["TLSCertificateFile"].getString();
        controllerConf.m_tlsPrivKeyFile = confFile["TLSPrivateKeyFile"].getString();

        controllerConf.m_maxHTTPConnections = confFile["MaxHTTPConnections"].getInt();
        controllerConf.m_maxWSConnections = confFile["MaxWSConnections"].getInt();
        controllerConf.m_maxDevices = confFile["MaxDevices"].getInt();

        controllerConf.m_sessionExpireTime = confFile["SessionExpireTime"].getInt();

        controllerConf.m_logMode = confFile["LogMode"].getString();
        if (controllerConf.m_logMode == "debug") {
            Logger::debugEnabled(true);
            m_logger.dbg("Debug logging enabled.");
        } else if (controllerConf.m_logMode != "normal") {
            throw GeneralException("Unknown log mode specified.", "HomeController::init");
        }

        m_logger.log("Configuration file loaded.");
    } catch (GeneralException& e) {
        m_logger.csh("Failed to load configuration: " + e.what());
        return false;
    }

    try {
        // init device manager
        m_deviceManager.init();

        // init user manager
        m_userManager.init(&m_deviceManager);

        // init auth manager
        m_authManager.init(&m_userManager, controllerConf.m_sessionExpireTime);

        // init http modules
        HTTPModuleManager::init();

        // start http server
        m_httpServer.init(this, controllerConf.m_serverPort, controllerConf.m_tlsCertFile, controllerConf.m_tlsPrivKeyFile, controllerConf.m_maxHTTPConnections);

        // start ws server
        m_wsServer.init(this, controllerConf.m_maxWSConnections);

        // start device server
        m_deviceServer.init(this, controllerConf.m_maxDevices);
    } catch(GeneralException& e) {
        m_logger.csh("Failed to initialize: " + e.what() + " (" + e.func() + ")");
        return false;
    }

    m_status = HomeControllerStatus::RUNNING;

    // start http server thread
    std::thread httpsThread(&HTTPServer::run, &m_httpServer);

    m_logger.log("Initialization finished.");

    // start main loop, will exit on Ctrl-C
    loop();

    // wait for http thread to finish
    httpsThread.join();

    return true;
}

void HomeController::loop() {
    while(m_status == HomeControllerStatus::RUNNING) {

    }
}

void HomeController::shutdown() {
    if (m_status != HomeControllerStatus::RUNNING) {
        m_logger.err("WARNING: Early shutdown detected! Please wait until initialized.");
        return;
    }

    m_logger.log("Shutting down...");

    m_status = HomeControllerStatus::STOPPED;

    m_httpServer.stop();

    m_wsServer.stop();

    m_deviceServer.stop();
}

void HomeController::signalInterrupt(int s) {
    m_logger.dbg("Received interrupt from user.");
    shutdown();
}

void HomeController::signalPipe(int s) {
    m_logger.err("Received signal SIGPIPE!");
}

void HomeController::signalSegv(int s) {
    m_logger.csh("*** SEGMENTATION FAULT! ***");
    exit(-1);
}