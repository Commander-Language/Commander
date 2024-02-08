/**
 * @file job_runner.hpp
 * @brief Job Runner Implementation
 */
#include "job_runner.hpp"
#include "builtins/builtins.hpp"
#include "source/util/commander_exception.hpp"
#include <cstdlib>
#include <cstring>

/* Unix/Mac specific includes */
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
/* Windows specific includes */
// #include <Windows.h>

namespace JobRunner {

    /**
     * @details Only call this at the start of a pipe or for commands not part of a pipe.
     */
    JobInfo execProcess(Process* process) {
        switch (process->getType()) {
            case JobRunner::ProcessType::BUILTIN: {
                if (process->pipe != nullptr) { return doPiping(process); }
                if (process->background) { return doBackground(process); }
                return execBuiltin(process);
            }
            case JobRunner::ProcessType::EXTERNAL: {
                if (process->pipe != nullptr) { return doPiping(process); }
                if (process->background) { return doBackground(process); }
                return execFork(process);
            }
            default:
                return {};
        }
    }


    /**
     * @details for builtin commands
     */
    JobInfo execBuiltin(Process* process) { 
        // get the function so we can call it!
        auto builtin = Builtins::getBuiltinFunction(process->getProcess());
        return builtin(process->args);
    }

    /**
     * @details for external commands
     */
    JobInfo execFork(Process*) { 
        return {}; }
    void execNoFork(Process*) {}

    /**
     * @details should work with any order of builtin and external types
     */
    JobInfo doPiping(Process* process) { 
        return {}; 
    }

    /**
     * @details should work with either external or builtin
     *          we also shouldn't be calling this in a pipe
     */
    JobInfo doBackground(Process* process) { return {}; }
    
    /**
     * @details should work for 
     */
    JobInfo doSaveInfo(Process* process, bool partOfPipe){

        return {};
    }

    std::string getOutputFromFD(){

        return "";
    }

}  // namespace JobRunner
