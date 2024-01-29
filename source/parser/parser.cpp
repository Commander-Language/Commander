/**
 * @file source/parser/parser.cpp
 * @brief Implements the `Parser` class.
 *
 */

#include "parser.hpp"

#include "source/util/commander_exception.hpp"

namespace Parser {

    Parser::Parser() { _parseTable = ParseTable(); }

    ASTNodeList Parser::parse(const lexer::TokenList& tokens) {
        std::vector<ProductionItem> productionStack;
        std::vector<ParserAction::StateNum> stateStack {0};
        size_t tokenIndex = 0;

        while (true) {
            const auto& token = tokens[tokenIndex];
            const auto action = _parseTable.getNextAction(stateStack.back(), tokens[tokenIndex]->type);

            switch (action.actionType) {
                case ParserAction::SHIFT:
                    productionStack.emplace_back(token);
                    ++tokenIndex;
                    stateStack.emplace_back(action.nextState);
                    break;
                case ParserAction::REDUCE: {
                    const std::vector<ProductionItem> poppedItems {productionStack.end() - (long)action.ruleSize,
                                                                   productionStack.end()};
                    productionStack.erase(productionStack.end() - (long)action.ruleSize, productionStack.end());

                    const auto newNode = action.nodeConstructor.value()(poppedItems);
                    productionStack.emplace_back(newNode);
                    stateStack.erase(stateStack.end() - (long)action.ruleSize, stateStack.end());
                    stateStack.push_back(_parseTable.getNextState(stateStack.back(), newNode->nodeType()));
                } break;
                case ParserAction::ACCEPT:
                    return [&]() {
                        ASTNodeList result;
                        for (const auto& node : productionStack) result.push_back(node.node);
                        return result;
                    }();
                case ParserAction::ERROR:
                    throw util::CommanderException("Unexpected `" + lexer::tokenTypeToString(token->type) + "` token: `"
                                                           + token->contents + "`",
                                                   token->position);
            }
        }
    }

}  //  namespace Parser
