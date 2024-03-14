/**
 * @file thread_queue.hpp
 * @brief Defines the `ThreadQueue` class, which queues tasks and runs them in parallel.
 * @details `ThreadQueue::add()` is a templated function, so implementation is in this file.
 */

#ifndef THREAD_QUEUE_HPP
#define THREAD_QUEUE_HPP

#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace Util {

    /**
     * @brief Queues tasks and runs them in parallel.
     * @details Uses a fixed number of threads for performance.
     */
    class ThreadQueue {
    public:
        /**
         * @brief The default number of threads to use.
         */
        static constexpr std::size_t defaultNumThreads = 8;

        /**
         * @brief Class constructor.
         *
         * @param numThreads The number of threads to use.
         */
        explicit ThreadQueue(std::size_t numThreads = defaultNumThreads);

        /**
         * @brief Class destructor.
         * @details Joins all threads before destructing.
         */
        ~ThreadQueue();

        /**
         * @brief Adds the given job to the queue.
         * @details Automatically starts the job when there is a free thread.
         *     A job is a callable function.
         *     If that function takes arguments, then the arguments must also be provided.
         *
         *     Example:
         *
         *     ```
         *     ThreadQueue tasks;
         *
         *     const auto printFive = [](){ std::cout << 5 << "\n"; };
         *     tasks.add(printFive);
         *
         *     const auto printNum = [](int num) { std::cout << num << "\n"; };
         *     tasks.add(printNum, 42);
         *     ```
         *
         * @tparam FuncType The type of the function being added.
         * @tparam ArgsType The type of the arguments of the function being added.
         * @param job The job to add to the queue, as a callable function.
         * @param args Zero or more arguments that are passed to the function.
         */
        template<typename FuncType, typename... ArgsType>
        void add(FuncType&& job, ArgsType&&... args);

        /**
         * @brief Waits for all jobs in the queue to finish running.
         */
        void wait();

    private:
        /**
         * @brief This function is what each thread runs; it loops each time a new task is given to it.
         */
        void _eventLoop();

        /**
         * @brief This acts as a signal to a thread, indicating that there is a new available job.
         */
        std::condition_variable _newJobSig;

        /**
         * @brief This acts as a signal to a thread, indicating that all jobs are finished running.
         */
        std::condition_variable _doneWaitingSig;

        /**
         * @brief A mutex for guarding critical sections.
         */
        std::mutex _guard;

        /**
         * @brief The queue of jobs to run.
         * @details Each job is essentially wrapped in a lambda that takes no arguments and returns nothing.
         */
        std::queue<std::function<void()>> _jobs;

        /**
         * @brief The set of all the threads belonging to this thread queue.
         */
        std::vector<std::thread> _threads;

        /**
         * @brief Whether the threads should break from the event loop if the job queue is empty.
         * @details Used for cleanup.
         */
        std::atomic_bool _stop;

        /**
         * @brief Whether there is a thread waiting for all jobs to finish running.
         */
        std::atomic_bool _waiting;

        /**
         * @brief The number of threads that are currently running jobs.
         */
        std::atomic_uint _workingThreads;
    };

    template<typename FuncType, typename... ArgsType>
    void ThreadQueue::add(FuncType&& job, ArgsType&&... args) {
        {
            const auto func = std::bind(std::forward<FuncType>(job), std::forward<ArgsType>(args)...);
            const std::lock_guard lock(_guard);
            _jobs.push(func);
        }
        _newJobSig.notify_one();
    }

}  // namespace Util

#endif  // THREAD_QUEUE_HPP
