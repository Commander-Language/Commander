/**
 * @file flow_controller.cpp
 * @brief Implements the Flow Controller and runtime
 *
 *  Important: Assuming the types used with binary/unary expressions to be int. The only exceptions being logical
 *             operators like && and ||. Update to be more generic when we know the types of expressions, possibly
 *             through type checking or saving types in the symbol table.
 *
 * Node Helper Functions:
 *      TODO: Finish the following: _cmd, _type,
 * Statements:
 *      TODO: Finish the following: If, For, While, Do While, Return, Scope, Command, Alias
 * Expressions:
 *      TODO: Finish the following: Command
 */

#include "flow_controller.hpp"
#include "source/job_runner/job_runner.hpp"
#include "source/util/commander_exception.hpp"
#include <cmath>

namespace FlowController {

    //  ==========================
    //  ||   Commander types    ||
    //  ==========================
    CommanderLambda::CommanderLambda(std::vector<Parser::BindingNodePtr> bindings, Parser::StmtNodePtr body)
        : bindings(std::move(bindings)), body(std::move(body)) {}

    //  ==========================
    //  ||    Flow Controller   ||
    //  ==========================
    FlowController::FlowController(Parser::ASTNodeList& nodes) : _nodes(std::move(nodes)) {
        _symbolTable.pushSymbolTable();  // push in the global scope
    }

    void FlowController::runtime() {
        for (auto& node : _nodes) {
            switch (getAbstractNodeType(node->nodeType())) {
                case Parser::BINDING: {
                    _binding(std::static_pointer_cast<Parser::BindingNode>(node));
                    break;
                }
                case Parser::BINDINGS: {
                    _bindings(std::static_pointer_cast<Parser::BindingsNode>(node));
                    break;
                }
                case Parser::CMD: {
                    _cmd(std::static_pointer_cast<Parser::CmdNode>(node));
                    break;
                }
                case Parser::EXPR: {
                    _expr(std::static_pointer_cast<Parser::ExprNode>(node));
                    break;
                }
                case Parser::EXPRS: {
                    _exprs(std::static_pointer_cast<Parser::ExprsNode>(node));
                    break;
                }
                case Parser::PRGM: {
                    _prgm(std::static_pointer_cast<Parser::PrgmNode>(node));
                    break;
                }
                case Parser::STMT: {
                    _stmt(std::static_pointer_cast<Parser::StmtNode>(node));
                    break;
                }
                case Parser::STMTS: {
                    _stmts(std::static_pointer_cast<Parser::StmtsNode>(node));
                    break;
                }
                case Parser::STRING: {
                    _string(std::static_pointer_cast<Parser::StringNode>(node));
                    break;
                }
                case Parser::TYPE: {
                    _type(std::static_pointer_cast<Parser::TypeNode>(node));
                    break;
                }
                case Parser::VARIABLE: {
                    _variable(std::static_pointer_cast<Parser::VariableNode>(node));
                    break;
                }
                default: {
                    throw Util::CommanderException("Flow Controller: Encountered unknown node type");
                }
            }
        }
    }

    //  ==========================
    //  ||    Node Evaluation   ||
    //  ==========================
    void FlowController::_binding(const Parser::BindingNodePtr& node) {
        // TODO: Find better default value for each type
        _setVariable(node->variable, 0);
    }

    void FlowController::_bindings(const Parser::BindingsNodePtr& node) {
        for (auto& binding : node->bindings) { _binding(binding); }
    }

    void FlowController::_cmd(const Parser::CmdNodePtr& node) {
        // TODO: Implement
    }

