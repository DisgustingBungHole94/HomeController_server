#pragma once

#include <chrono>

class Timer {
    public:
        Timer(std::chrono::milliseconds duration);
        ~Timer();

        void reset();

        bool isFinished();

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_startTime;

        std::chrono::milliseconds m_duration;
};