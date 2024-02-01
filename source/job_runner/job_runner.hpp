/**
 * @file job_runner.hpp
 * @brief Job Runner header
 * @details A job could be one command or a pipeline of commands.
 *          Examples of a job could be:
 *              1) ls -la
 *              2) ls -la | grep *.txt | wc -l
 *              3) `ls -la`
 *              4) cat text.txt &
 *
 *          Missing features/Known Bugs TODO:
 *              1) Backtick for pipeline bigger than one Ex. `ls -la | grep *.txt | head`
 *              2) Background job for pipeline bigger than one Ex. ls -la | grep *.txt | head &
 *              4) Need to gracefully exit pipeline if process fails in middle of pipeline
 */

#ifndef JOBRUNNER_HPP
#define JOBRUNNER_HPP

#include <string>
#include <tuple>
#include <vector>

namespace jobRunner {

    const int bufferSize = 4096;

    using JobInfo = std::tuple<std::string, std::string, int>;

    /**
     * @brief Enum of types of commands
     */
    enum commandType {
        BACKGROUND,
        BUILT_IN,
        EXEC,
    };

    /**
     * @brief Helper class for a command struct.
     * @details C functions like exec, use old style strings and string arrays, so use this class to easily use new c++
     * strings/arrays and convert to old style C when needed
     */
    class CommandArgs {
    private:
        /**
         * @brief Vector to hold arguments to a string
         */
        std::vector<std::string> _args;
        /**
         * @brief Vector to hold arguments to a string
         * @details Like _cargs, but holds pointers of char
         *          to be able to use in C functions
         */
        std::vector<char*> _cargs;

    public:
        /**
         * @brief Add an arg
         * @param  arg Argument of command to add to list of arguments
         */
        void addArg(const std::string& arg);

        /**
         * @brief Get the list of arguments as C style array
         */
        char** getCArgs();

        /**
         * @brief Get the list of arguments as C++ style vector
         */
        std::vector<std::string> getArgs();
    };

    /**
     * @brief Represents a command
     */
    class Command {
    private:
        /**
         * @brief Helper method to execute a command
         * @details Should be called in a fork
         */
        void _execCommand();

        /**
         * @brief Helper method to execute a builtin
         * @details Should be called in a fork
         */
        void _execBuiltin();

        /**
         * @brief Type of command
         */
        const commandType _type;

        /**
         * @brief Arguments to a command
         */
        CommandArgs _args;

        /**
         * @brief Name of command
         */
        std::string _name;

    public:
        /**
         * @brief Constructor
         * @param name The name of the command
         * @param type Type of command
         * @details Pushes the name of the command to the arguments,
         *          since by convention the program name is the first argument
         */
        Command(std::string name, commandType type);

        /**
         * @brief Run this command
         * @details Currently this should be run in a fork, but
         *          might have to reconsider as we add more builtins
         */
        void runCommand();

        /**
         * @brief Run this command, but return command information
         * @details Should be called in the main parent process this function will deal with fork
         */
        JobInfo runCommandSave();

        /**
         * @brief Add an argument to the command
         * @param arg Argument of command to add
         */
        void addArg(const std::string& arg);

        /**
         * @brief Get args of command
         * @details Just for mock (demo day)
         */
        CommandArgs getArgs();
    };

    /**
     * @brief Represents a pipeline of commands
     * @details A pipeline of commands can be of size one or more
     */
    class PipeCommands {
    private:
        /**
         * @brief Vector of command pointers
         */
        std::vector<Command*> _pipeline;

        /**
         * @brief Helper for running a pipeline of size bigger than one
         */
        JobInfo _runPipeHelper();

    public:
        /**
         * @brief Add a command to the pipeline
         * @param command The command to be added
         */
        void addCommand(Command* command);

        /**
         * @brief Run the pipeline
         * @param save Determine to save job information or not
         */
        JobInfo runPipeLine(bool save);

        /**
         * @brief Run the pipeline mocked
         * @details Just for mock (demo day)
         */
        JobInfo runPipeLineMocked();
    };

    /**
     * @brief Represents a job
     * @details A job is a pipeline of commands and determines if we need to save
     *          the returned information or not
     */
    class Job {
    private:
        /**
         * @brief The pipeline of commands
         */
        PipeCommands _pipeline;

        /**
         * @brief Determine if we want to redirect job return information
         */
        bool _save = false;

        /**
         * @brief Determine if we want to mock the job
         * @details Just for mock (demo day)
         */
        bool _mock = false;

        /**
         * @brief Run the job mocked
         * @return Job Information (just return value in mock)
         * @details Just for mock (demo day)
         */
        JobInfo runJobMocked();

    public:
        /**
         * @brief Run the job
         * @return Job Information of the job (empty if not set to save)
         */
        JobInfo runJob();

        /**
         * @brief Add a command to the pipeline
         * @param command Command to add to pipeline
         */
        void addCommandToPipeline(Command* command);

        /**
         * @brief Set if job should return job information
         * @param save Set true or false to save job info
         */
        void setJobToSave(bool save);

        /**
         * @brief Set to mock a job instead of doing it ourselves
         * @param mock A bool determining to mock the job or not
         * @details Just for mock (demo day)
         */
        void setJobToMock(bool mock);
    };

    /**
     * @brief fork() with error handling
     */
    int forkCheckErrors();

    /**
     * @brief Resize the given array
     * @param arr Array to be resized
     * @param currentSize Number of elements in the array
     * @details Creates a new array of double size, then copies old array data to it.
     *          Then set the old array to this new array.
     */
    void resizeArrayHelper(char** arr, size_t currentSize);

}  // namespace jobRunner
#endif
