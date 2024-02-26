/**
 * @file source/parser/parser_action.cpp
 * @brief Implements the `ParserAction` class.
 * @details Used to tell the parser which action to take at a given step.
 */

#include "parser_action.hpp"

#include <utility>

namespace Parser {

    ParserAction::ParserAction() : actionType(ERROR) {}

    ParserAction::ParserAction(const ActionType actionType, const StateNum nextState)
        : actionType(actionType), nextState(nextState) {}

    ParserAction::ParserAction(const std::size_t ruleSize, NodeConstructor nodeConstructor)
        : actionType(REDUCE), ruleSize(ruleSize), nodeConstructor(std::move(nodeConstructor)) {}

}  //  namespace Parser
