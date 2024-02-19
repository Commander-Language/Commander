/**
 * @file source/parser/production_item.cpp
 * @brief Implements the `ProductionItem` class.
 */

#include "ast_node.hpp"
#include "production_item.hpp"

#include <utility>

namespace Parser {

    ProductionItem::ProductionItem(Lexer::TokenPtr token) : token(std::move(token)) {}

    ProductionItem::ProductionItem(ASTNodePtr node) : node(std::move(node)) {}

}  //  namespace Parser
