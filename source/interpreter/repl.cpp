/**
 * @file repl.hpp
 * @brief Implements the `REPL` class for the commander REPL environment.
 * @details See the header file for documentation.
 */

#include "repl.hpp"

#include "console.hpp"

#include "source/util/commander_exception.hpp"

#include <fstream>
#include <iostream>

REPL::REPL(const std::function<void(const std::string&)>& interpretFunc) : _interpretFunc(interpretFunc) {}

void REPL::run() {
    std::cout << "Commander Language Version 1.0\n"
              << "Basic REPL for Commander scripting language\n";

    while (true) {
        std::string line = _readLine();

        if (line == "quit" || line == "exit") break;

        if (line == "clear") {
            Console::clearScreen();
        } else
            try {
                // Make a temporary file for the lexer.
                const std::string tmpFileName = std::tmpnam(nullptr);  // NOLINT(*-mt-unsafe)
                std::ofstream tmp(tmpFileName);
                tmp << line;
                tmp.close();  // Close file here to save changes

                _interpretFunc(tmpFileName);

                if (std::remove(tmpFileName.c_str()) != 0) {  // NOLINT(*-mt-unsafe)
                    throw Util::CommanderException("I/O error: could not delete temporary file");
                }
            } catch (const Util::CommanderException& err) { std::cerr << err.what() << "\n"; }
    }
}

std::string REPL::_readLine() {
    const std::string prompt = ">>  ";
    std::cout << prompt;

    std::string line;
    std::string lineCopy;  // In case we come back to the line after looking through history.
    std::size_t historyNum = 0;
    unsigned int position = 0;

    const auto refreshLine = [&] {
        Console::clearLine();
        std::cout << prompt << line;
        Console::moveCursorHorizontal(static_cast<int>(position) - static_cast<int>(line.size()));
    };

    while (true) {
        auto [charType, charContents] = Console::getChar();

        switch (charType) {
            case Console::Character::ARROW_UP:
                if (historyNum < _history.size()) {
                    ++historyNum;
                    line = _history[_history.size() - historyNum];
                    position = line.size();
                    refreshLine();
                }
                break;
            case Console::Character::ARROW_DOWN:
                if (historyNum > 1) {
                    --historyNum;
                    line = _history[_history.size() - historyNum];
                    position = line.size();
                    refreshLine();
                    break;
                }
                if (historyNum == 1) {
                    --historyNum;
                    line = lineCopy;
                    position = line.size();
                    refreshLine();
                    break;
                }
                break;
            case Console::Character::ARROW_LEFT:
                --position;
                Console::moveCursorHorizontal(-1);
                break;
            case Console::Character::ARROW_RIGHT:
                ++position;
                Console::moveCursorHorizontal(1);
                break;

            case Console::Character::BACKSPACE:
                if (position > 0) {
                    if (position == 1) {
                        line = line.substr(1);
                    } else {
                        line = line.substr(0, position - 1) + line.substr(position);
                    }
                    --position;
                    historyNum = 0;
                    lineCopy = line;
                    refreshLine();
                }
                break;

            case Console::Character::DELETE:
                if (position < line.size()) {
                    if (position == line.size() - 1) {
                        line = line.substr(0, position);
                    } else {
                        line = line.substr(0, position) + line.substr(position + 1);
                    }
                    historyNum = 0;
                    lineCopy = line;
                    refreshLine();
                }
                break;

            case Console::Character::CTRL_C:
            case Console::Character::CTRL_D:
                std::cout << "\n";
                exit(0);  // NOLINT(*-mt-unsafe)

            case Console::Character::CTRL_L:
                Console::clearScreen();
                refreshLine();
                break;

            case Console::Character::UTF8:
                if (position++ == line.size()) {
                    line += charContents;
                    std::cout << charContents;
                } else {
                    line.insert(position - 1, 1, charContents);
                    refreshLine();
                }
                historyNum = 0;
                lineCopy = line;
                break;

            case Console::Character::NEWLINE:
                std::cout << "\n";
                _history.push_back(line);
                return line;
        }
    }
}
