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
 *              3) Save return doesn't have dynamic memory. Max size for backtick is currently 4096
 *              4) Need to gracefully exit pipeline if process fails in middle of pipeline
 */

#ifndef JOBRUNNER_HPP
#define JOBRUNNER_HPP

#include <string>
#include <tuple>
#include <vector>

namespace jobRunner {

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
        std::vector<std::string> _args;
        std::vector<char*> _cargs;

    public:
        /**
         * @brief Add an arg
         */
        void addArg(const std::string&);

        /**
         * @brief Get the list of arguments
         */
        char** getArgs();
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

        const commandType _type;
        CommandArgs _args;
        std::string _name;

    public:
        /**
         * @brief Constructor
         */
        Command(std::string, commandType);

        /**
         * @brief Run this command
         */
        void runCommand();

        /**
         * @brief Run this command, but return command information
         * @details Should be called in the main parent process this function will deal with fork
         */
        JobInfo runCommandSave();

        /**
         * @brief Add an argument to the command
         */
        void addArg(const std::string&);
    };

    /**
     * @brief Represents a pipeline of commands
     * @details A pipeline of commands can be of size one or more
     */
    class PipeCommands {
    private:
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
         *
         */
        JobInfo runPipeLine(bool save);
    };

    /**
     * @brief Represents a job
     * @details A job is a pipeline of commands and determines if we need to save
     *          the returned information or not
     */
    class Job {
    private:
        PipeCommands _pipeline;
        bool _save = false;

    public:
        /**
         * @brief Run the job
         * @return Job Information of the job (empty if not set to save)
         */
        JobInfo runJob();

        /**
         * @brief Add a command to the pipeline
         */
        void addCommandToPipeline(Command*);

        /**
         * @brief Set if job should return job information
         */
        void setJobToSave(bool);
    };

    /*
     * @brief fork() with error handling
     */
    int forkCheckErrors();

}  // namespace jobRunner
#endif
