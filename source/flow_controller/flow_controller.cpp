/**
 * @file flow_controller.cpp
 * @brief Implements the Flow Controller and runtime
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
        // TODO: generic value for symbol table
        _symbolTable.addOrUpdateVariable(std::move(name), 0);
    }
    std::any FlowController::getVariable(std::string name) {
        std::any value = _symbolTable.getVariable(name);
        if (value.has_value()) { return value; }
        // TODO: Throw symbol not found error
    }
    CommanderLambda FlowController::findFunctionOrLambda(const Parser::ExprNodePtr& expr) {
        if (expr->exprType() == Parser::ExprType::VARIABLE_EXPR) {
            auto varExpr = std::dynamic_pointer_cast<Parser::IdentVariableNode>(expr);
            // TODO: Check if return type is a lambda
            std::any func = getVariable(varExpr->varName);
            return std::any_cast<CommanderLambda>(func);
        } else if (expr->exprType() == Parser::ExprType::LAMBDA_EXPR) {

        } else {
            // TODO: Throw error
        }
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
                default: {
                    // TODO: Throw unknown type error
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
                auto floatExp = std::dynamic_pointer_cast<Parser::FloatExprNode>(node);
                return floatExp->value;
            }
            case Parser::ExprType::STRING_EXPR: {
                // TODO: Implement
                break;
            }
            case Parser::ExprType::BOOL_EXPR: {
                auto boolExp = std::dynamic_pointer_cast<Parser::BoolExprNode>(node);
                return boolExp->value;
            }
            case Parser::ExprType::VARIABLE_EXPR: {
                auto varExp = std::dynamic_pointer_cast<Parser::VarExprNode>(node);
                return getVariable(std::dynamic_pointer_cast<Parser::IdentVariableNode>(varExp->variable)->varName);
            }
            case Parser::ExprType::ARRAY_EXPR: {
                auto arrExp = std::dynamic_pointer_cast<Parser::ArrayExprNode>(node);
                // TODO: Implement
                return arrExp->expressions;  // What should this return?!
            }
            case Parser::ExprType::ARRAY_INDEXED_EXPR: {
                auto arrExp = std::dynamic_pointer_cast<Parser::ArrayIndexExprNode>(node);
                // TODO: Implement
                break;
            }
            case Parser::ExprType::TUPLE_EXPR: {
                auto tupleExp = std::dynamic_pointer_cast<Parser::TupleExprNode>(node);
                CommanderTuple tuple;
                for (auto& expr : tupleExp->expressions) { tuple.emplace_back(_expr(expr)); }
                return tuple;
            }
            case Parser::ExprType::TUPLE_INDEXED_EXPR: {
                auto tupleExp = std::dynamic_pointer_cast<Parser::TupleIndexExprNode>(node);
                CommanderInt index = std::any_cast<CommanderInt>(_expr(tupleExp->index));
                CommanderTuple tuple = std::any_cast<CommanderTuple>(_expr(tupleExp->tuple));

                if (index >= tuple.size() || index < 0) {
                    // TODO: Throw index out of bounds error
                }
                return tuple[index];
            }
            case Parser::ExprType::TERNARY_EXPR: {
                auto ternaryExpr = std::dynamic_pointer_cast<Parser::TernaryExprNode>(node);
                bool condition = std::any_cast<bool>(_expr(ternaryExpr->condition));
                std::any ifTrue = _expr(ternaryExpr->trueExpr);
                std::any ifFalse = _expr(ternaryExpr->falseExpr);
                return condition ? ifTrue : ifFalse;
            }
            case Parser::ExprType::UNARY_EXPR: {
                auto unOp = std::dynamic_pointer_cast<Parser::UnOpExprNode>(node);
                return _unaryOp(unOp);
            }
            case Parser::ExprType::BINARY_EXPR: {
                auto binOp = std::dynamic_pointer_cast<Parser::BinOpExprNode>(node);
                return _binaryOp(binOp);
            }
            case Parser::ExprType::FUNCTION_CALL_EXPR: {
                auto funcExpr = std::dynamic_pointer_cast<Parser::CallExprNode>(node);
                std::any func = _expr(funcExpr->func);

                // TODO: Implement
            }
            case Parser::ExprType::LAMBDA_EXPR: {
                auto lambdaExpr = std::dynamic_pointer_cast<Parser::LambdaExprNode>(node);
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

    std::any FlowController::_unaryOp(std::shared_ptr<Parser::UnOpExprNode>& unOp) {
        switch (unOp->opType) {
            case Parser::NEGATE:
                break;
            case Parser::NOT:
                break;
            case Parser::PRE_INCREMENT:
                break;
            case Parser::POST_INCREMENT:
                break;
            case Parser::PRE_DECREMENT:
                break;
            case Parser::POST_DECREMENT:
                break;
        }
    }

    std::any FlowController::_binaryOp(std::shared_ptr<Parser::BinOpExprNode>& binOp) {
        switch (binOp->opType) {
            case Parser::LESSER:
                break;
            case Parser::GREATER:
                break;
            case Parser::LESSER_EQUAL:
                break;
            case Parser::GREATER_EQUAL:
                break;
            case Parser::MODULO:
                break;
            case Parser::DIVIDE:
                break;
            case Parser::MULTIPLY:
                break;
            case Parser::SUBTRACT:
                break;
            case Parser::ADD:
                break;
            case Parser::EXPONENTIATE:
                break;
            case Parser::AND:
                break;
            case Parser::OR:
                break;
            case Parser::EQUAL: {
                auto variable = std::dynamic_pointer_cast<Parser::IdentVariableNode>(binOp->leftVariable);
                std::any value = _expr(binOp->rightExpr);
                // any_cast<int64_t> might change later
                _symbolTable.addOrUpdateVariable(variable->varName, std::any_cast<CommanderInt>(value));
                return value;
            }
            case Parser::NOT_EQUAL:
                break;
            case Parser::ADD_EQUAL:
                break;
            case Parser::SUBTRACT_EQUAL:
                break;
            case Parser::MULTIPLY_EQUAL:
                break;
            case Parser::DIVIDE_EQUAL:
                break;
            case Parser::MODULO_EQUAL:
                break;
            case Parser::EXPONENTIATE_EQUAL:
                break;
        }
    }
}  // namespace FlowController
