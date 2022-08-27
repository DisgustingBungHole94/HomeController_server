#pragma once

#include <string>

class hc::exception {
    public:
        hc::exception() 
            : hc::exception("", "")
        {}

        hc::exception(std::string err, std::string func)
            : m_err(err), m_func(func)
        {}

        ~hc::exception() {}

        std::string what() { return m_err; }
        std::string func() { return m_func; }

        void setErr(std::string err) { m_err = err; }
        void setFunc(std::string func) { m_func = func; }

    protected:
        std::string m_err;
        std::string m_func;
};