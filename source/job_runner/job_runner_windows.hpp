/**
 * @file job_runner.hpp
 * @brief Definitions for job runner classes: Process and JobRunner
 * @details A job could be one command or a pipeline of commands.
 *          Examples of a job could be:
 *              1) ls -la
 *              2) ls -la | grep *.txt | wc -l
 *              3) `ls -la`
 *              4) cat text.txt &
 *
 * TODO: Update to use shared_ptrs
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
         * @param process - the process to execute
         * @param in - file descriptor to read from (for builtins)
         * @param out - file descriptor to write to (for builtins)
         */
        JobInfo _exec(const Process::ProcessPtr& process);

        /**
         * @brief Exectue a builtin command without returning
         * @details Shouldn't return, so fork before calling if needed.
         * @param process - The process to execute
         * @param in - The file descriptor to read from (default std in)
         * @param out - The file descriptor to write to (default std out)
         */
        void _execBuiltinNoReturn(const Process::ProcessPtr& process);

        /**
         * @brief Execute a builtin
         * @param process - The process to execute
         * @param in - The file descriptor to read from (default std in)
         * @param out - The file descriptor to write to (default std out)
         * @return The job information
         */
        JobInfo _execBuiltin(const Process::ProcessPtr& process);

        /**
         * @brief Does piping of processes
         * @details Should work with any order of builtin and external types.
         *          Don't call a background process in here.
         * @param process - The start process of the pipeline
         * @return The job information of the last process if set to save
         */
        JobInfo _doPiping(const Process::ProcessPtr& process);

        /**
         * @brief Execute a process in the background
         * @param process - The process to execute
         */
        void _doBackground(const Process::ProcessPtr& process);

        /**
         * @brief Set up process to be able to save return information from
         * @param process - The process to execute
         * @param partOfPipe - Is the process part of a pipe
         *                     (if it is, should call this at end of pipe)
         * @return The job information
         */
        JobInfo _doSaveInfo(const Process::ProcessPtr& process, bool partOfPipe, int* fds = nullptr, size_t count = 0);
    };

}  // namespace JobRunner
#endif
