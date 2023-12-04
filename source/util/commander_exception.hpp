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

    /**
     * @brief Class that represents an exception that gets thrown as a result of an error when running Commander.
     */
    class CommanderException : std::exception {
    public:
        /**
         * Constructor for the exception that stores a single message directly
         * @param message The message of the exception
         */
        CommanderException(std::string message) : _errorMessage(std::move(message)) {}

        /**
         * Constructor for the exception that creates the message with the include position in the file
         * @param message The message of the exception
         * @param position The position in the file that the exception occurs
         */
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
