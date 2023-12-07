/**
 * @file source/parser/parser_action.hpp
 * @brief Defines the `ParserAction` class.
 * @details Used to tell the parser which action to take at a given step.
 *
 */

#ifndef PARSER_ACTION_HPP
#define PARSER_ACTION_HPP

#include "ast_node.hpp"
#include "production_item.hpp"

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
         * @brief Whether the parser should perform a shift or a reduce action.
         *
         */
        enum ActionType { SHIFT, REDUCE };

        /**
         * @brief Default constructor.
         * @details Necessary for use in a `std::unordered_map`.
         *
         */
        ParserAction();

        /**
         * @brief Class constructor for a shift action.
         *
         * @param nextState The next state, to which the parser needs to transition.
         */
        ParserAction(StateNum nextState);

        /**
         * @brief Class constructor for a reduce action.
         *
         * @param ruleSize The number of production items to pop from the stack, and to provide to the node constructor.
         * @param nodeConstructor A function that constructs a new AST node smart pointer.
         */
        ParserAction(size_t ruleSize, NodeConstructor nodeConstructor);

        /**
         * @brief Whether the parser should perform a shift or a reduce action.
         *
         */
        ActionType actionType;

        /**
         * @brief The next state, to which the parser needs to transition.
         * @details Only relevant for shift actions.
         *
         */
        StateNum nextState{};

        /**
         * @brief The number of production items to pop from the stack, and to provide to the node constructor.
         * @details Only relevant for reduce actions.
         *
         */
        size_t ruleSize{};

        /**
         * @brief A function that constructs a new AST node smart pointer.
         * @details Only relevant for reduce actions.
         *
         */
        std::optional<NodeConstructor> nodeConstructor;
    };

}  //  namespace Parser

#endif  //  PARSER_ACTION_HPP
