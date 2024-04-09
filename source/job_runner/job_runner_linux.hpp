/**
 * @file job_runner_linux.hpp
 * @brief A job runner implementation for Linux/Unix
 */

#ifndef JOBRUNNER_LINUX_HPP
#define JOBRUNNER_LINUX_HPP

#include "builtins/builtins.hpp"
#include "job_runner_interface.hpp"
#include "process.hpp"
#include "source/util/print.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <tuple>
#include <unistd.h>
#include <vector>

namespace JobRunner {
    //  ==========================
    //  ||   JobRunner Class    ||
    //  ==========================
    /**
     * @brief Holds a process and determines how to execute it
     */
    class JobRunnerLinux : JobRunner {
    public:
        /**
         * @brief Constructor
         * @param process - The process being set to run
         */
        JobRunnerLinux(Process::ProcessPtr process);

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
        void _exec(const Process::ProcessPtr& process);

        /**
         * @brief Exectue a builtin command without returning
         * @details Shouldn't return, so fork before calling if needed.
         * @param process - The process to execute
         * @param in - The file descriptor to read from (default std in)
         * @param out - The file descriptor to write to (default std out)
         */
        void _execBuiltinNoReturn(const Process::ProcessPtr& process, int in = STDIN_FILENO, int out = STDOUT_FILENO);

        /**
         * @brief Execute a builtin
         * @param process - The process to execute
         * @param in - The file descriptor to read from (default std in)
         * @param out - The file descriptor to write to (default std out)
         * @return The job information
         */
        JobInfo _execBuiltin(const Process::ProcessPtr& process, int in = STDIN_FILENO, int out = STDOUT_FILENO);

        /**
         * @brief Execute an external program without forking
         * @details This shouldn't return, so fork before calling if needed.
         * @param process - The process to execute
         */
        void _execNoFork(const Process::ProcessPtr& process);

        /**
         * @brief Execute a external program with a fork
         * @param process - The process to execute
         * @return The job information
         */
        JobInfo _execFork(const Process::ProcessPtr& process);

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

        /**
         * @brief Helper to resize a char array
         * @details Create a new array of double size, copy contents
         *          of old array and then set old array to new array
         * @param array - The array to resize
         * @param size - The current size of the array
         */
        void _resize(std::unique_ptr<char[]>& array, size_t size);

        /**
         * @brief A helper to do a fork with error checking
         * @return The process ID of the forked child
         */
        int _fork();
    };

}  // namespace JobRunner
#endif
