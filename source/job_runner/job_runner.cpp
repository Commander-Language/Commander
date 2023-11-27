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

    void runProcessMock(Process* process) {
        // TODO: Implement runJobMock()
    }

    void runNoPipeProcess(Process* process) {
        int pid = Fork();
        if (pid == 0) { runProcess(process); }
        wait(nullptr);
    }

    void runPipeProcess(Process* process) {
        int fd[2];
        pipe(fd);
        int pid1 = Fork();
        if (pid1 == 0) {
            // close fd 1 (std output) and replace with our pipe write end
            close(1);
            dup(fd[1]);
            // remember to close pipe ends
            close(fd[0]);
            close(fd[1]);

            runProcess(process);
        }
        int pid2 = Fork();
        if (pid2 == 0) {
            // close fd 0 (std input) and replace with our pipe read end
            close(0);
            dup(fd[0]);
            // remember to close pipe ends
            close(fd[0]);
            close(fd[1]);

            if (process->pipe->pipe != nullptr) {
                runPipeProcess(process->pipe);
            } else {
                runProcess(process->pipe);
            }
        }

        // TODO: error checking on close and wait
        // remember to close pipe ends (even in parent)
        close(fd[0]);
        close(fd[1]);
        wait(nullptr);
        wait(nullptr);

    }

    void runProcess(Process* process) {
        if (checkFlagSet(jobRunner::BACKGROUND, process->flags)) {
            // TODO: Implement background process
        }
        else if (checkFlagSet(jobRunner::SAVE_RETURN, process->flags)) {
            // TODO: Implement save return process process
        }
        else
            execvp(process->name.c_str(), process->args.getArgs());
    }

    void runJob(Process* process) {
        if (checkFlagSet(jobRunner::PIPE, process->flags)) { runPipeProcess(process); }
        if (checkFlagSet(jobRunner::NO_PIPE, process->flags)) { runNoPipeProcess(process); }
    }

    int Fork() {
        int pid = fork();
        if (pid < 0) {
            // TODO: Throw CommanderException
        }
        return pid;
    }

    bool checkFlagSet(processType type, int flags) { return (type & flags) != 0; }
}  // namespace jobRunner