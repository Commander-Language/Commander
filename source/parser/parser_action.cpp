/**
* @file source/parser/parser_action.cpp
* @brief Implements the `ParserAction` class.
* @details Used to tell the parser which action to take at a given step.
*
*/

#include "parser_action.hpp"

namespace Parser {

   ParserAction::ParserAction() : actionType(SHIFT), nextState(0), ruleSize(0) {}

   ParserAction::ParserAction(Parser::ParserAction::StateNum nextState)
       : actionType(SHIFT), nextState(nextState), ruleSize(0) {}

   ParserAction::ParserAction(size_t ruleSize, const Parser::ParserAction::NodeConstructor& nodeConstructor)
       : actionType(REDUCE), nextState(0), ruleSize(ruleSize), nodeConstructor(nodeConstructor) {}
}  // namespace Parser