    std::any FlowController::_expr(const Parser::ExprNodePtr& node) {
        switch (node->nodeType()) {
            case Parser::INT_EXPR: {
                auto intExp = std::static_pointer_cast<Parser::IntExprNode>(node);
                return intExp->value;
            }
            case Parser::FLOAT_EXPR: {
                auto floatExp = std::static_pointer_cast<Parser::FloatExprNode>(node);
                return floatExp->value;
            }
            case Parser::STRING_EXPR: {
                auto stringExp = std::static_pointer_cast<Parser::StringExprNode>(node);
                return _string(stringExp->stringNode);
            }
            case Parser::BOOL_EXPR: {
                auto boolExp = std::static_pointer_cast<Parser::BoolExprNode>(node);
                return boolExp->value;
            }
            case Parser::VAR_EXPR: {
                auto varExp = std::static_pointer_cast<Parser::VarExprNode>(node);
                return _getVariable(std::static_pointer_cast<Parser::IdentVariableNode>(varExp->variable)->varName);
            }
            case Parser::ARRAY_EXPR: {
                auto arrExp = std::static_pointer_cast<Parser::ArrayExprNode>(node);
                CommanderArray<std::any> array;
                for (auto& expr : arrExp->expressions) {
                    std::any const value = _expr(expr);
                    array.push_back(value);
                }
                return array;
            }
            case Parser::ARRAY_INDEX_EXPR: {
                auto arrayIndexExpression = std::static_pointer_cast<Parser::ArrayIndexExprNode>(node);
                auto arrayVariable = std::dynamic_pointer_cast<Parser::IdentVariableNode>(arrayIndexExpression->array);
                // TODO: Update to generic array
                auto array = std::any_cast<CommanderArray<CommanderInt>>(_getVariable(arrayVariable->varName));

                // TODO: might be understanding this wrong? why list of indices?
                auto index = std::any_cast<CommanderInt>(_expr(arrayIndexExpression->indexExprs[0]));
                return array[index];
            }
            case Parser::TUPLE_EXPR: {
                auto tupleExp = std::static_pointer_cast<Parser::TupleExprNode>(node);
                CommanderTuple tuple;
                for (auto& expr : tupleExp->expressions) { tuple.emplace_back(_expr(expr)); }
                return tuple;
            }
            case Parser::TUPLE_INDEX_EXPR: {
                auto tupleExp = std::static_pointer_cast<Parser::TupleIndexExprNode>(node);
                auto index = std::any_cast<CommanderInt>(_expr(tupleExp->index));
                auto tuple = std::any_cast<CommanderTuple>(_expr(tupleExp->tuple));

                if (index >= tuple.size() || index < 0) {
                    throw Util::CommanderException("Index out of bounds: Index " + std::to_string(index)
                                                   + "out of bounds for tuple of size " + std::to_string(tuple.size()));
                }
                return tuple[index];
            }
            case Parser::TERNARY_EXPR: {
                auto ternaryExpression = std::static_pointer_cast<Parser::TernaryExprNode>(node);
                bool const condition = std::any_cast<bool>(_expr(ternaryExpression->condition));

                std::any const ifTrue = _expr(ternaryExpression->trueExpr);
                std::any const ifFalse = _expr(ternaryExpression->falseExpr);
                return condition ? ifTrue : ifFalse;
            }
            case Parser::UNOP_EXPR: {
                auto unaryOperation = std::static_pointer_cast<Parser::UnOpExprNode>(node);
                return _unaryOp(unaryOperation);
            }
            case Parser::BINOP_EXPR: {
                auto binaryOperation = std::static_pointer_cast<Parser::BinOpExprNode>(node);
                return _binaryOp(binaryOperation);
            }
            case Parser::CALL_EXPR: {
                auto functionExpression = std::static_pointer_cast<Parser::CallExprNode>(node);
                auto function = std::any_cast<CommanderLambda>(_expr(functionExpression->func));

                _symbolTable.pushSymbolTable();  // new scope for function

                int bindingIndex = 0;
                for (auto& arg : functionExpression->args) {
                    // args and bindings should be lined up 1 to 1
                    std::any const argValue = _expr(arg);
                    std::string const name = function.bindings[bindingIndex]->variable;

                    _setVariable(name, argValue);
                    bindingIndex++;
                }
                std::any returnValue = _stmt(function.body);

                _symbolTable.popSymbolTable();  // remove function scope!
                return returnValue;
            }
            case Parser::LAMBDA_EXPR: {
                auto lambdaExpression = std::static_pointer_cast<Parser::LambdaExprNode>(node);
                CommanderLambda lambda(lambdaExpression->bindings, lambdaExpression->body);
                return lambda;
            }
            case Parser::CMD_EXPR: {
                // TODO: Implement
                break;
            }
            default: {
                throw Util::CommanderException("Flow Controller: Unknown expression encountered");
            }
        }
        return -1;  // TODO: Find better default return
    }

