#include "homecontroller.h"

#include <homecontroller/exception/exception.h>
#include <homecontroller/util/config.h>

#include <thread>

bool homecontroller::start() {
    m_logger.log("homecontroller v2 by Josh Dittmer. initializing...");

    homecontroller_config controller_conf;

    try {
        hc::util::config conf_file;

        conf_file.load("./conf/homecontroller.conf");

        controller_conf.m_server_port = conf_file["ServerPort"].get_int();
        controller_conf.m_tls_cert_file = conf_file["TLSCertificateFile"].get_string();
        controller_conf.m_tls_priv_key_file = conf_file["TLSPrivateKeyFile"].get_string();

        controller_conf.m_connection_expire_time = conf_file["ConnectionExpireTime"].get_int();
        controller_conf.m_session_expire_time = conf_file["SessionExpireTime"].get_int();

        controller_conf.m_log_mode = conf_file["LogMode"].get_string();
        if (controller_conf.m_log_mode == "debug") {
            hc::util::logger::enable_debug();
            m_logger.dbg("debug logging enabled.");
        } else if (controller_conf.m_log_mode != "normal") {
            throw hc::exception("unknown log mode specified.", "HomeController::init");
        }

        m_logger.log("configuration file loaded");
    } catch (hc::exception& e) {
        m_logger.csh("failed to load configuration: " + std::string(e.what()));
        return false;
    }

    try {
        // start tls server
        m_server.init(controller_conf.m_server_port, controller_conf.m_tls_cert_file, controller_conf.m_tls_priv_key_file, controller_conf.m_connection_expire_time);
        m_server.start_threads();
    } catch(hc::exception& e) {
        m_logger.csh("failed to initialize: " + std::string(e.what()) + " (" + e.func() + ")");
        return false;
    }

    m_status = homecontroller_status::RUNNING;

    // start http server thread
    std::thread server_thread(&hc::net::ssl::tls_server::run, &m_server);

    m_logger.log("initialization finished");

    // start main loop, will exit on Ctrl-C
    loop();

    // wait for http thread to finish
    server_thread.join();

    return true;
}

void homecontroller::loop() {
    std::string current_cmd;

    while(m_status == homecontroller_status::RUNNING) {
        
    }
}

void homecontroller::shutdown() {
    if (m_status != homecontroller_status::RUNNING) {
        m_logger.err("early shutdown detected, please wait until initialized");
        return;
    }

    m_logger.log("shutting down...");

    m_status = homecontroller_status::STOPPED;

    m_server.stop();
    m_server.stop_threads();
}

void homecontroller::signal_interrupt(int s) {
    m_logger.dbg("received interrupt from user");
    shutdown();
}

void homecontroller::signal_pipe(int s) {
    m_logger.err("received signal SIGPIPE");
}

void homecontroller::signal_segv(int s) {
    m_logger.csh("*** SEGMENTATION FAULT! ***");
    m_logger.csh("homecontroller has encountered a critical error and must exit");
    exit(-1);
}