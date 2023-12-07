/**
 * @file source/parser/parser_action.cpp
 * @brief Implements the `ParserAction` class.
 * @details Used to tell the parser which action to take at a given step.
 */

#include "parser_action.hpp"

namespace Parser {

    ParserAction::ParserAction() : actionType(SHIFT) {}

    ParserAction::ParserAction(Parser::ParserAction::StateNum nextState) : actionType(SHIFT), nextState(nextState) {}

    ParserAction::ParserAction(size_t ruleSize, Parser::ParserAction::NodeConstructor nodeConstructor)
        : actionType(REDUCE), ruleSize(ruleSize), nodeConstructor(std::move(nodeConstructor)) {}
}  //  namespace Parser
