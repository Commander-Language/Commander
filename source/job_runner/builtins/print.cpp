/**
 * @file print.cpp
 * @brief Implements the print and println built-in commands
 *
 */

#include "print.hpp"
#include "shared.hpp"
#include <unistd.h>

ReturnInfo print(const List& args, int in, int out) {
    if (args.size() == 1) { return {"", "", SUCCESS}; }

    std::string output;
    if (args[0] == "print") {
        for (int i = 1; i < args.size(); i++) { output.append(args[i]); }

        if (write(out, output.c_str(), output.size()) < 0) {
            // TODO: figure out error messages
            return {"", "", ERROR};
        }

        return {output, "", SUCCESS};
    }
    if (args[0] == "println") {
        for (int i = 1; i < args.size(); i++) { output.append(args[i]); }
        output.append("\n");
        if (write(out, output.c_str(), output.size()) < 0) {
            // TODO: figure out error messages
            return {"", "", ERROR};
        }
        return {output, "", SUCCESS};
    }

    return {"", "", ERROR};
}