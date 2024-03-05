/**
 * @file job_runner.hpp
 * @brief Implements the classes Process and JobRunner
 */
#include "job_runner.hpp"

/* Unix/Mac specific includes */
#include <fcntl.h>
#include <memory>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>
/* Windows specific includes */
// #include <Windows.h>

namespace JobRunner {
    //  ==========================
    //  ||   Process Struct     ||
    //  ==========================

    ProcessType Process::getType() const { return type; }

    const char* Process::getName() const { return processName.c_str(); }

    Process::Process(std::vector<ProcessPtr> processes)
        : pipe(processes[1]), pipeSize(processes.size()), isFirst(true) {
        // first in pipe is this process
        ProcessPtr start = processes[0];
        type = start->type;
        processName = start->processName;
        args = start->args;
        background = start->background;
        saveInfo = start->saveInfo;

        // connect the pipeline
        for (int i = 2; i < processes.size(); i++) { processes[i - 1]->pipe = processes[i]; }

        processes.back()->isLast = true;
    }

    Process::Process(std::vector<std::string> args, ProcessType type, bool isBackground, bool isSave)
        : args(args), type(type), processName(args[0]), background(isBackground), saveInfo(isSave) {}

    //  ==========================
    //  ||   JobRunner Class    ||
    //  ==========================

    JobRunner::JobRunner(ProcessPtr process) : _process(std::move(process)) {}

    JobInfo JobRunner::execProcess() {
        std::stringstream buffer;
        std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());

