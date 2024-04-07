#include "process.hpp"
namespace Process {

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

}  // namespace Process