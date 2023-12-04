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
    /*
     * CommandArgs Class
     */
    void CommandArgs::addArg(const std::string& arg) { _args.emplace_back(arg); }

    char** CommandArgs::getArgs() {
        _cargs.reserve(_args.size());
        for (auto& arg : _args) _cargs.emplace_back(arg.data());
        return _cargs.data();
    }

}  // namespace jobRunner
