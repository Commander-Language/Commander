/**
 * @file source/parser/generator/generator.hpp
 * @brief Defines the parse table `Generator` class and methods.
 */

#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "grammar.hpp"

#include "source/lexer/lexer.hpp"
#include "source/parser/ast_node.hpp"

#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

namespace Parser {

    /**
     * @brief The parse table generator class.
     * @details Builds the parse table, and then generates C++ source text.
     */
    class Generator {
    public:
        /**
         * @brief Shorthand for the `TokenType` enumeration from the lexer.
         */
        using TokenType = Lexer::tokenType;

        /**
         * @brief Each state has a unique integer ID.
         */
        using StateNum = unsigned int;

        /**
         * @brief Represents an initializer list for a `ParserAction`.
         * @details Copied to the generated output.
         */
        using ParserActionInitializer = std::string;

        /**
         * @brief Class constructor.
         * @details Does a lot of work under the hood. Avoid calling more than once.
         */
        Generator();

        /**
         * @brief Generates C++ source text for the parse table.
         *
         * @param output The stream to which to output the generated C++ source text.
         */
        void generateSource(std::ostream& output) const;

    private:
        /**
         * @brief Joins the given items into a string with the given delimiter.
         *
         * @param delimiter The delimiter of the string.
         * @param strings The items that will be joined together.
         * @return The joined string with the given delimiter.
         */
        static std::string _join(const std::string& delimiter, const std::vector<std::string>& strings);

        /**
         * @brief Wraps the given string in curly braces.
         *
         * @param contents The string to put inside curly braces.
         * @return The string wrapped in curly braces.
         */
        static std::string _wrap(const std::string& contents);

        /**
         * @brief Makes a braced pair of the two given strings.
         *
         * @param left The left string.
         * @param right The right string.
         * @return A braced pair of the two given strings.
         */
        static std::string _pair(const std::string& left, const std::string& right);

        /**
         * @brief A mapping of a state number and a token type to a parser action.
         */
        std::unordered_map<StateNum, std::unordered_map<TokenType, ParserActionInitializer>> _nextAction;

        /**
         * @brief A mapping of a state number an an AST node type to the parser's next state.
         */
        std::unordered_map<StateNum, std::unordered_map<ASTNodeType, StateNum>> _nextState;
    };

}  //  namespace Parser

/**
 * @brief Main program entry point.
 *
 * @param argc The number of command-line arguments.
 * @param argv The command-line argument vector.
 */
int main(int argc, char** argv);

#endif  //  GENERATOR_HPP
