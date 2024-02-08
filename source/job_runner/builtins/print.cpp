/**
 * @file scan.hpp
 * @brief scan builtin header file
 */
#include "print.hpp"
#include "shared.hpp"
#include <iostream>

namespace Print {
    ReturnInfo print(const List& args) {
        if (args.size() == 1) { return {"", args[0] + ": No arguments given", FAIL}; }

        std::string output;
        if (args[0] == "print") {
            for (int i = 1; i < args.size(); i++) { output.append(args[i]); }
            std::cout << output;

            return {output, "", SUCCESS};
        }
        if (args[0] == "println") {
            for (int i = 1; i < args.size(); i++) { output.append(args[i]); }
            std::cout << output << "\n";
            return {output, "", SUCCESS};
        }

        return {"", "", ERROR};
    }
}  // namespace Print
