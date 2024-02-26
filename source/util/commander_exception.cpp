/**
 * @file commander_exception.cpp
 * @brief Contains implementation for functions in commander_exception.hpp
 *
 */
#include "commander_exception.hpp"

#include <utility>

namespace Util {
    CommanderException::CommanderException(std::string  message) : _errorMessage(std::move(message)) {}
    CommanderException::CommanderException(const std::string& message, const Lexer::FilePosition& position)
        : _errorMessage(message + "\n" + position.toString()) {}
    const char* CommanderException::what() const noexcept { return _errorMessage.c_str(); }
}  // namespace Util