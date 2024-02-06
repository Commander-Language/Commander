/**
 * @file source/parser/generator/generator.hpp
 * @brief Defines the parse table `Generator` class and methods.
 */

#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "grammar.hpp"

#include "source/lexer/lexer.hpp"
#include "source/parser/ast_node.hpp"

#include <cstddef>
#include <ostream>
#include <set>
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
         * @brief Represents an entry in a grammar rule.
         * @details Either a token type or an AST node type.
         *     Every rule is made up of these.
         */
        using GrammarEntry = Grammar::GrammarEntry;

        /**
         * @brief Represents a single grammar rule.
         * @details Made up of the resulting AST node type and a list of component `GrammarEntry`s.
         */
        using GrammarRule = Grammar::GrammarRule;

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

        /**
         * @brief A lookahead kernel for building the parse table.
         * @details Consists of a grammar rule,
         *     how far into the grammar rule we are,
         *     and a lookahead token type.
         */
        struct Kernel {
            /**
             * @brief Hashing functor for `Kernel` objects.
             */
            struct Hash {
                /**
                 * @brief Hashes the given `Kernel`.
                 *
                 * @param kernel The kernel to hash.
                 * @return The hash value of the given kernel.
                 */
                size_t operator()(const Kernel& kernel) const;
            };

            /**
             * @brief Default constructor.
             * @details Necessary for use in some containers.
             */
            Kernel();

            /**
             * @brief Class constructor.
             *
             * @param rule The `GrammarRule` to which this kernel refers.
             * @param priority The priority of the `GrammarRule`.
             * @param index How far we are into the `GrammarRule`.
             * @param lookahead The next lookahead token type.
             */
            Kernel(GrammarRule rule, size_t priority, size_t index, TokenType lookahead);

            /**
             * @brief Equality operator.
             *
             * @param other The other `Kernel` against which to compare.
             * @return True if the two `Kernel`s are equal; false otherwise.
             */
            bool operator==(const Kernel& other) const;

            /**
             * @brief Inequality operator.
             *
             * @param other The other `Kernel` against which to compare.
             * @return False if the two `Kernel`s are equal; true otherwise.
             */
            bool operator!=(const Kernel& other) const;

            /**
             * @brief Less-than operator.
             *
             * @param other The other `Kernel` against which to compare.
             * @return True if this `Kernel`'s hashed value is below the other `Kernel`'s hashed value; false otherwise.
             */
            bool operator<(const Kernel& other) const;

            /**
             * @brief Stream insertion operator.
             *
             * @param stream The `std::ostream` that will receive a kernel as input.
             * @param kernel The kernel to stream to the given `std::ostream`.
             * @return The given stream.
             */
            friend std::ostream& operator<<(std::ostream& stream, const Kernel& kernel);

            /**
             * @brief The `GrammarRule` to which this kernel refers.
             */
            GrammarRule rule;

            /**
             * @brief The priority of the grammar rule.
             */
            size_t priority;

            /**
             * @brief The index into the grammar rule.
             */
            size_t index;

            /**
             * @brief The lookahead token.
             */
            TokenType lookahead;
        };

        /**
         * @brief A set of `Kernel`s.
         */
        using KernelSet = std::set<Kernel>;

        /**
         * @brief Hashing functor for `KernelSet`s.
         */
        struct HashKernelSet {
            size_t operator()(const KernelSet& kernelSet) const;
        };

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
         * @brief Makes a braced pair of the two given items.
         *
         * @tparam LeftType The type of the item on the left.
         * @tparam RightType The type of the item on the right.
         * @param left The left item.
         * @param right The right item.
         * @return A braced pair of the two given items.
         */
        template<typename LeftType, typename RightType>
        static std::string _pair(const LeftType& left, const RightType& right);

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
