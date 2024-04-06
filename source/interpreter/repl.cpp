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

        // If the line is empty, just ignore it.
        if (line.empty()) continue;

        // Special built-in command: "quit" or "exit" exits the REPL.
        if (line == "quit" || line == "exit") break;

        // Special built-in command: "clear" to clear the screen.
        if (line == "clear") {
            Console::clearScreen();
        } else
            try {
                _interpretFunc(line);
            } catch (const Util::CommanderException& err) { std::cerr << err.what() << "\n"; }
    }
}

std::string REPL::_readLine() {
    const std::string prompt = ">> ";
    std::cout << prompt;

    std::string displayedLine;
    std::string lastEditedLine;  // <-- So that <Arrow-up> and then <Arrow-down> doesn't delete what you were typing.
    std::size_t historyNum = 0;
    unsigned int position = 0;  // <-- Current index into the line. (Changed with, e.g., <Arrow-left> and <Arrow-right>)

    // If we delete characters, then we need to re-draw the whole line.
    const auto refreshLine = [&] {
        Console::clearLine();
        std::cout << prompt << displayedLine;
        Console::moveCursorHorizontal(static_cast<int>(position) - static_cast<int>(displayedLine.size()));
    };

    while (true) {
        auto [charType, charValue] = Console::getChar();

        switch (charType) {
            // ===================
            // ||  Arrow keys:  ||
            // ===================
            case Console::Character::ARROW_UP:
                if (historyNum < _history.size()) {
                    ++historyNum;
                    displayedLine = _history[_history.size() - historyNum];
                    position = displayedLine.size();
                    refreshLine();
                }
                break;
            case Console::Character::ARROW_DOWN:
                if (historyNum == 0) break;

                if (historyNum > 1) {
                    --historyNum;
                    displayedLine = _history[_history.size() - historyNum];
                    position = displayedLine.size();
                } else if (historyNum == 1) {
                    --historyNum;
                    displayedLine = lastEditedLine;
                    position = displayedLine.size();
                }

                refreshLine();
                break;
            case Console::Character::ARROW_LEFT:
                --position;
                Console::moveCursorHorizontal(-1);
                break;
            case Console::Character::ARROW_RIGHT:
                ++position;
                Console::moveCursorHorizontal(1);
                break;

            // =============================
            // ||  Backspace and delete:  ||
            // =============================
            case Console::Character::BACKSPACE:
                if (position > 0) {
                    if (position == 1) {
                        displayedLine = displayedLine.substr(1);
                    } else {
                        displayedLine = displayedLine.substr(0, position - 1) + displayedLine.substr(position);
                    }
                    --position;
                    historyNum = 0;
                    lastEditedLine = displayedLine;
                    // TODO: Don't redraw the whole line for deleting one character.
                    refreshLine();
                }
                break;
            case Console::Character::DELETE:
                if (position < displayedLine.size()) {
                    if (position == displayedLine.size() - 1) {
                        displayedLine = displayedLine.substr(0, position);
                    } else {
                        displayedLine = displayedLine.substr(0, position) + displayedLine.substr(position + 1);
                    }
                    historyNum = 0;
                    lastEditedLine = displayedLine;
                    // TODO: Don't redraw the whole line for deleting one character.
                    refreshLine();
                }
                break;

            // =====================
            // ||  <Ctrl>-<Key>:  ||
            // =====================
            case Console::Character::CTRL_C:
            case Console::Character::CTRL_D:
                std::cout << "\n";
                return "exit";

            case Console::Character::CTRL_L:
                Console::clearScreen();
                refreshLine();
                break;

            // ================
            // ||  Newline:  ||
            // ================
            case Console::Character::NEWLINE:
                std::cout << "\n";
                _history.push_back(displayedLine);
                return displayedLine;

            // ========================
            // ||  All other chars:  ||
            // ========================
            case Console::Character::UTF8:
                if (position++ == displayedLine.size()) {
                    displayedLine += charValue;
                    std::cout << charValue;
                } else {
                    displayedLine.insert(position - 1, 1, charValue);
                    // TODO: Don't redraw the whole line for inserting one character.
                    refreshLine();
                }
                historyNum = 0;
                lastEditedLine = displayedLine;
                break;
        }
    }
}
