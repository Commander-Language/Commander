/**
 * @file file_position.hpp
 * @brief Contains header content for file_position.cpp
 *
 */

#ifndef COMMANDER_FILE_POSITION_HPP
#define COMMANDER_FILE_POSITION_HPP

#include <string>

namespace Lexer {
    /**
     * @brief Represents a position in a file
     */
    struct FilePosition {
        /**
         * The file name and path
         */
        std::string fileName;

        /**
         * The line position in the file
         */
        int line;

        /**
         * The column position in the file
         */
        int column;

        /**
         * The index position in the file
         */
        int index;

        /**
         * @brief Returns the string representation of the FilePosition
         * @return The string representation of the FilePosition
         */
        [[nodiscard]] std::string toString() const;
    };
}  // namespace Lexer

#endif  // COMMANDER_FILE_POSITION_HPP