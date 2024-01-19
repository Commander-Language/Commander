/**
 * @file flow_controller.cpp
 * @brief Implements the Flow Controller and runtime
 *
 */

#include "flow_controller.hpp"

namespace FlowController {
    FlowController::FlowController(Parser::ASTNodeList& nodes) : _nodes(std::move(nodes)) {
        _symbolTable.pushSymbolTable();  // push in the global scope
    }
    void FlowController::runCommand() {
        // TODO: Implement
    }
    void FlowController::setVariable(std::string name, std::any value) {
        _symbolTable.addOrUpdateVariable(std::move(name), 0);
    }
    void FlowController::runtime() {
        for (auto& node : _nodes) {
            switch (node->nodeType()) {
                case Parser::BINDING: {
                    _binding(std::dynamic_pointer_cast<Parser::BindingNode>(node));
                    break;
                }
                case Parser::BINDINGS: {
                    _bindings(std::dynamic_pointer_cast<Parser::BindingsNode>(node));
                    break;
                }
                case Parser::CMD: {
                    _cmd(std::dynamic_pointer_cast<Parser::CmdNode>(node));
                    break;
                }
                case Parser::EXPR: {
                    _expr(std::dynamic_pointer_cast<Parser::ExprNode>(node));
                    break;
                }
                case Parser::EXPRS: {
                    _exprs(std::dynamic_pointer_cast<Parser::ExprsNode>(node));
                    break;
                }
                case Parser::PRGM: {
                    _prgm(std::dynamic_pointer_cast<Parser::PrgmNode>(node));
                    break;
                }
                case Parser::STMT: {
                    _stmt(std::dynamic_pointer_cast<Parser::StmtNode>(node));
                    break;
                }
                case Parser::STMTS: {
                    _stmts(std::dynamic_pointer_cast<Parser::StmtsNode>(node));
                    break;
                }
                case Parser::STRING: {
                    _string(std::dynamic_pointer_cast<Parser::StringNode>(node));
                    break;
                }
                case Parser::TYPE: {
                    _type(std::dynamic_pointer_cast<Parser::TypeNode>(node));
                    break;
                }
                case Parser::VARIABLE: {
                    _variable(std::dynamic_pointer_cast<Parser::VariableNode>(node));
                    break;
                }
            }
        }
    }

    void FlowController::_binding(Parser::BindingNodePtr node) {
        // TODO: Implement
    }
    void FlowController::_bindings(Parser::BindingsNodePtr node) {
        // TODO: Implement
    }
    void FlowController::_cmd(Parser::CmdNodePtr node) {
        // TODO: Implement
    }
    std::any FlowController::_expr(Parser::ExprNodePtr node) {
        switch (node->exprType()) {
            case Parser::ExprType::INT_EXPR: {
                auto intExp = std::dynamic_pointer_cast<Parser::IntExprNode>(node);
                return intExp->value;
            }
            case Parser::ExprType::FLOAT_EXPR: {
                // TODO: Implement
                break;
            }
            case Parser::ExprType::STRING_EXPR: {
                // TODO: Implement
                break;
            }
            case Parser::ExprType::BOOL_EXPR: {
                // TODO: Implement
                break;
            }
            case Parser::ExprType::VARIABLE_EXPR: {
                // TODO: Implement
                break;
            }
            case Parser::ExprType::ARRAY_EXPR: {
                // TODO: Implement
                break;
            }
            case Parser::ExprType::ARRAY_INDEXED_EXPR: {
                // TODO: Implement
                break;
            }
            case Parser::ExprType::TUPLE_EXPR: {
                // TODO: Implement
                break;
            }
            case Parser::ExprType::TUPLE_INDEXED_EXPR: {
                // TODO: Implement
                break;
            }
            case Parser::ExprType::TERNARY_EXPR: {
                // TODO: Implement
                break;
            }
            case Parser::ExprType::UNARY_EXPR: {
                // TODO: Implement
                break;
            }
            case Parser::ExprType::BINARY_EXPR: {
                auto binOp = std::dynamic_pointer_cast<Parser::BinOpExprNode>(node);
                switch (binOp->opType) {
                    case Parser::BinOpType::EQUAL: {
                        auto variable = std::dynamic_pointer_cast<Parser::IdentVariableNode>(binOp->leftVariable);
                        std::any value = _expr(binOp->rightExpr);
                        // any_cast<int64_t> might change later
                        _symbolTable.addOrUpdateVariable(variable->varName, std::any_cast<int64_t>(value));
                        return value;
                    }
                }
                break;
            }
            case Parser::ExprType::FUNCTION_CALL_EXPR: {
                // TODO: Implement
                break;
            }
            case Parser::ExprType::LAMBDA_EXPR: {
                // TODO: Implement
                break;
            }
            case Parser::ExprType::COMMAND_EXPR: {
                // TODO: Implement
                break;
            }
        }
        return -1;  // TODO: Find better default return
    }
    void FlowController::_exprs(Parser::ExprsNodePtr node) {
        for (auto& expr : node->exprs) { _expr(expr); }
    }
    void FlowController::_prgm(std::shared_ptr<Parser::PrgmNode> node) {
        for (auto& stmt : node->stmts) { _stmt(stmt); }
    }
    void FlowController::_stmt(Parser::StmtNodePtr node) {
        switch (node->stmtType()) {
            case Parser::StmtType::IF_STMT: {
                // TODO: Implement
                break;
            }
            case Parser::StmtType::FOR_STMT: {
                // TODO: Implement
                break;
            }
            case Parser::StmtType::WHILE_STMT: {
                // TODO: Implement
                break;
            }
            case Parser::StmtType::DO_WHILE_STMT: {
                // TODO: Implement
                break;
            }
            case Parser::StmtType::RETURN_STMT: {
                // TODO: Implement
                break;
            }
            case Parser::StmtType::SCOPE_STMT: {
                // TODO: Implement
                break;
            }
            case Parser::StmtType::COMMAND_STMT: {
                // TODO: Implement
                break;
            }
            case Parser::StmtType::EXPRESSION_STMT: {
                auto expr = std::dynamic_pointer_cast<Parser::ExprStmtNode>(node);
                _expr(expr->expression);
                break;
            }
            case Parser::StmtType::ALIAS_STMT: {
                // TODO: Implement
                break;
            }
        }
    }
    void FlowController::_stmts(Parser::StmtsNodePtr node) {
        // TODO: Implement
    }
    void FlowController::_string(Parser::StringNodePtr node) {
        // TODO: Implement
    }
    void FlowController::_type(Parser::TypeNodePtr node) {
        // TODO: Implement
    }
    void FlowController::_variable(Parser::VariableNodePtr node) {
        // TODO: Implement
    }
}  // namespace FlowController