        switch (_process->getType()) {
            case ProcessType::BUILTIN: {
                if (_process->pipe != nullptr) { return _doPiping(_process); }
                if (_process->background) {
                    _doBackground(_process);
                    return {};
                }
                if (_process->saveInfo) { return _doSaveInfo(_process, false); }
                std::cout.rdbuf(old);
                std::string output = buffer.str();
                Util::println(output);
                return _execBuiltin(_process);
            }
            case ProcessType::EXTERNAL: {
                if (_process->pipe != nullptr) { return _doPiping(_process); }
                if (_process->background) {
                    _doBackground(_process);
                    return {};
                }
                if (_process->saveInfo) { return _doSaveInfo(_process, false); }
                std::cout.rdbuf(old);
                std::string output = buffer.str();
                Util::println(output);
                return _execFork(_process);
            }
            default:
                return {};
        }

    }

    JobInfo JobRunner::_execBuiltin(const ProcessPtr& process, int in, int out) {
        // get the function so we can call it!
        auto builtin = Builtins::getBuiltinFunction(process->getName());
        return builtin(process->args, in, out);
    }

    void JobRunner::_execBuiltinNoReturn(const ProcessPtr& process, int in, int out) {
        // get the function so we can call it!
        auto builtin = Builtins::getBuiltinFunction(process->getName());
        builtin(process->args, in, out);
        _Exit(0);
    }

    void JobRunner::_execNoFork(const ProcessPtr& process) {
        // convert to c style array
        std::vector<char*> cargs;
        cargs.reserve(process->args.size() + 1);
        for (auto& arg : process->args) { cargs.emplace_back(arg.data()); }
        // make sure to null terminate!
        cargs.emplace_back(nullptr);

        execvp(process->getName(), cargs.data());
        throw Util::CommanderException("Job Runner: Bad exec");
    }

    JobInfo JobRunner::_execFork(const ProcessPtr& process) {
        int pid = _fork();
        if (pid == 0) {
            // convert to c style array
            std::vector<char*> cargs;
            cargs.reserve(process->args.size() + 1);
            for (auto& arg : process->args) { cargs.emplace_back(arg.data()); }
            // make sure to null terminate!
            cargs.emplace_back(nullptr);

            execvp(process->getName(), cargs.data());
            throw Util::CommanderException("Job Runner: Bad exec");
        }
        wait(nullptr);

        return {"", "", SUCCESS};
    }

    void JobRunner::_exec(const ProcessPtr& process) {
        switch (process->getType()) {
            case ProcessType::EXTERNAL: {
                _execNoFork(process);
            }
            case ProcessType::BUILTIN: {
                _execBuiltinNoReturn(process);
            }
        }
    }

    JobInfo JobRunner::_doPiping(const ProcessPtr& process) {
        JobInfo result {};

        size_t fdCount = (process->pipeSize - 1) * 2;
        int pipes[fdCount];
        for (int i = 0; i < fdCount; i += 2) { pipe2(&pipes[i], O_CLOEXEC); }

        int rIndex = 0;
        int wIndex = 1;
        ProcessPtr current = process;

        while (current != nullptr) {
            if (current->isFirst) {
                int pid = _fork();
                if (pid == 0) {
                    dup2(pipes[wIndex], STDOUT_FILENO);
                    for (int i = 0; i < fdCount; i++) { close(pipes[i]); }
                    _exec(current);
                }
                wIndex += 2;
            } else if (current->isLast) {
                if (current->saveInfo) {
                    result = _doSaveInfo(current, true, pipes, fdCount);
                } else {
                    int pid = _fork();
                    if (pid == 0) {
                        dup2(pipes[rIndex], STDIN_FILENO);
                        for (int i = 0; i < fdCount; i++) { close(pipes[i]); }
                        _exec(current);
                    }
                }
            } else {
                int pid = _fork();
                if (pid == 0) {
                    dup2(pipes[rIndex], STDIN_FILENO);
                    dup2(pipes[wIndex], STDOUT_FILENO);
                    for (int i = 0; i < fdCount; i++) { close(pipes[i]); }
                    _exec(current);
                }
                rIndex += 2;
                wIndex += 2;
            }
            current = current->pipe;
        }

        for (int i = 0; i < fdCount; i++) { close(pipes[i]); }
        for (int i = 0; i < process->pipeSize; i++) { wait(nullptr); }

        return result;
    }

    void JobRunner::_doBackground(const ProcessPtr& process) {
        int pid = _fork();
        if (pid == 0) {
            int pid2 = _fork();
            if (pid2 == 0) { _exec(process); }
            _Exit(0);
        }
        waitpid(pid, nullptr, 0);
    }

    JobInfo JobRunner::_doSaveInfo(const ProcessPtr& process, bool partOfPipe, int* fds, size_t count) {
        int pipeOut[2];
        int pipeErr[2];
        pipe2(pipeOut, O_CLOEXEC);
        pipe2(pipeErr, O_CLOEXEC);

        int pid = _fork();
        if (pid == 0) {
            // if part of pipe set up the last pipe here
            if (partOfPipe) {
                dup2(fds[count - 2], STDIN_FILENO);
                for (int i = 0; i < count; i++) { close(fds[i]); }
            }
            dup2(pipeOut[1], STDOUT_FILENO);
            dup2(pipeErr[1], STDERR_FILENO);

            close(pipeOut[0]);
            close(pipeOut[1]);
            close(pipeErr[0]);
            close(pipeErr[1]);

            _exec(process);
        }

        // don't forget to close pipes
        if (partOfPipe)
            for (int i = 0; i < count; i++) { close(fds[i]); }

        // close write ends
        close(pipeOut[1]);
        close(pipeErr[1]);

        // size of buffer
        size_t stdoutBufSize = 2048;
        size_t stderrBufSize = 2048;

        auto stdoutOutput = std::make_unique<char[]>(stdoutBufSize);
        auto stderrOutput = std::make_unique<char[]>(stderrBufSize);

        // size of output
        size_t stdoutSize = 0;
        size_t stderrSize = 0;
        // number of bytes read
        size_t stdoutRead = 0;
        size_t stderrRead = 0;

        bool reading = true;
        while (reading) {
            stdoutRead = read(pipeOut[0], &stdoutOutput[stdoutSize], stdoutBufSize - stdoutSize);
            stderrRead = read(pipeErr[0], &stderrOutput[stderrSize], stderrBufSize - stderrSize);

            if (stdoutRead == 0 && stderrRead == 0) reading = false;

            stdoutSize += stdoutRead;
            stderrSize += stderrRead;

            if (stdoutSize >= stdoutBufSize) {
                _resize(stdoutOutput, stdoutBufSize);
                stdoutBufSize *= 2;
            }
            if (stderrSize >= stderrBufSize) {
                _resize(stderrOutput, stderrBufSize);
                stderrBufSize *= 2;
            }
        }
        close(pipeOut[0]);
        close(pipeErr[0]);

        if (stdoutSize >= stdoutBufSize) _resize(stdoutOutput, stdoutBufSize);
        stdoutOutput[stdoutSize] = '\0';
        if (stderrSize >= stderrBufSize) _resize(stderrOutput, stderrBufSize);
        stderrOutput[stderrSize] = '\0';

        int stat;
        waitpid(pid, &stat, 0);

        std::string out(stdoutOutput.get());
        std::string err(stderrOutput.get());

        return {out, err, WEXITSTATUS(stat)};
    }

    //  ==========================
    //  ||    Helper Methods    ||
    //  ==========================
    void JobRunner::_resize(std::unique_ptr<char[]>& arr, size_t size) {
        auto newArray = std::make_unique<char[]>(size * 2);
        memcpy(newArray.get(), arr.get(), size);

        arr = std::move(newArray);
    }

    int JobRunner::_fork() {
        int pid = fork();
        if (pid < 0) { throw Util::CommanderException("Job Runner: Error forking"); }
        return pid;
    }
}  // namespace JobRunner
