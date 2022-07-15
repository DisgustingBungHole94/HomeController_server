#include "tls_server.h"

#include <vector>

#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "../util/exception/socket_close_exception.h"
#include "../util/exception/socket_timeout_exception.h"

#include "../http/http_response.h"

namespace tls {
    std::string getSSLErrors() {
        tls::StringBIO bio;
        ERR_print_errors(bio.getBIO());
        return bio.getStr();
    }
}

TLSClient::TLSClient(int clientSocket, std::string ip, tls::UniquePtr<SSL> ssl)
    : m_clientSocket(clientSocket), m_ip(ip), m_ssl(std::move(ssl)), m_clientFinished(false)
{
    m_startTime = std::chrono::high_resolution_clock::now();
}

TLSClient::~TLSClient() {}

std::string TLSClient::recv() {
    const unsigned int MAX_BUF_LEN = 4096;
    std::vector<unsigned char> buf(MAX_BUF_LEN);

    std::string data;

    int numBytes = 0;
    do {
        numBytes = SSL_read(m_ssl.get(), &buf[0], buf.size());
        
        int res = SSL_get_error(m_ssl.get(), numBytes);
        if (res == SSL_ERROR_WANT_READ) {
            throw SocketTimeoutException();
        }

        if (numBytes < 0) {
            m_clientFinished = true;
            throw GeneralException("Failed to receive bytes.", "TLSClient::recv");
        } else if (numBytes == 0) {
            throw SocketCloseException();
        }

        buf.resize(numBytes);
        data.append(buf.cbegin(), buf.cend());
    } while(numBytes == MAX_BUF_LEN);

    return data;
}

void TLSClient::send(std::string data) {
    if (SSL_write(m_ssl.get(), data.c_str(), data.length()) < 0) {
        throw GeneralException("Failed to send bytes.", "TLSClient::send");
    }
}

void TLSClient::stop() {
    if (!m_clientFinished) {
        m_clientFinished = true;

        SSL_shutdown(m_ssl.get());
        shutdown(m_clientSocket, SHUT_RDWR);
        close(m_clientSocket);
    }
}

void TLSClient::setTimeout(int seconds) {
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = 0;
    setsockopt(SSL_get_fd(m_ssl.get()), SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
}

TLSServer::TLSServer()
    : m_logger("TLSServer"), m_port(0), m_serverSocket(-1), m_running(false)
{}

TLSServer::~TLSServer() {}

void TLSServer::init(int port, const std::string& certFile, const std::string& privKeyFile) {
    m_sslContext.reset(SSL_CTX_new(TLS_method()));
    SSL_CTX_set_min_proto_version(m_sslContext.get(), TLS1_2_VERSION);

    if (SSL_CTX_use_certificate_file(m_sslContext.get(), certFile.c_str(), SSL_FILETYPE_PEM) <= 0) {
        m_logger.err("OpenSSL error: " + tls::getSSLErrors());
        throw GeneralException("Failed to load server certificate file.", "TLSServer::init");
    }

    if (SSL_CTX_use_PrivateKey_file(m_sslContext.get(), privKeyFile.c_str(), SSL_FILETYPE_PEM) <= 0) {
        m_logger.err("OpenSSL error: " + tls::getSSLErrors());
        throw GeneralException("Failed to load server private key file.", "TLSServer::init");
    }

    m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverSocket < 0) {
        throw GeneralException("Failed to create TCP socket.", "TLSServer::init");
    }

    const int reuseAddr = 1;
    setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuseAddr, sizeof(int));

    struct sockaddr_in servAddr;
    std::memset(&servAddr, 0, sizeof(servAddr));

    m_port = port;

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(m_port);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(m_serverSocket, (struct sockaddr*)&servAddr, sizeof(servAddr)) < 0) {
        throw GeneralException("Failed to bind TCP socket, is the port already being used?", "TLSServer::init");
    }

    listen(m_serverSocket, 5);

    m_running = true;

    m_logger.log("TLS server open; listening on port " + std::to_string(m_port));
}

std::unique_ptr<TLSClient> TLSServer::accept() {
    if (!m_running) return nullptr;

    struct sockaddr_in clientAddr;
    int clientLen = sizeof(clientAddr);

    int clientSocket = ::accept(m_serverSocket, (struct sockaddr*)&clientAddr, (socklen_t*)&clientLen);
    if (m_running == false) {
        return nullptr;
    } else if (clientSocket < 0) {
        throw GeneralException("Failed to establish connection.", "TLSServer::acceptClient");
    }

    struct sockaddr_in* clientAddrPtr = (struct sockaddr_in*)&clientAddr;
    struct in_addr ipAddr = clientAddrPtr->sin_addr;
    char ipBuf[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &ipAddr, ipBuf, INET_ADDRSTRLEN);

    std::string ip(ipBuf);

    tls::UniquePtr<SSL> clientSSL(SSL_new(m_sslContext.get()));
    SSL_set_fd(clientSSL.get(), clientSocket);

    int res = SSL_accept(clientSSL.get());
    if (res <= 0) {
        m_logger.dbg("OpenSSL error: " + tls::getSSLErrors());
        m_logger.dbg("TLS handshake failed for client [" + ip + "].");

        close(clientSocket);
        return nullptr;
    }

    std::unique_ptr<TLSClient> client = std::make_unique<TLSClient>(clientSocket, ip, std::move(clientSSL));

    m_logger.dbg("Client [" + client->getIp() + "] connected, TLS handshake successful.");

    return std::move(client);
}

void TLSServer::stop() {
    m_running = false;

    shutdown(m_serverSocket, SHUT_RDWR);
    close(m_serverSocket);

    m_logger.log("TLS server closed.");
}