/**
 * @file job_runner.hpp
 * @brief Job Runner Implementation
 */
#include "job_runner.hpp"
/* Unix/Mac specific includes */
#include <sys/wait.h>
#include <unistd.h>
/* Windows specific includes */
// #include <Windows.h>

namespace jobRunner {
    void ProcessArgs::pushArg(const std::string& arg) { _args.emplace_back(arg); }

    char** ProcessArgs::getArgs() {
        _cargs.reserve(_args.size());
        for (auto& cstr : _args) _cargs.push_back(cstr.data());
        return _cargs.data();
    }

    bool checkFlagSet(processType type, int flags) { return (type & flags) != 0; }

    void runJob(Process* process) {
        if (checkFlagSet(jobRunner::PIPE, process->flags)) {
            // TODO: Implement pipe process
        }
        if (checkFlagSet(jobRunner::NONE, process->flags)) {
            // TODO: Implement no pipe process
        }
    }

    void runJobMock(Process*) {
        // TODO: Implement runJobMock()
    }
}  // namespace jobRunner