/**
 * @file source/parser/parse_table.hpp
 * @brief Defines the `ParseTable` class.
 * @details Provides transitions from parser state to state.
 */

#ifndef PARSE_TABLE_HPP
#define PARSE_TABLE_HPP

#include "ast_node.hpp"
#include "grammar.hpp"
#include "parser_action.hpp"

#include "source/lexer/lexer.hpp"

#include <functional>
#include <string>
#include <unordered_map>

namespace Parser {

    /**
     * @brief The table of state transitions that the parser follows.
     */
    class ParseTable {
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
         * @brief Class constructor.
         * @details Does a lot of initialization work under the hood. Avoid calling more than once.
         */
        ParseTable();

        /**
         * @brief Looking at the next unparsed token, reports which action to take.
         *
         * @param stateNum The current state of the parser.
         * @param tokenType The type of the next unparsed token.
         * @return The next action for the parser to take.
         */
        ParserAction getNextAction(StateNum stateNum, TokenType tokenType);

        /**
         * @brief Given the top AST node on the stack, reports the next state to use.
         *
         * @param stateNum The current state of the parser.
         * @param nodeType The AST node type of the top AST node on the stack.
         * @return The next state, to which the parser will transition.
         */
        StateNum getNextState(StateNum stateNum, ASTNodeType nodeType);

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
         * @brief A mapping of a state number and a token type to a parser action.
         */
        std::unordered_map<StateNum, std::unordered_map<TokenType, ParserAction>> _nextAction;

        /**
         * @brief A mapping of a state number an an AST node type to the parser's next state.
         */
        std::unordered_map<StateNum, std::unordered_map<ASTNodeType, StateNum>> _nextState;
    };

}  //  namespace Parser

#endif  //  PARSE_TABLE_HPP
