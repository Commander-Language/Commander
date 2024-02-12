/**
 * @file source/parser/parse_table.hpp
 * @brief Defines the `ParseTable` class.
 * @details Provides transitions from parser state to state.
 *     The implementation of this class is auto-generated by the parse table generator.
 */

#ifndef PARSE_TABLE_HPP
#define PARSE_TABLE_HPP

#include "ast_node.hpp"
#include "parser_action.hpp"

#include "source/lexer/lexer.hpp"

#include <unordered_map>
#include <vector>

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
         * @brief A mapping of a state number (index) and a token type to a parser action.
         */
        std::vector<std::unordered_map<TokenType, ParserAction>> _nextAction;

        /**
         * @brief A mapping of a state number and an AST node type to the parser's next state.
         */
        std::unordered_map<StateNum, std::unordered_map<ASTNodeType, StateNum>> _nextState;
    };

}  //  namespace Parser

#endif  //  PARSE_TABLE_HPP
