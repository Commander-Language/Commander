/**
 * @file source/parser/parser.cpp
 * @brief Implements the `Parser` class.
 *
 */

#include "parser.hpp"
#include "ast_node.hpp"
#include "parser_action.hpp"
#include "production_item.hpp"

#include "source/util/commander_exception.hpp"

#include <cstddef>
#include <vector>

namespace Parser {

    Parser::Parser() { _parseTable = ParseTable(); }

    ASTNodeList Parser::parse(const Lexer::TokenList& tokens) {
        std::vector<ProductionItem> productionStack;
        std::vector<ParserAction::StateNum> stateStack {0};
        std::size_t tokenIndex = 0;

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
                    const std::vector<ProductionItem> poppedItems {
                            productionStack.end() - static_cast<long>(action.ruleSize), productionStack.end()};
                    productionStack.erase(productionStack.end() - static_cast<long>(action.ruleSize),
                                          productionStack.end());

                    const auto newNode = action.nodeConstructor.value()(poppedItems);
                    productionStack.emplace_back(newNode);
                    stateStack.erase(stateStack.end() - static_cast<long>(action.ruleSize), stateStack.end());
                    stateStack.push_back(
                            _parseTable.getNextState(stateStack.back(), getAbstractNodeType(newNode->nodeType())));
                } break;
                case ParserAction::ACCEPT:
                    return [&] {
                        ASTNodeList result;
                        for (const auto& node : productionStack) result.push_back(node.node);
                        return result;
                    }();
                case ParserAction::ERROR:
                    throw Util::CommanderException("Unexpected `" + tokenTypeToString(token->type) + "` token: `"
                                                           + token->contents + "`",
                                                   token->position);
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
            case STRING_EXPRS:
            case TYPES:
            case CMD:
            case BASIC_CMD:
            case PIPE_CMD:
            case ASYNC_CMD:
                return type;
            case LVALUE:
            case VAR_LVALUE:
            case INDEX_LVALUE:
                return LVALUE;
            case EXPR:
            case INT_EXPR:
            case FLOAT_EXPR:
            case STRING_EXPR:
            case BOOL_EXPR:
            case VAR_EXPR:
            case LVALUE_EXPR:
            case ARRAY_EXPR:
            case INDEX_EXPR:
            case TUPLE_EXPR:
            case TERNARY_EXPR:
            case UNOP_EXPR:
            case BINOP_EXPR:
            case CALL_EXPR:
            case API_CALL_EXPR:
            case LAMBDA_EXPR:
            case CMD_EXPR:
            case SCAN_EXPR:
            case READ_EXPR:
                return EXPR;
            case STMT:
            case IF_STMT:
            case FOR_STMT:
            case WHILE_STMT:
            case DO_WHILE_STMT:
            case RETURN_STMT:
            case BREAK_STMT:
            case CONTINUE_STMT:
            case SCOPE_STMT:
            case CMD_STMT:
            case EXPR_STMT:
            case ALIAS_STMT:
            case IMPORT_STMT:
            case PRINT_STMT:
            case PRINTLN_STMT:
            case WRITE_STMT:
            case TYPE_STMT:
            case FUNCTION_STMT:
            case TIMEOUT_STMT:
            case ASSERT_STMT:
                return STMT;
            case TYPE:
            case INT_TYPE:
            case FLOAT_TYPE:
            case BOOL_TYPE:
            case STRING_TYPE:
            case ARRAY_TYPE:
            case TUPLE_TYPE:
            case FUNCTION_TYPE:
            case VARIABLE_TYPE:
                return TYPE;
        }
        return PRGM;
    }

}  //  namespace Parser
