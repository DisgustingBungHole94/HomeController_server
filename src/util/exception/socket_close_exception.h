#pragma once

#include "general_exception.h"

class SocketCloseException : public hc::exception {
    public:
        SocketCloseException() {
            setErr("Socket closed!");
            setFunc("TLSClient::*");
        }

        ~SocketCloseException() {}
};