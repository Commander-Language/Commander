/**
 * @file job_runner.hpp
 * @brief Job Runner Implementation
 */
#include "job_runner.hpp"
#include "builtins/builtins.hpp"
#include "source/util/commander_exception.hpp"
#include <cstring>
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

    char** CommandArgs::getCArgs() {
        _cargs.reserve(_args.size() + 1);
        for (auto& arg : _args) _cargs.emplace_back(arg.data());

        // execvp expects a null terminated array
        _cargs.emplace_back(nullptr);
        return _cargs.data();
    }
    std::vector<std::string> CommandArgs::getArgs() {
        std::vector<std::string> result;
        for (auto& arg : _args) { result.emplace_back(arg); }
        return result;
    }

    /*
     * Command Class
     */
    Command::Command(std::string name, commandType type) : _name(std::move(name)), _type(type) { addArg(_name); }

    void Command::addArg(const std::string& arg) { _args.addArg(arg); }

    void Command::runCommand() {
        switch (_type) {
            case commandType::EXEC: {
                _execCommand();  // shouldn't return
                break;
            }
            case commandType::BACKGROUND: {
                // we double fork here, letting system deal with lifetime of process
                int const pid = forkCheckErrors();
                if (pid == 0) { _execCommand(); }
                break;
            }
            case commandType::BUILT_IN: {
                _execBuiltin();
                break;
            }
        }
    }
    void Command::_execBuiltin() {
        // for right now use if statements
        // find a better way
        std::string const name = _args.getArgs()[0];
        if (name == "scan") {
            builtins::scan(_args.getArgs());
        } else if (name == "print" || name == "println") {
            builtins::print(_args.getArgs());
        }
    }
    void Command::_execCommand() {
        execvp(_name.c_str(), _args.getCArgs());
        throw util::CommanderException("Job Runner: error trying to exec command");
    }

    JobInfo Command::runCommandSave() {
        // create two pipes; one for stdout and another for stderr
        int stdoutPipe[2];
        int stderrPipe[2];
        pipe2(stdoutPipe, O_CLOEXEC);
        pipe2(stderrPipe, O_CLOEXEC);

        int const processID = forkCheckErrors();
        if (processID == 0) {
            close(stdoutPipe[0]);
            close(stderrPipe[0]);

            dup2(stdoutPipe[1], STDOUT_FILENO);
            dup2(stderrPipe[1], STDERR_FILENO);

            close(stdoutPipe[1]);
            close(stderrPipe[1]);

            _execCommand();
        }

        // close write end of pipes since we won't need them
        close(stdoutPipe[1]);
        close(stderrPipe[1]);

        // bufferSizeOut
        int bufferSizeOut = 4096;
        int bufferSizeErr = 4096;

        // we will save output here
        char* stdoutBuffer = new char[bufferSizeOut];
        char* stderrBuffer = new char[bufferSizeErr];

        bool stillReading = true;

        // size of our buffers so far
        size_t stderrSize = 0;
        size_t stdoutSize = 0;
        // count how many bytes read from read() call
        size_t stderrRead;
        size_t stdoutRead;

        while (stillReading) {
            stdoutRead = read(stdoutPipe[0], &stdoutBuffer[stdoutSize], bufferSizeOut - stdoutSize);
            stderrRead = read(stderrPipe[0], &stderrBuffer[stderrSize], bufferSizeErr - stderrSize);

            if (stdoutRead == 0 && stderrRead == 0) stillReading = false;

            stderrSize += stderrRead;
            stdoutSize += stdoutRead;

            if (stdoutSize >= bufferSizeOut) {
                resizeArrayHelper(&stdoutBuffer, bufferSizeOut);
                bufferSizeOut *= 2;
            }
            if (stderrSize >= bufferSizeErr) {
                resizeArrayHelper(&stderrBuffer, bufferSizeErr);
                bufferSizeErr *= 2;
            }
        }

        // we are done reading, so close read end of pipes
        close(stdoutPipe[0]);
        close(stderrPipe[0]);

        // null terminate our char arrays
        if (stderrSize >= bufferSizeErr) resizeArrayHelper(&stderrBuffer, stderrSize);
        stderrBuffer[stderrSize] = '\0';

        if (stdoutSize >= bufferSizeOut) resizeArrayHelper(&stdoutBuffer, stdoutSize);
        stdoutBuffer[stdoutSize] = '\0';

        // we want stats process
        int stat;
        waitpid(processID, &stat, 0);

        std::string const stdoutString(stdoutBuffer);
        std::string const stderrString(stderrBuffer);

        return {stdoutString, stderrString, WEXITSTATUS(stat)};
    }

    /*
     * PipeCommands Class
     */
    JobInfo PipeCommands::_runPipeHelper() {
        JobInfo result;

        size_t const numOfCommands = _pipeline.size();
        size_t const numOfFileDescriptors = (numOfCommands - 1) * 2;

        // an array of pipes for the pipeline
        int pipes[numOfFileDescriptors];

        // keep track of indices of previous/current pipes
        int prevPipeIndices[2] = {0, 1};
        int currPipeIndices[2] = {0, 1};

        // initialize our pipes
        for (int i = 0; i < numOfFileDescriptors; i += 2) pipe2(&pipes[i], O_CLOEXEC);

        for (int i = 0; i < numOfCommands; i++) {
            int const pid = forkCheckErrors();
            if (pid == 0) {
                if (i == 0) {
                    // dup just write end for first command
                    dup2(pipes[currPipeIndices[1]], STDOUT_FILENO);
                } else if (i == (numOfCommands - 1)) {
                    // dup just read end for last command
                    dup2(pipes[prevPipeIndices[0]], STDIN_FILENO);
                } else {
                    // read from prevPipe and write to current pipe
                    dup2(pipes[prevPipeIndices[0]], STDIN_FILENO);
                    dup2(pipes[currPipeIndices[1]], STDOUT_FILENO);
                }

                // close open pipes
                for (int j = 0; j < numOfFileDescriptors; j++) close(pipes[j]);

                _pipeline[i]->runCommand();  // shouldn't run a background process here
            }

            // set previous pipe indices and increment current pipe indices
            prevPipeIndices[0] = currPipeIndices[0];
            prevPipeIndices[1] = currPipeIndices[1];
            currPipeIndices[0] += 2;
            currPipeIndices[1] += 2;

        }  // for

        // close open pipes in parent
        for (int j = 0; j < numOfFileDescriptors; j++) close(pipes[j]);
        // wait for all processes
        for (int i = 0; i < numOfCommands; i++) wait(nullptr);

        return result;
    }

    void PipeCommands::addCommand(Command* command) { _pipeline.emplace_back(command); }

    JobInfo PipeCommands::runPipeLine(bool save) {
        if (save) {
            if (_pipeline.size() == 1) return _pipeline[0]->runCommandSave();
            return {};
        }

        if (_pipeline.size() == 1) {
            int const pid = forkCheckErrors();
            if (pid == 0) _pipeline[0]->runCommand();
            wait(nullptr);
            return {};
        }
        // pipeline is greater than one so run the helper
        return _runPipeHelper();
    }
    /*
     * Job Class
     */
    JobInfo Job::runJob() { return _pipeline.runPipeLine(_save); }

    void Job::addCommandToPipeline(Command* command) { _pipeline.addCommand(command); }

    void Job::setJobToSave(bool save) { _save = save; }

    /*
     * Helper Methods
     */
    int forkCheckErrors() {
        int const processID = fork();
        if (processID < 0) { throw util::CommanderException("Job Runner: error trying to fork"); }
        return processID;
    }

    void resizeArrayHelper(char** arr, size_t currentSize) {
        char* newArr = new char[currentSize * 2];
        memcpy(newArr, *arr, currentSize * sizeof(char));
        delete[] *arr;
        *arr = newArr;
    }

}  // namespace jobRunner
