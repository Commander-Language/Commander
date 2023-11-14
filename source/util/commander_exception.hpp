/**
 * @file commander_exception.cpp
 * @brief Contains implementation for functions in commander_exception.hpp
 *
 */

#ifndef COMMANDER_COMMANDER_EXCEPTION_HPP
#define COMMANDER_COMMANDER_EXCEPTION_HPP

#include "../lexer/lexer.hpp"
#include <string>
#include <utility>

namespace util {

    class CommanderException : std::exception {
    public:
        CommanderException(std::string message) : _errorMessage(std::move(message)) {}
        CommanderException(const std::string& message, const lexer::FilePosition& position)
            : _errorMessage(message + "\n" + position.toString()) {}

        /**
         * Returns the error message associated with the exception
         * @return The error message
         */
        [[nodiscard]] const char* what() const noexcept override;

    private:
        std::string _errorMessage;
    };

}  // namespace util

#endif  // COMMANDER_COMMANDER_EXCEPTION_HPP
