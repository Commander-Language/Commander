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
 *          refactored: 02/06/24 inspired by fish shell
 */

#ifndef JOBRUNNER_HPP
#define JOBRUNNER_HPP

#include <string>
#include <tuple>
#include <vector>

namespace JobRunner {
    using JobInfo = std::tuple<std::string, std::string, int>;

    enum ProcessType : u_int8_t {
        EXTERNAL,
        BUILTIN,
    };

    class Process {
    public:
        ~Process() = default;

        [[nodiscard]] const ProcessType getType();
        [[nodiscard]] const char* getProcess();

        const char* processName;

        // ensure this array of args is null terminated!!
        std::vector<std::string> args;

        bool background;

        bool saveInfo;

        Process* pipe;
        int writeFD;
        int readFD;
        bool isFirst;
        bool isLast;
    };

    JobInfo execProcess(Process*);

    JobInfo execBuiltin(Process*);

    JobInfo execFork(Process*);
    void execNoFork(Process*);

    JobInfo doPiping(Process*);
    JobInfo doBackground(Process*);
    JobInfo doSaveInfo(Process*, bool);
    std::string getOutputFromFD(int);
}  // namespace JobRunner
#endif
