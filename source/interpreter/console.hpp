#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <cstdint>
#include <tuple>

class Console {
public:
    struct Character {
        enum CharacterType : std::uint8_t {
            ARROW_UP,
            ARROW_DOWN,
            ARROW_LEFT,
            ARROW_RIGHT,

            BACKSPACE,
            DELETE,
            CTRL_C,
            CTRL_D,
            CTRL_L,

            UTF8,
            NEWLINE
        };

        CharacterType type;

        char utf8Contents;
    };

    static Character getChar();

    static std::tuple<unsigned int, unsigned int> getConsoleSize();

    static std::tuple<unsigned int, unsigned int> getCursorPosition();

    static void setCursorPosition(unsigned int row, unsigned int col);

    static void moveCursorVertical(int offset);

    static void moveCursorHorizontal(int offset);

    static void clearLine();

    static void clearScreen();
};

#endif  // CONSOLE_HPP
