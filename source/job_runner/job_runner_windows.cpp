/**
 * @file job_runner.hpp
 * @brief Implements the classes Process and JobRunner
 */
#include "job_runner_windows.hpp"
#include "process.hpp"
#include <memory>
#include <utility>

/* Windows specific includes */
#include <Windows.h>

namespace JobRunner {

    //  ==========================
    //  ||   JobRunner Class    ||
    //  ==========================

    JobRunnerWindows::JobRunnerWindows(Process::ProcessPtr process) { _process = std::move(process); }

    JobInfo JobRunnerWindows::execProcess() {
        switch (_process->getType()) {
            case Process::ProcessType::BUILTIN: {
                if (_process->pipe != nullptr) { return _doPiping(_process); }
                if (_process->background) {
                    _doBackground(_process);
                    return {};
                }
                if (_process->saveInfo) { return _doSaveInfo(_process, false); }
                return _execBuiltin(_process);
            }
            case Process::ProcessType::EXTERNAL: {
                if (_process->pipe != nullptr) { return _doPiping(_process); }
                if (_process->background) {
                    _doBackground(_process);
                    return {};
                }
                if (_process->saveInfo) { return _doSaveInfo(_process, false); }
                //return _exec(_process);
            }
            default:
                return {};
        }
    }

    JobInfo JobRunnerWindows::_execBuiltin(const Process::ProcessPtr& process, int in, int out) {
        // get the function so we can call it!
        auto builtin = Builtins::getBuiltinFunction(process->getName());
        return builtin(process->args, in, out);
    }

    void JobRunnerWindows::_execBuiltinNoReturn(const Process::ProcessPtr& process, int in, int out) {
        // get the function so we can call it!
        auto builtin = Builtins::getBuiltinFunction(process->getName());
        builtin(process->args, in, out);
        _Exit(0);
    }


    void JobRunnerWindows::_exec(const Process::ProcessPtr& process) {
    }

    JobInfo JobRunnerWindows::_doPiping(const Process::ProcessPtr& process) {
    }

    void JobRunnerWindows::_doBackground(const Process::ProcessPtr& process) {
    }

    JobInfo JobRunnerWindows::_doSaveInfo(const Process::ProcessPtr& process, bool partOfPipe, int* fds, size_t count) {
    }
}  // namespace JobRunner
