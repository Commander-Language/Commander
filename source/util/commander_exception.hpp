/**
 * @file commander_exception.cpp
 * @brief Contains implementation for functions in commander_exception.hpp
 *
 */

#ifndef COMMANDER_COMMANDER_EXCEPTION_HPP
#define COMMANDER_COMMANDER_EXCEPTION_HPP

#include "../lexer/lexer.hpp"
#include <string>

namespace Util {

    class CommanderException : std::exception {
    public:
        CommanderException(std::string message) : errorMessage(message) {}
        CommanderException(std::string message, Lexer::FilePosition position)
            : errorMessage(message + "\n" + position.toString()) {}

        /**
         * Returns the error message associated with the exception
         * @return The error message
         */
        const char* what() const noexcept override;

    private:
        std::string errorMessage;
    };

}  // namespace Util

#endif  // COMMANDER_COMMANDER_EXCEPTION_HPP
