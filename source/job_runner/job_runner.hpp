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

#ifndef JOBRUNNER_HPP
#define JOBRUNNER_HPP

#include <memory>
#include <string>
#include <tuple>
#include <unistd.h>
#include <vector>

namespace JobRunner {
    /**
     * @brief Structure of job information
     * @details A tuple containing the std output,
     *          std error, and return code, in that order
     */
    using JobInfo = std::tuple<std::string, std::string, int>;
    /**
     * @brief Types of processes
     * @details External processes call programs in the system.
     *          Builtins are called within (see builtins folder)
     */
    enum ProcessType : u_int8_t {
        EXTERNAL,
        BUILTIN,
    };

    //  ==========================
    //  ||   Process Struct     ||
    //  ==========================

    /**
     * @brief Represents a process to execute
     */
    struct Process {
        /**
         * @brief Constructor for a pipeline of processes
         * @param processes - A list of processes to link
         * @details This links up the pipeline in the order given.
         *          Use other constructor to create these processes.
         */
        Process(std::vector<std::shared_ptr<Process>> processes);

        /**
         * @brief Constructor
         * @param isBackground - Sets if we run the process in the background (default to false)
         * @param isSave - Sets if we save the output to a tuple (default to false)
         */
        Process(std::vector<std::string> args, ProcessType type, bool isBackground = false, bool isSave = false);

        /**
         * @brief Destructor
         */
        ~Process() = default;

        /**
         * @brief Get the type of process
         * @return Type of process this is
         */
        [[nodiscard]] ProcessType getType() const;

        /**
         * @brief Get the name of the process
         * @return The name as a const char*
         */
        [[nodiscard]] const char* getName() const;

        /**
         * @brief The arguments for this process
         * @details The first argument should be the name of the process.
         */
        std::vector<std::string> args;

        /**
         * @brief The type of this process
         */
        ProcessType type;

        /**
         * @brief The name of this process
         */
        std::string processName;

        /**
         * @brief Is a background process or not
         */
        bool background;

        /**
         * @brief Save return information or not
         */
        bool saveInfo;

        /**
         * @brief A linked-list of processes, that represents a pipe.
         * @details Order of the linked-list is the order to execute and link pipes.
         */
        std::shared_ptr<Process> pipe = nullptr;

        /**
         * @brief The size of the pipeline
         */
        size_t pipeSize = 1;

        /**
         * @brief Is this process at the start of the pipeline
         */
        bool isFirst = false;

        /**
         * @brief Is this process at the end of the pipeline
         */
        bool isLast = false;
    };

    using ProcessPtr = std::shared_ptr<Process>;

    //  ==========================
    //  ||   JobRunner Class    ||
    //  ==========================

    /**
     * @brief Holds a process and determines how to execute it
     */
    class JobRunner {
    public:
        /**
         * @brief Constructor
         * @param process - The process being set to run
         */
        JobRunner(ProcessPtr process);

        /**
         * @brief Destructor
         */
        ~JobRunner() = default;

        /**
         * @return The job information
         */
        JobInfo execProcess();

    private:
        /**
         * @details The process to execute
         */
        ProcessPtr _process;

        /**
         * @brief Executes a process
         * @details Executes a builtin or external process.
         *          This shouldn't return, so fork before calling if needed.
         * @param process - the process to execute
         * @param in - file descriptor to read from (for builtins)
         * @param out - file descriptor to write to (for builtins)
         */
        void _exec(const ProcessPtr& process);

        /**
         * @brief Exectue a builtin command without returning
         * @details Shouldn't return, so fork before calling if needed.
         * @param process - The process to execute
         * @param in - The file descriptor to read from (default std in)
         * @param out - The file descriptor to write to (default std out)
         */
        void _execBuiltinNoReturn(const ProcessPtr& process, int in = STDIN_FILENO, int out = STDOUT_FILENO);

        /**
         * @brief Execute a builtin
         * @param process - The process to execute
         * @param in - The file descriptor to read from (default std in)
         * @param out - The file descriptor to write to (default std out)
         * @return The job information
         */
        JobInfo _execBuiltin(const ProcessPtr& process, int in = STDIN_FILENO, int out = STDOUT_FILENO);

        /**
         * @brief Execute an external program without forking
         * @details This shouldn't return, so fork before calling if needed.
         * @param process - The process to execute
         */
        void _execNoFork(const ProcessPtr& process);

        /**
         * @brief Execute a external program with a fork
         * @param process - The process to execute
         * @return The job information
         */
        JobInfo _execFork(const ProcessPtr& process);

        /**
         * @brief Does piping of processes
         * @details Should work with any order of builtin and external types.
         *          Don't call a background process in here.
         * @param process - The start process of the pipeline
         * @return The job information of the last process if set to save
         */
        JobInfo _doPiping(const ProcessPtr& process);

        /**
         * @brief Execute a process in the background
         * @param process - The process to execute
         */
        void _doBackground(const ProcessPtr& process);

        /**
         * @brief Set up process to be able to save return information from
         * @param process - The process to execute
         * @param partOfPipe - Is the process part of a pipe
         *                     (if it is, should call this at end of pipe)
         * @return The job information
         */
        JobInfo _doSaveInfo(const ProcessPtr& process, bool partOfPipe, int* fds = nullptr, size_t count = 0);

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