    void FlowController::_exprs(const Parser::ExprsNodePtr& node) {
        for (auto& expr : node->exprs) { _expr(expr); }
    }

    void FlowController::_prgm(const std::shared_ptr<Parser::PrgmNode>& node) {
        for (auto& stmt : node->stmts) { _stmt(stmt); }
    }

    std::any FlowController::_stmt(const Parser::StmtNodePtr& node) {
        switch (node->nodeType()) {
            case Parser::IF_STMT: {
                // TODO: Implement
                break;
            }
            case Parser::FOR_STMT: {
                // TODO: Implement
                break;
            }
            case Parser::WHILE_STMT: {
                // TODO: Implement
                break;
            }
            case Parser::DO_WHILE_STMT: {
                // TODO: Implement
                break;
            }
            case Parser::RETURN_STMT: {
                // TODO: Implement
                break;
            }
            case Parser::SCOPE_STMT: {
                // TODO: Implement
                break;
            }
            case Parser::CMD_STMT: {
                // TODO: Implement
                break;
            }
            case Parser::EXPR_STMT: {
                auto expr = std::static_pointer_cast<Parser::ExprStmtNode>(node);
                return _expr(expr->expression);
            }
            case Parser::ALIAS_STMT: {
                // TODO: Implement
                break;
            }
            default: {
                throw Util::CommanderException("Flow Controller: Unknown binary expression encountered");
            }
        }
        return nullptr;
    }

    void FlowController::_stmts(const Parser::StmtsNodePtr& node) {
        for (auto& stmt : node->stmts) { _stmt(stmt); }
    }

    std::string FlowController::_string(const Parser::StringNodePtr& node) {
        auto stringExp = std::dynamic_pointer_cast<Parser::StringExprNode>(node);
        auto stringNode = stringExp->stringNode;

        int indexLiteral = 0;
        int indexExpression = 0;
        std::string stringResult;
        while (indexLiteral < stringNode->literals.size() && indexExpression < stringNode->expressions.size()) {
            std::any const exprValue = _expr(stringNode->expressions[indexExpression]);

            stringResult.append(stringNode->literals[indexLiteral]);
            stringResult.append(_commanderTypeToString(exprValue));

            indexLiteral++;
            indexExpression++;
        }
        return stringResult;
    }

    void FlowController::_type(const Parser::TypeNodePtr& node) {
        // TODO: Implement
    }

    void FlowController::_variable(const Parser::VariableNodePtr&) {}

    std::any FlowController::_unaryOp(std::shared_ptr<Parser::UnOpExprNode>& unOp) {
        switch (unOp->opType) {
            case Parser::NEGATE: {
                auto expr = std::any_cast<CommanderInt>(_expr(unOp->expr));
                return -1 * expr;
            }
            case Parser::NOT: {
                auto expr = std::any_cast<CommanderBool>(_expr(unOp->expr));
                return !expr;
            }
            case Parser::PRE_INCREMENT: {
                // might have to update symbol table if variable
                auto expr = std::any_cast<CommanderInt>(_expr(unOp->expr));
                return ++expr;
            }
            case Parser::POST_INCREMENT: {
                // might have to update symbol table if variable
                auto expr = std::any_cast<CommanderInt>(_expr(unOp->expr));
                return expr++;
            }
            case Parser::PRE_DECREMENT: {
                // might have to update symbol table if variable
                auto expr = std::any_cast<CommanderInt>(_expr(unOp->expr));
                return --expr;
            }
            case Parser::POST_DECREMENT: {
                // might have to update symbol table if variable
                auto expr = std::any_cast<CommanderInt>(_expr(unOp->expr));
                return expr--;
            }
            default: {
                throw Util::CommanderException("Flow Controller: Unknown unary expression encountered");
            }
        }
    }

