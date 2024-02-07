/**
 * @file scan.hpp
 * @brief scan builtin header file
 */
#include "print.hpp"
#include <iostream>

namespace Print {
    List print(List args) {
        if (args.size() <= 1) {
            std::cout << args[0] << ": no argument given\n";
        }
        if (args[0] == "print") {
            for (int i = 1; i < args.size(); i++) { std::cout << args[i]; }
        }
        if (args[0] == "println") {
            for (int i = 1; i < args.size(); i++) { std::cout << args[i]; }
            std::cout << "\n";
        }

        return {};
    }
}  // namespace Print
