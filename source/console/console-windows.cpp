/**
 * @file console-windows.cpp
 * @brief Implementation of the `Console` class for Windows systems.
 */

#include "console.hpp"

#include "source/util/commander_exception.hpp"

#include <array>
#include <iostream>

Console::Character Console::getChar() {
// TODO
}

std::tuple<unsigned int, unsigned int> Console::getConsoleSize() {
// TODO
}

std::tuple<unsigned int, unsigned int> Console::getCursorPosition() {
// TODO
}

void Console::setCursorPosition(const unsigned int row, const unsigned int col) {
// TODO
}

void Console::moveCursorVertical(const int offset) {
// TODO
}

void Console::moveCursorHorizontal(const int offset) {
// TODO
}

void Console::clearLine() {
// TODO
}

void Console::clearScreen() {
// TODO
}
