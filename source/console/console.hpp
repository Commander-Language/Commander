/**
 * @file console.hpp
 * @brief Defines lots of utilities related to the console.
 * @details Implementation requires interfacing with platform-specific libraries;
 *          as such, there is a different implementation file for each platform.
 */

#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

/**
 * @brief Defines lots of utilities related to the console.
 */
class Console {
public:
    /**
     * @brief A single character typed from the user.
     * @details Encapsulates special characters such as <Ctrl-C>, <Backspace>, and more.
     */
    struct Character {
        /**
         * @brief An enumeration of all different character types.
         */
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

        /**
         * @brief The type of this character.
         */
        CharacterType type;

        /**
         * @brief The `char` value of this character.
         */
        char value;
    };

    /**
     * @brief Gets a single input character from the console.
     *
     * @return A single input character.
     */
    static Character getChar();

    /**
     * @brief Reports the size of the console window.
     *
     * @return The size of the console window in format {number of rows, number of columns}.
     */
    static std::tuple<unsigned int, unsigned int> getConsoleSize();

    /**
     * @brief Reports the position of the cursor in the console window.
     *
     * @return The position of the cursor in the console window in format {row, column}.
     */
    static std::tuple<unsigned int, unsigned int> getCursorPosition();

    /**
     * @brief Sets the position of the cursor in the console window.
     *
     * @param row The cursor's row position in the console window.
     * @param col The cursor's column position in the console window.
     */
    static void setCursorPosition(unsigned int row, unsigned int col);

    /**
     * @brief Moves the cursor up or down by the given number of rows.
     *
     * @param offset How many rows to move up (positive) or down (negative).
     */
    static void moveCursorVertical(int offset);

    /**
     * @brief Moves the cursor left or right by the given number of columns.
     *
     * @param offset How many columns to move left (negative) or right (positive).
     */
    static void moveCursorHorizontal(int offset);

    /**
     * @brief Clears the current line of the console window.
     */
    static void clearLine();

    /**
     * @brief Clears the console screen.
     */
    static void clearScreen();

    /**
     * @brief Reads a line of text from stdin.
     * @details Supports arrows, backspace, etc. Compare to GNU's `readline`.
     *
     * @param prompt The prompt to display to the user.
     * @param history A history to look through. Defaults to empty.
     * @return A line read from stdin.
     */
    static std::string readLine(const std::string& prompt, const std::vector<std::string>& history = {});
};

#endif  // CONSOLE_HPP
