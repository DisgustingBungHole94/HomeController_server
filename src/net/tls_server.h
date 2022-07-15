#pragma once

#include "../util/exception/general_exception.h"
#include "../util/logger.h"

#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <mutex>

#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>

namespace tls {
    template<class T> struct DeleterOf;
    template<> struct DeleterOf<SSL_CTX> { void operator()(SSL_CTX* p) const { SSL_CTX_free(p); } };
    template<> struct DeleterOf<SSL> { void operator()(SSL* p) const { SSL_free(p); } };
    template<> struct DeleterOf<BIO> { void operator()(BIO* p) const { BIO_free_all(p); } };
    template<> struct DeleterOf<BIO_METHOD> { void operator()(BIO_METHOD* p) const { BIO_meth_free(p); } };

    template<class OpenSSLType>
    using UniquePtr = std::unique_ptr<OpenSSLType, tls::DeleterOf<OpenSSLType>>;

    class StringBIO {
        public:
            StringBIO(StringBIO&&) = delete;
            StringBIO& operator=(StringBIO&&) = delete;

            explicit StringBIO() {
                m_methods.reset(BIO_meth_new(BIO_TYPE_SOURCE_SINK, "StringBIO"));
                if (m_methods == nullptr) {
                    throw GeneralException("Failed to create methods for StringBIO", "StringBIO::StringBIO");
                }

                BIO_meth_set_write(m_methods.get(), [](BIO* bio, const char* data, int len) -> int {
                    std::string* str = reinterpret_cast<std::string*>(BIO_get_data(bio));
                    str->append(data, len);
                    return len;
                });

                m_bio.reset(BIO_new(m_methods.get()));
                if (m_bio == nullptr) {
                    throw GeneralException("Failed to create BIO for StringBIO", "StringBIO::StringBIO");
                }

                BIO_set_data(m_bio.get(), &m_str);
                BIO_set_init(m_bio.get(), 1);
            }

            BIO* getBIO() { return m_bio.get(); }
            std::string getStr() { return m_str; }
        private:
            std::string m_str;

            tls::UniquePtr<BIO_METHOD> m_methods;
            tls::UniquePtr<BIO> m_bio;
    };

    extern std::string getSSLErrors();
};

class TLSClient {
    public:
        explicit TLSClient(int clientSocket, std::string ip, tls::UniquePtr<SSL> ssl);
        
        TLSClient(TLSClient&) = delete;
        TLSClient& operator=(const TLSClient&) = delete;

        ~TLSClient();

        std::string recv();
        void send(std::string data);

        void stop();

        void setTimeout(int seconds);

        std::string getIp() { return m_ip; }
        std::chrono::high_resolution_clock::time_point getStartTime() { return m_startTime; }

    private:
        int m_clientSocket;
        std::string m_ip;

        tls::UniquePtr<SSL> m_ssl;

        std::chrono::high_resolution_clock::time_point m_startTime;

        bool m_clientFinished;
};

class TLSServer {
    public:
        TLSServer();
        ~TLSServer();

        void init(int port, const std::string& certFile, const std::string& privKeyFile);
        std::unique_ptr<TLSClient> accept();

        void stop();

    private:
        Logger m_logger;

        int m_port;
        int m_serverSocket;

        tls::UniquePtr<SSL_CTX> m_sslContext;

        bool m_running;
};