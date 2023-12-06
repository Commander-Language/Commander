/**
 * @file scan.hpp
 * @brief scan builtin header file
 * @details Still needs work just a quick implementation of some builtins
 */
#ifndef COMMANDER_BUILTIN_HPP
#define COMMANDER_BUILTIN_HPP
#include <string>
#include <vector>
namespace builtins {

    /**
     * @brief Print something to the terminal
     * @param args List of arguments
     */
    void print(std::vector<std::string> args);

    /**
     * @brief Scan for user input
     * @param args List of arguments
     * @return A string from user input
     */
    std::string scan(std::vector<std::string> args);
}  // namespace builtins
#endif