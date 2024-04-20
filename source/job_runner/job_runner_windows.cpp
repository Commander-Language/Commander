/**
 * @file job_runner_windows.hpp
 * @brief Implements the JobRunner
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
                if (_process->pipe != nullptr) { return _exec(_process, false, true); }
                if (_process->background) { return _exec(_process, true, false); }
                if (_process->saveInfo) { return _doSaveInfo(_process, false); }
                return _execBuiltin(_process);
            }
            case Process::ProcessType::EXTERNAL: {
                if (_process->pipe != nullptr) { return _exec(_process, false, true); }
                if (_process->background) { return _exec(_process, true, false); }
                if (_process->saveInfo) { return _doSaveInfo(_process, false); }
                return _exec(_process, false, false);
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

    JobInfo JobRunnerWindows::_exec(const Process::ProcessPtr& process, bool background, bool pipe) {
        if (process->saveInfo) { return _doSaveInfo(process, pipe); }

        std::string command = _setupCommandString(process, pipe);
        STARTUPINFO startUp {};
        PROCESS_INFORMATION processInfo {};
        if (background) { startUp.wShowWindow = SW_HIDE; }
        std::string name = "powershell -Command " + command;
        if (!CreateProcess(nullptr,      //
                           name.data(),  //
                           nullptr,      //
                           nullptr,      //
                           TRUE,         //
                           0,            //
                           nullptr,      //
                           nullptr,      //
                           &startUp,     //
                           &processInfo)) {
            throw Util::CommanderException("Job Runner: Bad Exec");
        }

        if (!background) { WaitForSingleObject(processInfo.hProcess, INFINITE); }

        DWORD returnCode;
        GetExitCodeProcess(processInfo.hProcess, &returnCode);

        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
        return {"", "", returnCode};
    }

    JobInfo JobRunnerWindows::_doSaveInfo(const Process::ProcessPtr& process, bool partOfPipe) {
        SECURITY_ATTRIBUTES saAttr;
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = nullptr;

        HANDLE stdRead, stdWrite;
        HANDLE errRead, errWrite;
        if (CreatePipe(&stdRead, &stdWrite, &saAttr, 0) == 0) { throw Util::CommanderException(""); }
        if (CreatePipe(&errRead, &errWrite, &saAttr, 0) == 0) { throw Util::CommanderException(""); }

        SetHandleInformation(stdRead, HANDLE_FLAG_INHERIT, 0);
        SetHandleInformation(errRead, HANDLE_FLAG_INHERIT, 0);

        STARTUPINFO startupInfo {};
        PROCESS_INFORMATION processInfo {};

        startupInfo.cb = sizeof(STARTUPINFO);
        startupInfo.hStdError = errWrite;
        startupInfo.hStdOutput = stdWrite;
        startupInfo.dwFlags |= STARTF_USESTDHANDLES;

        std::string name = "powershell -Command " + _setupCommandString(process, partOfPipe);
        if (!CreateProcess(nullptr, name.data(), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &startupInfo,
                           &processInfo)) {
            CloseHandle(stdRead);
            CloseHandle(stdWrite);
            throw Util::CommanderException("");
        }

        CloseHandle(stdWrite);
        CloseHandle(errWrite);

        DWORD bytesRead;

        const size_t bufSize = 4096;
        char stdBuffer[bufSize];

        std::string stdOutput;

        while ((ReadFile(stdRead, stdBuffer, bufSize, &bytesRead, nullptr) != 0) && bytesRead != 0) {
            stdOutput.append(stdBuffer, bytesRead);
        }

        bytesRead = 0;
        char errBuffer[bufSize];
        std::string errOutput;

        while ((ReadFile(errRead, errBuffer, bufSize, &bytesRead, nullptr) != 0) && bytesRead != 0) {
            errOutput.append(errBuffer, bytesRead);
        }

        DWORD returnCode;
        GetExitCodeProcess(processInfo.hProcess, &returnCode);

        CloseHandle(stdRead);
        CloseHandle(errRead);
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);

        return {stdOutput, errOutput, returnCode};
    }

    std::string JobRunnerWindows::_setupCommandString(const Process::ProcessPtr& process, bool pipe) {
        std::string command;
        if (pipe) {
            auto current = process;
            while (current) {
                for (auto& arg : current->args) {
                    command.append(arg);
                    command.append(" ");
                }
                current = current->pipe;
                if (current) command.append(" | ");
            }
            return command;
        }
        for (auto& arg : process->args) {
            command.append(arg);
            command.append(" ");
        }
        return command;
    }
}  // namespace JobRunner
