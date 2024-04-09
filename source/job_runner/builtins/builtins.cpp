/**
 * @file builtins.cpp
 * @brief Implementation of builtins.hpp
 *
 */

#include "builtins.hpp"

namespace Builtins {

    std::unordered_map<std::string, Function> builtins = {};

    Function getBuiltinFunction(const std::string& name) {
        if (builtins.find(name) == builtins.end()) {
            throw Util::CommanderException("Builtin Error: Unknown builtin " + name);
        }
        return builtins[name];
    }

}  // namespace Builtins
