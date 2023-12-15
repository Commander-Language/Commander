/**
 * @file scan.hpp
 * @brief scan builtin header file
 */
#include "builtins.hpp"
#include <iostream>
#include <stdlib.h>

namespace builtins {
    void print(std::vector<std::string> args) {
        if (args.size() <= 1) {
            std::cout << args[0] << ": no argument given\n";
            exit(1);
        }
        if (args[0] == "print") {
            for (int i = 1; i < args.size(); i++) { std::cout << args[i]; }
        }
        if (args[0] == "println") {
            for (int i = 1; i < args.size(); i++) { std::cout << args[i]; }
            std::cout << "\n";
        }
        // only works with non-thread safe exit
        exit(0);
    }
}  // namespace builtins