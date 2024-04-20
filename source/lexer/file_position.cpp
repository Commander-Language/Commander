/**
 * @file file_position.cpp
 * @brief Contains implementation for functions in file_position.hpp
 *
 */

#include "file_position.hpp"

namespace Lexer {
    std::string FilePosition::toString() const {
        return fileName + ":" + std::to_string(line) + ":" + std::to_string(column);
    }
}  // namespace Lexer