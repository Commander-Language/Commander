/**
 * @file source/parser/parser_action.hpp
 * @brief Defines the `ParserAction` class.
 * @details Used to tell the parser which action to take at a given step.
 */

#ifndef PARSER_ACTION_HPP
#define PARSER_ACTION_HPP

#include "ast_node.hpp"
#include "production_item.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>

namespace Parser {

    /**
     * @brief Tells the parser which action to take.
     *
     */
    class ParserAction {
    public:
        /**
         * @brief Each parser state has a unique ID number.
         *
         */
        using StateNum = unsigned int;

        /**
         * @brief A function that constructs a new AST node smart pointer.
         * @details Constructs the AST node from a list of production items.
         *
         */
        using NodeConstructor = std::function<ASTNodePtr(const ProductionItemList&)>;

        /**
         * @brief The type of action to take.
         * @details Whether to perform a shift or reduce action, or that we're done parsing,
         *     or that there was a syntax error.
         */
        enum ActionType : uint8_t { SHIFT, REDUCE, ACCEPT, ERROR };

        /**
         * @brief Default constructor.
         * @details Necessary for use in a `std::unordered_map`. The action type is `ERROR`.
         *
         */
        ParserAction();

        /**
         * @brief Class constructor for a `SHIFT` or `ACCEPT` action.
         *
         * @param actionType The type of action to perform.
         * @param nextState The next state, to which the parser needs to transition.
         *     Defaults to 0 for non-`SHIFT` actions.
         */
        ParserAction(ActionType actionType, StateNum nextState = 0);

        /**
         * @brief Class constructor for a `REDUCE` action.
         *
         * @param ruleSize The number of production items to pop from the parse stack and state stack,
         *     and to provide to the node constructor.
         * @param nodeConstructor A function that constructs a new AST node smart pointer.
         */
        ParserAction(size_t ruleSize, NodeConstructor nodeConstructor);

        /**
         * @brief The type of action that the parser should take.
         */
        ActionType actionType;

        /**
         * @brief The next state, to which the parser needs to transition.
         * @details Only relevant for `SHIFT` actions.
         */
        StateNum nextState {};

        /**
         * @brief The number of production items to pop from the stack, and to provide to the node constructor.
         * @details Only relevant for `REDUCE` actions.
         */
        size_t ruleSize {};

        /**
         * @brief A function that constructs a new AST node smart pointer.
         * @details Only relevant for `REDUCE` actions.
         */
        std::optional<NodeConstructor> nodeConstructor;
    };

}  //  namespace Parser

#endif  //  PARSER_ACTION_HPP
