#pragma once

#include <thread>

class Job {
    public:
        Job();
        ~Job();

        void run();
        void stop();

        template<class JobType>
        friend class ThreadPool;

    protected:
        std::thread::id m_id;
};