/**
 * @file source/parser/production_item.hpp
 * @brief Defines the `ProductionItem` class.
 * @details This is the thing that goes on the stack.
 *
 */

#ifndef PRODUCTION_ITEM_HPP
#define PRODUCTION_ITEM_HPP

#include "ast_node.hpp"

#include "source/lexer/lexer.hpp"

#include <optional>

namespace Parser {

    /**
     * @brief Either a token or an AST node.
     * @details This goes on the parse stack.
     *
     */
    struct ProductionItem {
        /**
         * @brief The token, if this is a token.
         *
         */
        std::optional<lexer::Token> token;

        /**
         * @brief The AST node, if this is an AST node.
         *
         */
        std::optional<ASTNodePtr> node;
    };

}  //  namespace Parser

#endif  //  PRODUCTION_ITEM_HPP
