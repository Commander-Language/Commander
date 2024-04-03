/**
 * @file job_runner.hpp
 * @brief Implements the classes Process and JobRunner
 */
#include "job_runner_windows.hpp"
#include "process.hpp"
#include <Windows.h>
#include <cstdlib>
#include <memory>
#include <utility>

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
                return _exec(_process);
            }
            default:
                return {};
        }
    }

    JobInfo JobRunnerWindows::_execBuiltin(const Process::ProcessPtr& process) {
        // get the function so we can call it!
        auto builtin = Builtins::getBuiltinFunction(process->getName());
        return builtin(process->args, 1, 0);
    }

    void JobRunnerWindows::_execBuiltinNoReturn(const Process::ProcessPtr& process) {
        // get the function so we can call it!
        auto builtin = Builtins::getBuiltinFunction(process->getName());
        builtin(process->args, 1, 0);
        _Exit(0);
    }


    JobInfo JobRunnerWindows::_exec(const Process::ProcessPtr& process) {
        STARTUPINFO startUp {};
        PROCESS_INFORMATION processInfo {};
        std::string name = "powershell -Command " + process->args[0];
        if (!CreateProcess(nullptr,                  //
                           name.data(),  //
                           nullptr,                  //
                           nullptr,                  //
                           TRUE,                     //
                           0,                        //
                           nullptr,                  //
                           nullptr,                  //
                           &startUp,                 //
                           &processInfo)) {
            throw Util::CommanderException("Job Runner: Bad Exec");
        }
        WaitForSingleObject(processInfo.hProcess, INFINITE);

        DWORD returnCode;
        GetExitCodeProcess(processInfo.hProcess, &returnCode);
        return {"", "", returnCode};
    }

    JobInfo JobRunnerWindows::_doPiping(const Process::ProcessPtr& process) {
        return {};
    }

    void JobRunnerWindows::_doBackground(const Process::ProcessPtr& process) {

    }

    JobInfo JobRunnerWindows::_doSaveInfo(const Process::ProcessPtr& process, bool partOfPipe, int* fds, size_t count) {
        return {};
    }

}  // namespace JobRunner
