/**
 * @file source/parser/parser.cpp
 * @brief Implements the `Parser` class.
 *
 */

#include "parser.hpp"

namespace Parser {

    Parser::Parser() { _parseTable = ParseTable(); }

    ASTNodeList Parser::parse(const lexer::TokenList& tokens) {
        std::stack<ProductionItem> productionStack;
        std::stack<ParserAction::StateNum> stateStack;
        stateStack.push(0);
        for (const lexer::TokenPtr& token : tokens) {
            ParserAction action = _parseTable.getNextAction(stateStack.top(), token->type);
            // Shift action
            if (action.actionType == ParserAction::SHIFT) {
                productionStack.push({token, {}});
                stateStack.push(action.nextState);
                continue;
                // Reduce action
            } else {
                std::vector<ProductionItem> productionList;
                for (size_t i = 0; i < action.ruleSize; i++) {
                    productionList.push_back(productionStack.top());
                    productionStack.pop();
                }
                ASTNodePtr node = (*action.nodeConstructor)(productionList);
                productionStack.push({{}, node});
            }
            while (!productionStack.top().node) {
                productionStack.pop();
                stateStack.pop();
            }
            ParserAction::StateNum nextState = _parseTable.getNextState(stateStack.top(),
                                                                        productionStack.top().node->nodeType());
            stateStack.push(nextState);
        }
    }
}  // namespace Parser