    std::any FlowController::_binaryOp(std::shared_ptr<Parser::BinOpExprNode>& binOp) {
        // TODO: Make general to any type, for now assume just using int or bool
        CommanderInt left;
        CommanderInt right;

        std::string variableName;
        if (binOp->leftExpr) { left = std::any_cast<CommanderInt>(_expr(binOp->leftExpr)); }
        if (binOp->leftVariable) {
            auto var = std::static_pointer_cast<Parser::IdentVariableNode>(binOp->leftVariable);
            variableName = var->varName;
        }
        right = std::any_cast<CommanderInt>(_expr(binOp->rightExpr));

        switch (binOp->opType) {
            case Parser::LESSER: {
                return left < right;
            }
            case Parser::GREATER: {
                return left > right;
            }
            case Parser::LESSER_EQUAL: {
                return left <= right;
            }
            case Parser::GREATER_EQUAL: {
                return left >= right;
            }
            case Parser::MODULO: {
                return left % right;
            }
            case Parser::DIVIDE: {
                if (right == 0) { throw Util::CommanderException("Divide by zero error encountered"); }
                return left / right;
            }
            case Parser::MULTIPLY: {
                return left * right;
            }
            case Parser::SUBTRACT: {
                return left - right;
            }
            case Parser::ADD: {
                return left + right;
            }
            case Parser::EXPONENTIATE: {
                return std::pow(left, right);
            }
            case Parser::AND: {
                return std::any_cast<CommanderBool>(left) && std::any_cast<CommanderBool>(right);
            }
            case Parser::OR: {
                return std::any_cast<CommanderBool>(left) || std::any_cast<CommanderBool>(right);
            }
            case Parser::SET: {
                auto variable = std::static_pointer_cast<Parser::IdentVariableNode>(binOp->leftVariable);
                std::any value = _expr(binOp->rightExpr);

                _setVariable(variable->varName, value);
                return value;
            }
            case Parser::NOT_EQUAL: {
                return left != right;
            }
            case Parser::ADD_EQUAL: {
                CommanderInt const newValue = *_symbolTable.getVariable(variableName) + right;
                _setVariable(variableName, newValue);
                return newValue;
            }
            case Parser::SUBTRACT_EQUAL: {
                CommanderInt const newValue = *_symbolTable.getVariable(variableName) - right;
                _setVariable(variableName, newValue);
                return newValue;
            }
            case Parser::MULTIPLY_EQUAL: {
                CommanderInt const newValue = *_symbolTable.getVariable(variableName) * right;
                _setVariable(variableName, newValue);
                return newValue;
            }
            case Parser::DIVIDE_EQUAL: {
                if (right == 0) { throw Util::CommanderException("Divide by zero error encountered"); }
                CommanderInt const newValue = *_symbolTable.getVariable(variableName) / right;
                _setVariable(variableName, newValue);
                return newValue;
            }
            case Parser::MODULO_EQUAL: {
                CommanderInt const newValue = *_symbolTable.getVariable(variableName) % right;
                _setVariable(variableName, newValue);
                return newValue;
            }
            case Parser::EXPONENTIATE_EQUAL: {
                CommanderInt const newValue = std::pow(*_symbolTable.getVariable(variableName), right);
                _setVariable(variableName, newValue);
                return newValue;
            }
            default: {
                throw Util::CommanderException("Flow Controller: Unknown binary expression encountered");
            }
        }
    }

    //  ==========================
    //  ||   Helper Methods     ||
    //  ==========================
    void FlowController::_runCommand() {
        // TODO: Implement
    }

    void FlowController::_setVariable(std::string name, std::any value) {
        // TODO: update when symbol table is generic
        _symbolTable.addOrUpdateVariable(std::move(name), std::any_cast<CommanderInt>(value));
    }

    std::any FlowController::_getVariable(const std::string& name) {
        int* value = _symbolTable.getVariable(name);
        if (value != nullptr) { return *value; }
        throw Util::CommanderException("Symbol Error: Not found \"" + name + "\"");
    }

    std::string FlowController::_commanderTypeToString(std::any value) {
        // TODO: Fix this.
        // This is not the right approach, send type of value as a parameter instead.
        return std::any_cast<std::string>(value);
    }

    bool FlowController::hasVariable(std::string name) { return _symbolTable.varExistsInScope(name); }

    int FlowController::getVariableValue(std::string name) { return std::any_cast<int>(_getVariable(name)); }
}  // namespace FlowController
