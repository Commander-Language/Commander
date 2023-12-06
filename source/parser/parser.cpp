/**
* @file source/parser/parser.cpp
* @brief Implements the `Parser` class.
*
*/

#include "parser.hpp"

namespace Parser {

    Parser::Parser() {
        _parseTable = ParseTable();
    }

    ASTNodeList Parser::parse(const lexer::TokenList& tokens) {
        // TODO
    }
}