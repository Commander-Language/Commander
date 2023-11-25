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
 */

#include <string>
#include <tuple>
#include <vector>

namespace jobRunner {
    /**
     * @brief Enum of types of process
     */
    enum processType {
        BACKGROUND,
        BUILT_IN,
        PIPE_START,
        PIPE_BETWEEN,
        PIPE_END,
        SAVE_RETURN,
        REDIRECTION,
    };

    /**
     * @brief Represents a process
     * @details A process could be one simple process or many processes linked by a pipe. The type of process
     * is determined by given flags.
     */
    struct Process {
        int flags;
        std::string name;
        std::string fileToRedirect;
        Process* pipe;
    };

    using JobReturn = std::tuple<std::string, std::string, int>;

    /**
     * @brief Run a given job
     */
    JobReturn runJob(Process*);

}  // namespace jobRunner
#endif