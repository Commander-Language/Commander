/**
 * @file source/parser/production_item.cpp
 * @brief Implements the `ProductionItem` class.
 */

#include "production_item.hpp"

namespace Parser {

    ProductionItem::ProductionItem(Lexer::TokenPtr token) : token(std::move(token)) {}

    ProductionItem::ProductionItem(ASTNodePtr node) : node(std::move(node)) {}

}  //  namespace Parser
