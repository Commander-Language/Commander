/**
 * @file thread_queue.cpp
 * @brief Implements most of the `ThreadQueue` class's methods.
 * @details See header file for documentation.
 */

#include "thread_queue.hpp"

#include <iostream>

namespace Util {

    ThreadQueue::ThreadQueue(const std::size_t numThreads)
        : _threads(numThreads), _stop(false), _waiting(false), _workingThreads(numThreads) {
        for (auto& thread : _threads) thread = std::thread([&] { _eventLoop(); });
    }

    ThreadQueue::~ThreadQueue() {
        wait();

        {
            const std::unique_lock lock(_guard);
            _stop = true;
        }

        _newJobSig.notify_all();
        for (auto& thread : _threads) thread.join();
    }

    void ThreadQueue::wait() {
        std::unique_lock lock(_guard);

        _waiting = true;
        _doneWaitingSig.wait(lock, [this] { return _workingThreads == 0 && _jobs.empty(); });
        _waiting = false;
    }

    void ThreadQueue::_eventLoop() {
        std::unique_lock lock(_guard);

        while (true) {
            --_workingThreads;

            const bool shouldNotify = _waiting && _workingThreads == 0 && _jobs.empty();
            lock.unlock();
            if (shouldNotify) _doneWaitingSig.notify_all();
            lock.lock();

            _newJobSig.wait(lock, [this] { return _stop || !_jobs.empty(); });
            if (_stop) return;

            const auto job = std::move(_jobs.front());
            _jobs.pop();

            ++_workingThreads;
            lock.unlock();
            job();
            lock.lock();
        }
    }

}  // namespace Util
