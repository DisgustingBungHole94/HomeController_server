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

        controller_conf.m_server_port = conf_file["ServerPort"].getInt();
        controller_conf.m_tls_cert_file = conf_file["TLSCertificateFile"].getString();
        controller_conf.m_tls_priv_key_file = conf_file["TLSPrivateKeyFile"].getString();

        controller_conf.m_connection_expire_time = conf_file["ConnectionExpireTime"].getInt();
        controller_conf.m_session_expire_time = conf_file["SessionExpireTime"].getInt();

        controller_conf.m_log_mode = conf_file["LogMode"].getString();
        if (controller_conf.m_log_mode == "debug") {
            hc::util::logger::enableDebug();
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
        m_server.set_connect_callback(std::bind(&homecontroller::on_connect, this, std::placeholders::_1));
        m_server.set_data_callback(std::bind(&homecontroller::on_ready, this, std::placeholders::_1));
        m_server.set_disconnect_callback(std::bind(&homecontroller::on_disconnect, this, std::placeholders::_1));

        m_server.init(controller_conf.m_server_port, controller_conf.m_tls_cert_file, controller_conf.m_tls_priv_key_file, controller_conf.m_connection_expire_time);
        
        int num_threads = std::thread::hardware_concurrency();
        m_thread_pool.start((num_threads <= 0) ? 4 : num_threads);
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

    m_thread_pool.stop();
}

void homecontroller::on_connect(hc::net::ssl::tls_server::connection_hdl hdl) {
    std::unique_ptr<session> s = std::make_unique<session>();
    
    m_sessions.insert(std::make_pair(hdl, std::move(s)));
}

void homecontroller::on_ready(hc::net::ssl::tls_server::connection_hdl hdl) {
    m_thread_pool.addJob(std::bind(&homecontroller::run_session, this, hdl));
}

void homecontroller::on_disconnect(hc::net::ssl::tls_server::connection_hdl hdl) {
    auto mit = m_sessions.find(hdl);
    if (mit == m_sessions.end()) {
        m_logger.err("connection is not associated with a session!");
        return;
    }

    m_sessions.erase(mit);
}

void homecontroller::run_session(hc::net::ssl::tls_server::connection_hdl hdl) {
    auto mit = m_sessions.find(hdl);
    if (mit == m_sessions.end()) {
        m_logger.err("connection is not associated with a session!");
        return;
    }

    try {
        auto conn = hc::net::ssl::tls_server::conn_from_hdl(hdl);

        if (!mit->second->onReady(conn)) {
            conn->close();
        }
    } catch(hc::exception& e) {
        m_logger.err("client error: " + std::string(e.what()) + " (" + std::string(e.func()) + ")");
    }
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