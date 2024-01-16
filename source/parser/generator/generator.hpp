/**
 * @file source/parser/generator/generator.hpp
 * @brief Defines the parse table `Generator` class and methods.
 */

#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "grammar.hpp"

#include "source/lexer/lexer.hpp"
#include "source/parser/parser_action.hpp"

#include <ostream>
#include <string>

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
        using TokenType = lexer::tokenType;

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

    private:
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
             * @brief Class constructor.
             *
             * @param rule The `GrammarRule` to which this kernel refers.
             * @param index How far we are into the `GrammarRule`.
             * @param lookahead The next lookahead token type.
             */
            Kernel(const GrammarRule& rule, size_t index, TokenType lookahead);

            /**
             * @brief Equality operator.
             *
             * @param other The other `Kernel` against which to compare.
             * @return True if the two kernels are equal; false otherwise.
             */
            bool operator==(const Kernel& other) const;

            /**
             * @brief Inequality operator.
             *
             * @param other The other `Kernel` against which to compare.
             * @return False if the two kernel are equal; true otherwise.
             */
            bool operator!=(const Kernel& other) const;

            /**
             * @brief The `GrammarRule` to which this kernel refers.
             */
            std::reference_wrapper<const GrammarRule> rule;

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
         * @brief Represents a state in the parsing automaton.
         */
        struct State {
            /**
             * @brief Class constructor.
             */
            State();

            /**
             * @brief Class constructor.
             *
             * @param kernels All relevant kernels corresponding to this state.
             * @param transitions All transitions from this state.
             */
            State(const std::vector<Kernel>& kernels,
                  const std::unordered_map<GrammarEntry, StateNum, Grammar::GrammarEntry::Hash>& transitions = {});

            /**
             * @brief All relevant kernels corresponding to this state.
             */
            std::vector<Kernel> kernels;

            /**
             * @brief All transitions from this state.
             */
            std::unordered_map<GrammarEntry, StateNum, Grammar::GrammarEntry::Hash> transitions;
        };

        /**
         * @brief Represents the closure of all reachable kernels from any single kernel.
         */
        class Closure {
        public:
            /**
             * @brief Class constructor.
             *
             * @param grammar A reference to the grammar rules.
             */
            Closure(const std::vector<GrammarRule>& grammar);

            /**
             * @brief Gets the closure of all reachable kernels from the given set of kernels.
             * @details Performs a calculation on the first query; saves results for subsequent queries.
             *
             * @param kernels The kernels for which to report the reachable kernels.
             * @return The vector of all possible reachable kernels from the given kernels.
             */
            std::vector<Kernel> operator[](const std::vector<Kernel>& kernels);

        private:
            /**
             * @brief A mapping from one kernel to the set of all reachable kernels from it.
             */
            std::unordered_map<Kernel, std::vector<Kernel>, Kernel::Hash> _closure;

            /**
             * @brief A reference to the grammar definition.
             */
            const std::vector<GrammarRule>& _grammar;
        };

        /**
         * @brief Joins the given items into a string with the given delimiter.
         *
         * @tparam StrType The types of the individual items.
         * @param delimiter The delimiter of the string.
         * @param strings The items that will be joined together.
         * @return The joined string with the given delimiter.
         */
        template<typename... StrType>
        static std::string _join(const std::string& delimiter, const StrType&... strings);

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
        template <typename LeftType, typename RightType>
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
