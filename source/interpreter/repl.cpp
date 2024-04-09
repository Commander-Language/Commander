/**
 * @file repl.hpp
 * @brief Implements the `REPL` class for the commander REPL environment.
 * @details See the header file for documentation.
 */

#include "repl.hpp"

#include "source/console/console.hpp"

#include "source/util/commander_exception.hpp"

#include <fstream>
#include <iostream>

REPL::REPL(const std::function<void(const std::string&)>& interpretFunc) : _interpretFunc(interpretFunc) {}

void REPL::run() {
    std::cout << "Commander Language Version 1.0\n"
              << "Basic REPL for Commander scripting language\n";

    const std::string prompt = ">> ";

    while (true) {
        std::string line = Console::readLine(prompt, _history);

        // If the line is empty, just ignore it.
        if (line.empty()) continue;

        // Special built-in command: "quit" or "exit" exits the REPL.
        if (line == "quit" || line == "exit") break;

        std::cout << "\"" << line << "\"\n";

        _history.push_back(line);

        // Special built-in command: "clear" to clear the screen.
        if (line == "clear") {
            Console::clearScreen();
        } else
            try {
                _interpretFunc(line);
            } catch (const Util::CommanderException& err) { std::cerr << err.what() << "\n"; }
    }
}
