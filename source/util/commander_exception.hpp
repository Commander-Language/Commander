/**
 * @file commander_exception.hpp
 * @brief Contains commander exception
 *
 */

#ifndef COMMANDER_COMMANDER_EXCEPTION_HPP
#define COMMANDER_COMMANDER_EXCEPTION_HPP

#include "source/lexer/lexer.hpp"
#include <string>
#include <utility>

namespace Util {

    /**
     * @brief Class that represents an exception that gets thrown as a result of an error when running Commander.
     */
    class CommanderException : std::exception {
    public:
        /**
         * @brief Constructor for the exception that stores a single message directly
         * @param message The message of the exception
         */
        CommanderException(std::string message);

        /**
         * @brief Constructor for the exception that creates the message with the include position in the file
         * @param message The message of the exception
         * @param position The position in the file that the exception occurs
         */
        CommanderException(const std::string& message, const Lexer::FilePosition& position);

        /**
         * Returns the error message associated with the exception
         * @return The error message
         */
        [[nodiscard]] const char* what() const noexcept override;

    private:
        /**
         * The error message of the exception
         */
        std::string _errorMessage;
    };

}  // namespace Util

#endif  // COMMANDER_COMMANDER_EXCEPTION_HPP
