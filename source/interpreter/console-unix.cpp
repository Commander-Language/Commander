#include "console.hpp"

#include "source/util/commander_exception.hpp"

#include <array>
#include <iostream>

#include <termio.h>
#include <unistd.h>

Console::Character Console::getChar() {
    // Source: https://github.com/antirez/linenoise/blob/master/linenoise.c
    constexpr int stdinFD = 0;

    std::cout << std::flush;

    struct TermiosGuard {
        termios oldTerminal {};

        TermiosGuard() {
            if (tcgetattr(stdinFD, &oldTerminal) < 0) throw Util::CommanderException("`tcsetattr` error");
        }

        ~TermiosGuard() { tcsetattr(stdinFD, TCSANOW, &oldTerminal); }
    };

    const TermiosGuard termGuard;

    termios curTerminal = termGuard.oldTerminal;
    curTerminal.c_lflag &= ~(ICANON | ECHO | ISIG);  // echoing off, canonical off, no signal chars
    curTerminal.c_cc[VMIN] = 1;
    curTerminal.c_cc[VTIME] = 0;

    if (tcsetattr(stdinFD, TCSANOW, &curTerminal) < 0) { throw Util::CommanderException("`tcsetattr` error"); }

    char curChar;
    const auto readChar = [&] {
        if (read(stdinFD, &curChar, 1) < 0) { throw Util::CommanderException("`read` error"); }
    };

    readChar();

    if (curChar == '\033') {
        readChar();

        if (curChar == '[') {
            readChar();

            if (curChar == 'A') return {Character::ARROW_UP, curChar};
            if (curChar == 'B') return {Character::ARROW_DOWN, curChar};
            if (curChar == 'C') return {Character::ARROW_RIGHT, curChar};
            if (curChar == 'D') return {Character::ARROW_LEFT, curChar};

            if (curChar == '3') {
                readChar();

                if (curChar == '~') return {Character::DELETE, curChar};
            }
        }
    }

    constexpr char ctrlOffset = 0x40;
    if (curChar + ctrlOffset == 'C') return {Character::CTRL_C, curChar};
    if (curChar + ctrlOffset == 'D') return {Character::CTRL_D, curChar};
    if (curChar + ctrlOffset == 'L') return {Character::CTRL_L, curChar};

    // std::cout << "[" << static_cast<int>(curChar) << "]";
    if (curChar == '\177') return {Character::BACKSPACE, curChar};

    if (curChar == '\n') return {Character::NEWLINE, curChar};

    return {Character::UTF8, curChar};
}

std::tuple<unsigned int, unsigned int> Console::getConsoleSize() {
    // Source: https://stackoverflow.com/questions/23369503/get-size-of-terminal-window-rows-columns
    winsize window {};
    ioctl(1, TIOCGWINSZ, &window);
    return {window.ws_row, window.ws_col};
}

std::tuple<unsigned int, unsigned int> Console::getCursorPosition() {
    constexpr unsigned int bufferSize = 32;

    std::array<char, bufferSize> buffer {};

    std::cout << "\033[6n";

    for (int ind = 0; ind < bufferSize; ++ind) {
        buffer.at(ind) = getChar().utf8Contents;
        if (buffer.at(ind) == 'R') break;
    }
    buffer[bufferSize - 1] = 0;

    int row;
    int col;
    if (std::scanf("\033[%d;%dR", &row, &col) != 2) {  // NOLINT(*-err34-c)
        throw Util::CommanderException("Error: could not parse cursor position");
    }

    return std::make_tuple(row - 1, col - 1);
}

void Console::setCursorPosition(const unsigned int row, const unsigned int col) {
    std::cout << "\033[" << row + 1 << ";" << col + 1 << "H";
}

void Console::moveCursorVertical(const int offset) {
    if (offset > 0) {
        std::cout << "\033[" << offset << "B";
    } else if (offset < 0) {
        std::cout << "\033[" << -offset << "A";
    }
}

void Console::moveCursorHorizontal(const int offset) {
    if (offset > 0) {
        std::cout << "\033[" << offset << "C";
    } else if (offset < 0) {
        std::cout << "\033[" << -offset << "D";
    }
}

void Console::clearLine() {
    const auto [rows, cols] = getConsoleSize();
    std::cout << "\r" << std::string(cols, ' ') << "\r";
}

void Console::clearScreen() { system("clear"); }
