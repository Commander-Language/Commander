#ifndef JOBRUNNER_HPP
#define JOBRUNNER_HPP
/**
 * @file job_runner.hpp
 * @brief Job Runner header
 * @details A job could be one process or a pipe line of process.
 *          Examples of a job could be:
 *              1) ls -la
 *              2) ls -la | grep *.txt | wc -l
 *              3) ls -la | grep *.txt > output.txt | wc -l
 *
 */

#include <string>
#include <tuple>
#include <vector>

namespace jobRunner {

    /**
     * @brief Helper class for a process struct.
     * @details C functions like exec, use old style strings and string arrays, so use this class to easily use new c++
     * strings/arrays and convert to old style C when needed
     */
    class ProcessArgs {
    private:
        std::vector<std::string> _args;
        std::vector<char*> _cargs;

    public:
        void pushArg(const std::string&);
        char** getArgs();
    };

    /**
     * @brief Enum of types of process
     */
    enum processType {
        NO_PIPE = 1,
        BACKGROUND,
        BUILT_IN,
        PIPE,
        SAVE_RETURN,
        REDIRECTION,
    };

    /**
     * @brief Represents a process
     * @details A process could be one simple process or many processes linked by a pipe. The type of process
     * is determined by given flags.
     */
    struct Process {
        int flags {};
        std::string name {};
        std::string fileToRedirect {};
        Process* pipe {};
        ProcessArgs args {};
        std::tuple<std::string, std::string, int> jobReturn {};
    };

    /**
     * @breif Helper to run a process
     * @details This will use system() to mock running a process
     */
    void runProcessMock(Process*);
    /**
     * Helper to run a process
     */
    void runNoPipeProcess(Process* process);
    /**
     * @brief Helper to run processes in a pipe
     */
    void runPipeProcess(Process* process);
    /**
     * @brief Run a single process
     * @details Run a process normally, in background, or with save return.
     */
    void runProcess(Process* process);

    /**
     * @brief Run a given job
     */
    void runJob(Process*);

    /**
     * @brief Check if given flag set in process
     * @details Helper method for Process struct that checks if flags are set
     * for a process type
     */
    bool checkFlagSet(processType, int);
    /*
     * @brief fork() with error checking
     */
    int Fork();
}  // namespace jobRunner
#endif