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

#include <vector>

namespace Parser {

    /**
     * @brief Either a token or an AST node.
     * @details This goes on the parse stack.
     *
     */
    struct ProductionItem {
        /**
         * @brief Class constructor from a token.
         *
         * @param token The token that makes up this `ProductionItem`.
         */
        ProductionItem(Lexer::TokenPtr token);

        /**
         * @brief Class constructor from an AST node.
         *
         * @param node The AST node that makes up this `ProductionItem`.
         */
        ProductionItem(ASTNodePtr node);

        /**
         * @brief The token, if this is a token.
         *
         */
        Lexer::TokenPtr token;

        /**
         * @brief The AST node, if this is an AST node.
         *
         */
        ASTNodePtr node;
    };

    /**
     * @brief A list of `ProductionItem`s.
     *
     */
    using ProductionItemList = std::vector<ProductionItem>;

}  //  namespace Parser

#endif  //  PRODUCTION_ITEM_HPP
