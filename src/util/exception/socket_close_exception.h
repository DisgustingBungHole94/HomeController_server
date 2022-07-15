#pragma once

#include "general_exception.h"

class SocketCloseException : public GeneralException {
    public:
        SocketCloseException() {
            setErr("Socket closed!");
            setFunc("TLSClient::*");
        }

        ~SocketCloseException() {}
};