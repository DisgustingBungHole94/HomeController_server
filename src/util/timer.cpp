#include "timer.h"

Timer::Timer(int duration)
    : m_duration(duration)
{
    reset();
}

Timer::~Timer() {}

void Timer::reset() {
    m_startTime = std::chrono::high_resolution_clock::now();
}

bool Timer::isFinished() {
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();;

    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime) > m_duration) {
        return true;
    }

    return false;
}