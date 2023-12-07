/**
 * @file scan.cpp
 * @brief scan builtin implementation
 */
#include "builtins.hpp"
#include <iostream>
namespace builtins {

    std::string scan(std::vector<std::string> args) {
        if (args.size() <= 1) {
            std::cout << "scan: no arguments given\n";
            return {};
        }

        std::string input;
        std::cout << args[1];
        std::cin >> input;
        return input;
    }

}  // namespace builtins