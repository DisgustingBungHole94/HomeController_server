#pragma once

#include "general_exception.h"

class SocketTimeoutException : public hc::exception {
    public:
        SocketTimeoutException() {
            setErr("Socket timed out!");
            setFunc("TLSClient::*");
        }

        ~SocketTimeoutException() {}
};