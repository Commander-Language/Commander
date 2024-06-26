/**
 * @file builtins.hpp
 * @brief Organize built-ins to call on later.
 * @details Save builtins in a map to be able to call on when needed.
 *          If it doesn't find the builtin, throw a commander error.
 *
 *          Builtins should take three parameters:
 *              1) List of string arguments
 *              2) input file descriptor
 *              3) output file descriptor
 *
 *          Notes:
 *              The first argument of the builtin command should be the name.
 *              The file descriptors are to be used when reading or writing (used for piping/redirection/save)
 *
 */
#ifndef COMMANDER_BUILTIN_HPP
#define COMMANDER_BUILTIN_HPP

#include "print.hpp"
#include "shared.hpp"
#include "source/util/commander_exception.hpp"
#include <string>
#include <unordered_map>

namespace Builtins {
    /**
     * @breif structure of builtins
     */
    using Function = ReturnInfo (*)(const List&, int, int);

    Function getBuiltinFunction(const std::string& name);
}  // namespace Builtins
#endif
