/**
 * @file job_runner_interface.hpp
 * @brief A job runner interface class
 */

#ifndef JOBRUNNER_INTERFACE_HPP
#define JOBRUNNER_INTERFACE_HPP

#include "process.hpp"
#include <string>
#include <tuple>

namespace JobRunner {
    /**
     * @brief Structure of job information
     * @details A tuple containing the std output,
     *          std error, and return code, in that order
     */
    using JobInfo = std::tuple<std::string, std::string, int>;

    //  ==========================
    //  ||   JobRunner Class    ||
    //  ==========================
    /**
     * @brief Holds a process and determines how to execute it
     */
    class JobRunner {
    public:
        /**
         * @brief Destructor
         */
        virtual ~JobRunner() = default;

        /**
         * @return The job information
         */
        virtual JobInfo execProcess() = 0;

    protected:
        /**
         * @details The process to execute
         */
        Process::ProcessPtr _process;

        const size_t _bufSize = 8192;
    };

}  // namespace JobRunner
#endif
