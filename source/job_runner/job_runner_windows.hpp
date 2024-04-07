/**
 * @file job_runner_windows.hpp
 * @brief A job runner implementation for Windows
 */

#ifndef JOBRUNNER_WINDOWS_HPP
#define JOBRUNNER_WINDOWS_HPP

#include "job_runner_interface.hpp"
#include "builtins/builtins.hpp"
#include "process.hpp"
#include "source/util/print.hpp"

namespace JobRunner {
    //  ==========================
    //  ||   JobRunner Class    ||
    //  ==========================
    /**
     * @brief Holds a process and determines how to execute it
     */
    class JobRunnerWindows : JobRunner {
    public:
        /**
         * @brief Constructor
         * @param process - The process being set to run
         */
        JobRunnerWindows(Process::ProcessPtr process);

        /**
         * @return The job information
         */
        JobInfo execProcess() override;

    private:
        /**
         * @brief Executes a process
         * @details Executes a builtin or external process.
         *          This shouldn't return, so fork before calling if needed.
         * @param process - The process to execute
         * @param background - Is the process a background process
         * @param pipe - Is the process a piped process
         */
        JobInfo _exec(const Process::ProcessPtr& process, bool background, bool pipe);

        /**
         * @brief Setup a string to be able to run in a windows powershell/command-line context
         * @param process - The process to parse to a string
         * @param pipe - Is the process a piped process
         * @return A string containing the command
         */
        std::string _setupCommandString(const Process::ProcessPtr& process, bool pipe);

        /**
         * @brief Exectue a builtin command without returning
         * @details Shouldn't return, so fork before calling if needed.
         * @param process - The process to execute
         */
        void _execBuiltinNoReturn(const Process::ProcessPtr& process);

        /**
         * @brief Execute a builtin
         * @param process - The process to execute
         * @return The job information
         */
        JobInfo _execBuiltin(const Process::ProcessPtr& process);

        /**
         * @brief Set up process to be able to save return information from
         * @param process - The process to execute
         * @param partOfPipe - Is the process part of a pipe
         *                     (if it is, should call this at end of pipe)
         * @return The job information
         */
        JobInfo _doSaveInfo(const Process::ProcessPtr& process, bool partOfPipe);
    };

}  // namespace JobRunner
#endif
