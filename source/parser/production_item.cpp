/**
 * @file source/parser/production_item.cpp
 * @brief Implements the `ProductionItem` class.
 */

#include "production_item.hpp"
#include "ast_node.hpp"

#include <utility>

namespace Parser {

    ProductionItem::ProductionItem(Lexer::TokenPtr token) : token(std::move(token)) {}

    ProductionItem::ProductionItem(ASTNodePtr node) : node(std::move(node)) {}

}  //  namespace Parser
