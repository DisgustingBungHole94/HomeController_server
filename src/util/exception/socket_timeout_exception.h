#pragma once

#include "general_exception.h"

class SocketTimeoutException : public GeneralException {
    public:
        SocketTimeoutException() {
            setErr("Socket timed out!");
            setFunc("TLSClient::*");
        }

        ~SocketTimeoutException() {}
};