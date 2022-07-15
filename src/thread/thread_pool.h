#pragma once

#include "job.h"
#include "../util/logger.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <memory>
#include <map>

template<class JobType>
class ThreadPool {
    public:
        ThreadPool(const std::string& name);
        ~ThreadPool();

        void start(int numThreads);
        void addJob(std::unique_ptr<JobType> job);
        void stop();

        bool isBusy();
        int numActiveJobs();

        std::shared_ptr<JobType> getJob(std::thread::id id);

    private:
        void workerLoop();
        std::string threadIdToString(std::thread::id id);

        Logger m_logger;

        std::mutex m_mutex;
        std::condition_variable m_cv;

        std::vector<std::thread> m_threads;
        std::map<std::thread::id, std::shared_ptr<JobType>> m_jobs;

        std::queue<std::unique_ptr<JobType>> m_jobQueue;

        bool m_shouldTerminate;
};