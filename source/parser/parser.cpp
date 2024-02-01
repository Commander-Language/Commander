/**
 * @file source/parser/parser.cpp
 * @brief Implements the `Parser` class.
 *
 */

#include "parser.hpp"

#include "source/util/commander_exception.hpp"

namespace Parser {

    Parser::Parser() { _parseTable = ParseTable(); }

    ASTNodeList Parser::parse(const Lexer::TokenList& tokens) {
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
                    stateStack.push_back(
                            _parseTable.getNextState(stateStack.back(), getAbstractNodeType(newNode->nodeType())));
                } break;
                case ParserAction::ACCEPT:
                    return [&]() {
                        ASTNodeList result;
                        for (const auto& node : productionStack) result.push_back(node.node);
                        return result;
                    }();
                case ParserAction::ERROR:
                    throw util::CommanderException("Unexpected token: `" + token->contents + "`", token->position);
            }
        }
    }

    ASTNodeType getAbstractNodeType(const ASTNodeType& type) {
        switch (type) {
            case BINDING:
            case BINDINGS:
            case EXPRS:
            case PRGM:
            case STMTS:
            case STRING:
            case VARIABLE:
                return type;
            case CMD:
            case CMD_CMD:
            case PIPE_CMD:
            case ASYNC_CMD:
                return CMD;
            case EXPR:
            case INT_EXPR:
            case FLOAT_EXPR:
            case STRING_EXPR:
            case BOOL_EXPR:
            case VAR_EXPR:
            case ARRAY_EXPR:
            case ARRAY_INDEX_EXPR:
            case TUPLE_EXPR:
            case TUPLE_INDEX_EXPR:
            case TERNARY_EXPR:
            case UNOP_EXPR:
            case BINOP_EXPR:
            case CALL_EXPR:
            case LAMBDA_EXPR:
            case CMD_EXPR:
                return EXPR;
            case STMT:
            case IF_STMT:
            case FOR_STMT:
            case WHILE_STMT:
            case DO_WHILE_STMT:
            case RETURN_STMT:
            case SCOPE_STMT:
            case CMD_STMT:
            case EXPR_STMT:
            case ALIAS_STMT:
                return STMT;
            case TYPE:
            case INT_TYPE:
            case FLOAT_TYPE:
            case BOOL_TYPE:
            case STRING_TYPE:
            case ARRAY_TYPE:
            case TUPLE_TYPE:
            case FUNCTION_TYPE:
                return TYPE;
        }
        return PRGM;
    }

}  //  namespace Parser
