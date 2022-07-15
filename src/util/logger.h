#pragma once

#include <string>

class Logger {
    public:
        Logger(const std::string& name);
        ~Logger();

        void log(const std::string msg, bool nl = true);
        void err(const std::string msg, bool nl = true);
        void csh(const std::string msg, bool nl = true);
        void dbg(const std::string msg, bool nl = true);

        static void debugEnabled(bool enabled) { _DEBUG_ENABLED = enabled; }

    private:
        static const std::string _MASTER_PREFIX;

        static const std::string _LOG_PREFIX;
        static const std::string _ERR_PREFIX;
        static const std::string _CSH_PREFIX;
        static const std::string _DBG_PREFIX;

        static bool _DEBUG_ENABLED;

        std::string m_name;
        std::string m_tabStr;

        std::string timestamp();

        void print(const std::string prefix, const std::string msg, bool nl, std::string color = "");
};