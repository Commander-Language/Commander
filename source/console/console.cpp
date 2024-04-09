/**
 * @file console.cpp
 * @brief Implementation of the `Console` class for methods that are shared between systems.
 */

#include "console.hpp"

#include <iostream>
#include <string>

std::string Console::readLine(const std::string& prompt, const std::vector<std::string>& history) {
    std::cout << prompt;

    std::string displayedLine;
    std::string lastEditedLine;  // <-- So that <Arrow-up> and then <Arrow-down> doesn't delete what you were typing.
    std::size_t historyNum = 0;
    unsigned int position = 0;  // <-- Current index into the line. (Changed with, e.g., <Arrow-left> and <Arrow-right>)

    // If we delete characters, then we need to re-draw the whole line.
    const auto refreshLine = [&] {
        clearLine();
        std::cout << prompt << displayedLine;
        moveCursorHorizontal(static_cast<int>(position) - static_cast<int>(displayedLine.size()));
    };

    while (true) {
        auto [charType, charValue] = getChar();

        switch (charType) {
            // ===================
            // ||  Arrow keys:  ||
            // ===================
            case Character::ARROW_UP:
                if (historyNum < history.size()) {
                    ++historyNum;
                    displayedLine = history[history.size() - historyNum];
                    position = displayedLine.size();
                    refreshLine();
                }
                break;
            case Character::ARROW_DOWN:
                if (historyNum == 0) break;

                if (historyNum > 1) {
                    --historyNum;
                    displayedLine = history[history.size() - historyNum];
                    position = displayedLine.size();
                } else if (historyNum == 1) {
                    --historyNum;
                    displayedLine = lastEditedLine;
                    position = displayedLine.size();
                }

                refreshLine();
                break;
            case Character::ARROW_LEFT:
                if (position == 0) break;
                --position;
                moveCursorHorizontal(-1);
                break;
            case Character::ARROW_RIGHT:
                if (position == displayedLine.size()) break;
                ++position;
                moveCursorHorizontal(1);
                break;

            // =============================
            // ||  Backspace and delete:  ||
            // =============================
            case Character::BACKSPACE:
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
            case Character::DELETE:
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
            case Character::CTRL_C:
            case Character::CTRL_D:
                std::cout << "\n";
                return "exit";

            case Character::CTRL_L:
                clearScreen();
                refreshLine();
                break;

            // ================
            // ||  Newline:  ||
            // ================
            case Character::NEWLINE:
                std::cout << "\n";
                return displayedLine;

            // ========================
            // ||  All other chars:  ||
            // ========================
            case Character::UTF8:
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
