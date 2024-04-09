/**
 * @file console-windows.cpp
 * @brief Implementation of the `Console` class for Windows systems.
 */

#include "console.hpp"

#include "source/util/commander_exception.hpp"

#include <iostream>

#include <conio.h>
#include <io.h>
#include <windows.h>

Console::Character Console::getChar() {
    std::cout << std::flush;
    int val = 0;
    while (val == 0) val = _getch();

    if (val == '\003') return {Character::CTRL_C, (char)val};
    if (val == '\004') return {Character::CTRL_D, (char)val};
    if (val == '\010') return {Character::BACKSPACE, (char)val};
    if (val == '\014') return {Character::CTRL_L, (char)val};
    if (val == '\015') return {Character::NEWLINE, (char)val};

    if (val == 224) {
        // Escape key
        val = _getch();
        if (val == 'H') return {Character::ARROW_UP, (char)val};
        if (val == 'K') return {Character::ARROW_LEFT, (char)val};
        if (val == 'M') return {Character::ARROW_RIGHT, (char)val};
        if (val == 'P') return {Character::ARROW_DOWN, (char)val};
    }

    return {Character::UTF8, (char)val};
}

std::tuple<unsigned int, unsigned int> Console::getConsoleSize() {
    CONSOLE_SCREEN_BUFFER_INFO term;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &term);
    return {term.srWindow.Bottom - term.srWindow.Top + 1, term.srWindow.Right - term.srWindow.Left + 1};
}

std::tuple<unsigned int, unsigned int> Console::getCursorPosition() {
    CONSOLE_SCREEN_BUFFER_INFO term;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &term);
    return {term.dwCursorPosition.Y - term.srWindow.Top, term.dwCursorPosition.X - term.srWindow.Left};
}

void Console::setCursorPosition(const unsigned int row, const unsigned int col) {
    CONSOLE_SCREEN_BUFFER_INFO term;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &term);
    term.dwCursorPosition.Y = (SHORT)row + term.srWindow.Top;
    term.dwCursorPosition.X = (SHORT)col + term.srWindow.Left;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), term.dwCursorPosition);
}

void Console::moveCursorVertical(const int offset) {
    CONSOLE_SCREEN_BUFFER_INFO term;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &term);
    term.dwCursorPosition.Y += (SHORT)offset;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), term.dwCursorPosition);
}

void Console::moveCursorHorizontal(const int offset) {
    CONSOLE_SCREEN_BUFFER_INFO term;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &term);
    term.dwCursorPosition.X += (SHORT)offset;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), term.dwCursorPosition);
}

void Console::clearLine() {
    const auto [rows, cols] = getConsoleSize();
    const auto [curRow, curCol] = getCursorPosition();
    setCursorPosition(curRow, 0);
    std::cout << std::string(cols, ' ');
    setCursorPosition(curRow, 0);
}

void Console::clearScreen() {
    // TODO: There's probably an escape sequence for this.
    system("cls");
}
