/**
 * @file builtins.hpp
 * @brief builtin header file
 * @details Still needs work just a quick implementation of some builtins
 */
#ifndef COMMANDER_BUILTIN_HPP
#define COMMANDER_BUILTIN_HPP

#include "shared.hpp"
#include "print.hpp"

#include <string>
#include <vector>

namespace Builtins {

    using Function = List (*)(std::vector<std::string>);

    inline Function getBuiltinFunction(const std::string& name){

        if(name == "print")
            return Print::print;

        return nullptr;
    }
}  // namespace Builtins
#endif
