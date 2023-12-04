/**
 * @file job_runner.hpp
 * @brief Job Runner Implementation
 */
#include "job_runner.hpp"
/* Unix/Mac specific includes */
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
/* Windows specific includes */
// #include <Windows.h>

namespace jobRunner {
    /*
     * CommandArgs Class
     */
    void CommandArgs::addArg(const std::string& arg) { _args.emplace_back(arg); }

    char** CommandArgs::getArgs() {
        _cargs.reserve(_args.size());
        for (auto& arg : _args) _cargs.emplace_back(arg.data());
        return _cargs.data();
    }

    /*
     * Command Class
     */
    Command::Command(std::string name, commandType type) : _name(std::move(name)), _type(type) { addArg(_name); }

    void Command::addArg(const std::string& arg) { _args.addArg(arg); }

    void Command::runCommand() {
        switch (_type) {
            case commandType::EXEC: {
                _execCommand();
            }
            case commandType::BACKGROUND: {
                // we double fork here, letting system deal with lifetime of process
                int const pid = forkCheckErrors();
                if (pid == 0) { _execCommand(); }
                _exit(1);
            }
            case commandType::BUILT_IN: {
                // TODO(): Implement built ins
            }
        }
    }

    void Command::_execCommand() { execvp(_name.c_str(), _args.getArgs()); }

    JobInfo Command::runCommandSave() {
        // bufferSize
        const int bufferSize = 4096;

        // create two pipes; one for stdout and another for stderr
        int stdoutPipe[2];
        int stderrPipe[2];
        pipe2(stdoutPipe, O_CLOEXEC);
        pipe2(stderrPipe, O_CLOEXEC);

        int const processID = forkCheckErrors();
        if (processID == 0) {
            close(stdoutPipe[0]);
            close(stderrPipe[0]);

            dup2(stdoutPipe[1], 1);
            dup2(stderrPipe[1], 2);

            close(stdoutPipe[1]);
            close(stderrPipe[1]);

            _execCommand();
        }

        // close write end of pipes since we won't need them
        close(stdoutPipe[1]);
        close(stderrPipe[1]);

        // we will save output here
        char stdoutBuffer[bufferSize];
        char stderrBuffer[bufferSize];

        bool stillReading = true;

        // size of our buffers so far
        size_t stderrSize = 0;
        size_t stdoutSize = 0;
        // count how many bytes read from read() call
        size_t stderrRead;
        size_t stdoutRead;

        while (stillReading) {
            stdoutRead = read(stdoutPipe[0], stdoutBuffer, sizeof(stdoutBuffer));
            stderrRead = read(stderrPipe[0], stderrBuffer, sizeof(stderrBuffer));

            if (stdoutRead == 0 && stderrRead == 0) stillReading = false;
            else {
                stderrSize += stderrRead;
                stdoutSize += stdoutRead;
            }
        }

        // we are done reading, so close read end of pipes
        close(stdoutPipe[0]);
        close(stderrPipe[0]);

        // null terminate our char arrays
        if (stderrSize > bufferSize) stderrBuffer[bufferSize - 1] = '\0';
        else
            stderrBuffer[stderrSize] = '\0';

        if (stdoutSize > bufferSize - 1) stdoutBuffer[bufferSize - 1] = '\0';
        else
            stdoutBuffer[stdoutSize] = '\0';

        // we want stats process
        int stat;
        waitpid(processID, &stat, 0);

        std::string stdoutString(stdoutBuffer);
        std::string stderrString(stderrBuffer);

        return {stdoutString, stderrString, WEXITSTATUS(stat)};
    }

}  // namespace jobRunner
