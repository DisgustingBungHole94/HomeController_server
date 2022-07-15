#include "thread_pool.h"

#include "../util/exception/general_exception.h"

#include "../ws/ws_session.h"
#include "../http/http_session.h"
#include "../device/device_session.h"

#include <sstream>

template <class JobType>
ThreadPool<JobType>::ThreadPool(const std::string& name) 
    : m_logger(name), m_shouldTerminate(false)
{}

template <class JobType>
ThreadPool<JobType>::~ThreadPool() {}

template <class JobType>
void ThreadPool<JobType>::start(int numThreads) {
    if (numThreads <= 0) {
        throw GeneralException("Invalid number of threads.", "HTTPThreadPool::start");
    }

    m_threads.resize(numThreads);
    for (int i = 0; i < numThreads; i++) {
        auto func = [this]{ workerLoop(); };
        m_threads.at(i) = std::thread(func);
    }

    m_logger.log("Started " + std::to_string(numThreads) + " worker threads.");
}

template <class JobType>
void ThreadPool<JobType>::addJob(std::unique_ptr<JobType> job) {
    std::unique_lock<std::mutex> lock(m_mutex);

    m_jobQueue.push(std::move(job));
    m_cv.notify_one();
}

template <class JobType>
void ThreadPool<JobType>::stop() {
    std::unique_lock<std::mutex> lock(m_mutex);

    m_shouldTerminate = true;

    lock.unlock();
    m_cv.notify_all();

    for (auto& x : m_jobs) {
        x.second->stop();
    }

    for (std::thread& t : m_threads) {
        t.join();
    }

    m_threads.clear();

    m_logger.log("All worker threads exited.");
}

template <class JobType>
bool ThreadPool<JobType>::isBusy() {
    std::unique_lock<std::mutex> lock(m_mutex);

    return m_jobQueue.size() > 0;
}

template <class JobType>
int ThreadPool<JobType>::numActiveJobs() {
    std::unique_lock<std::mutex> lock(m_mutex);

    return m_jobs.size();
}

template <class JobType>
void ThreadPool<JobType>::workerLoop() {
    while(true) {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_cv.wait(lock, [this] {
            return !m_jobQueue.empty() || m_shouldTerminate;
        });
        if (m_shouldTerminate) {
            return;
        }

        std::thread::id id = std::this_thread::get_id();

        m_logger.dbg("Thread [" + threadIdToString(id) + "] started job.");

        m_jobs.insert(std::make_pair(id, std::move(m_jobQueue.front())));
        m_jobQueue.pop();

        m_jobs[id]->m_id = id;

        lock.unlock();
        m_jobs[id]->run();
        lock.lock();

        m_jobs.erase(id);

        m_logger.dbg("Thread [" + threadIdToString(id) + "] finished job.");
    }
}

template <class JobType>
std::shared_ptr<JobType> ThreadPool<JobType>::getJob(std::thread::id id) {
    auto mit = m_jobs.find(id);
    if (mit == m_jobs.end()) {
        return nullptr;
    }

    return mit->second;
}

template <class JobType>
std::string ThreadPool<JobType>::threadIdToString(std::thread::id id) {
    std::ostringstream ss;
    ss << id;
    return ss.str();
}

template class ThreadPool<WebSocketSession>;
template class ThreadPool<HTTPSession>;
template class ThreadPool<DeviceSession>;