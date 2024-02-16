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

namespace FlowController {

    //  ==========================
    //  ||   Commander types    ||
    //  ==========================
    CommanderLambda::CommanderLambda(Parser::BindingsNodePtr bindings, Parser::StmtNodePtr body)
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
                case Parser::STRING_EXPRS: {
                    // TODO: Handle string expressions
                }
                case Parser::TYPES: {
                    _types(std::static_pointer_cast<Parser::TypesNode>(node));
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
                std::any value = _getVariable(
                        std::static_pointer_cast<Parser::IdentVariableNode>(varExp->variable)->varName);
                return value;
            }
            case Parser::ARRAY_EXPR: {
                auto arrExp = std::static_pointer_cast<Parser::ArrayExprNode>(node);
                TypeChecker::CommanderArray<std::any> array;
                for (auto& expr : arrExp->expressions->exprs) {
                    std::any const value = _expr(expr);
                    array.push_back(value);
                }
                return array;
            }
            case Parser::INDEX_EXPR: {
                // TODO: Index expressions
                return nullptr;
            }
            case Parser::TUPLE_EXPR: {
                auto tupleExp = std::static_pointer_cast<Parser::TupleExprNode>(node);
                TypeChecker::CommanderTuple tuple;
                for (auto& expr : tupleExp->expressions->exprs) { tuple.emplace_back(_expr(expr)); }
                return tuple;
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
                for (auto& arg : functionExpression->args->exprs) {
                    // args and bindings should be lined up 1 to 1
                    std::any const argValue = _expr(arg);
                    std::string const name = function.bindings->bindings[bindingIndex]->variable;

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
        for (auto& stmt : node->stmts->stmts) { _stmt(stmt); }
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
                std::any value = _expr(expr->expression);
                switch (expr->expression->type->getType()) {
                    case TypeChecker::INT:
                        // TODO: Implement method that stringifies a int
                        Util::println(std::to_string(std::any_cast<TypeChecker::CommanderInt>(value)));
                        break;
                    case TypeChecker::FLOAT:
                        // TODO: Implement method that stringifies a float
                        Util::println(std::to_string(std::any_cast<double_t>(value)));
                        break;
                    case TypeChecker::BOOL:
                        // TODO: Implement method that stringifies a bool
                        Util::println(std::to_string(std::any_cast<TypeChecker::CommanderBool>(value)));
                        break;
                    case TypeChecker::TUPLE:
                        // TODO: Implement method that stringifies a tuple and call it here
                        break;
                    case TypeChecker::ARRAY:
                        // TODO: Implement method that stringifies an array and call it here
                        break;
                    case TypeChecker::FUNCTION:
                        // TODO: Implement method that stringifies a function and call it here
                        break;
                    case TypeChecker::STRING:
                        // TODO: Implement method that stringifies a string and call it here
                        break;
                }
                return value;
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

        std::string stringResult;
        for (const Parser::ExprNodePtr& ptr : stringNode->expressions->expressions) {
            stringResult.append(_commanderTypeToString(_expr(ptr)));
        }
        return stringResult;
    }

    void FlowController::_types(const Parser::TypesNodePtr& node) {
        for (auto& type : node->types) { _type(type); }
    }

    void FlowController::_type(const Parser::TypeNodePtr& node) {
        // TODO: Implement
    }

    void FlowController::_variable(const Parser::VariableNodePtr&) {}

    std::any FlowController::_unaryOp(std::shared_ptr<Parser::UnOpExprNode>& unOp) {
        switch (unOp->opType) {
            case Parser::NEGATE: {
                auto expr = std::any_cast<TypeChecker::CommanderInt>(_expr(unOp->expr));
                return -1 * expr;
            }
            case Parser::NOT: {
                auto expr = std::any_cast<TypeChecker::CommanderBool>(_expr(unOp->expr));
                return !expr;
            }
            // TODO: Fix increment and decrement to work on variable, not expr
            case Parser::PRE_INCREMENT: {
                // might have to update symbol table if variable
                auto expr = std::any_cast<TypeChecker::CommanderInt>(_expr(unOp->expr));
                return ++expr;
            }
            case Parser::POST_INCREMENT: {
                // might have to update symbol table if variable
                auto expr = std::any_cast<TypeChecker::CommanderInt>(_expr(unOp->expr));
                return expr++;
            }
            case Parser::PRE_DECREMENT: {
                // might have to update symbol table if variable
                auto expr = std::any_cast<TypeChecker::CommanderInt>(_expr(unOp->expr));
                return --expr;
            }
            case Parser::POST_DECREMENT: {
                // might have to update symbol table if variable
                auto expr = std::any_cast<TypeChecker::CommanderInt>(_expr(unOp->expr));
                return expr--;
            }
            default: {
                throw Util::CommanderException("Flow Controller: Unknown unary expression encountered");
            }
        }
    }

    std::any FlowController::_binaryOp(std::shared_ptr<Parser::BinOpExprNode>& binOp) {
        // TODO: Make general to any type, for now assume just using int or bool
        TypeChecker::CommanderInt left;
        TypeChecker::CommanderInt right;

        std::string variableName;
        if (binOp->leftExpr) { left = std::any_cast<TypeChecker::CommanderInt>(_expr(binOp->leftExpr)); }
        if (binOp->leftVariable) {
            auto var = std::static_pointer_cast<Parser::IdentVariableNode>(binOp->leftVariable);
            variableName = var->varName;
        }
        right = std::any_cast<TypeChecker::CommanderInt>(_expr(binOp->rightExpr));

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
                return static_cast<TypeChecker::CommanderInt>(std::pow(left, right));
            }
            case Parser::AND: {
                return std::any_cast<TypeChecker::CommanderBool>(left) && std::any_cast<TypeChecker::CommanderBool>(right);
            }
            case Parser::OR: {
                return std::any_cast<TypeChecker::CommanderBool>(left) || std::any_cast<TypeChecker::CommanderBool>(right);
            }
            case Parser::SET: {
                // auto variable = std::static_pointer_cast<Parser::IdentVariableNode>(binOp->leftVariable);
                // std::any value = _expr(binOp->rightExpr);

                _setVariable(variableName, right);
                return right;
            }
            case Parser::NOT_EQUAL: {
                return left != right;
            }
            case Parser::ADD_SET: {
                TypeChecker::CommanderInt const newValue = *_symbolTable.getVariable<TypeChecker::CommanderInt>(variableName) + right;
                _setVariable(variableName, newValue);
                return newValue;
            }
            case Parser::SUBTRACT_SET: {
                TypeChecker::CommanderInt const newValue = *_symbolTable.getVariable<TypeChecker::CommanderInt>(variableName) - right;
                _setVariable(variableName, newValue);
                return newValue;
            }
            case Parser::MULTIPLY_SET: {
                TypeChecker::CommanderInt const newValue = *_symbolTable.getVariable<TypeChecker::CommanderInt>(variableName) * right;
                _setVariable(variableName, newValue);
                return newValue;
            }
            case Parser::DIVIDE_SET: {
                if (right == 0) { throw Util::CommanderException("Divide by zero error encountered"); }
                TypeChecker::CommanderInt const newValue = *_symbolTable.getVariable<TypeChecker::CommanderInt>(variableName) / right;
                _setVariable(variableName, newValue);
                return newValue;
            }
            case Parser::MODULO_SET: {
                TypeChecker::CommanderInt const newValue = *_symbolTable.getVariable<TypeChecker::CommanderInt>(variableName) % right;
                _setVariable(variableName, newValue);
                return newValue;
            }
            case Parser::EXPONENTIATE_SET: {
                TypeChecker::CommanderInt const newValue = std::pow(*_symbolTable.getVariable<TypeChecker::CommanderInt>(variableName), right);
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

    void FlowController::_setVariable(const std::string& name, std::any value) {
        // TODO: update when symbol table is generic
        _symbolTable.addOrUpdateVariable(name, std::any_cast<TypeChecker::CommanderInt>(value));
    }

    std::any FlowController::_getVariable(const std::string& name) {
        auto* value = _symbolTable.getVariable<TypeChecker::CommanderInt>(name);
        if (value != nullptr) { return static_cast<TypeChecker::CommanderInt>(*value); }
        throw Util::CommanderException("Symbol Error: Not found \"" + name + "\"");
    }

    std::string FlowController::_commanderTypeToString(std::any value) {
        // TODO: Fix this.
        // This is not the right approach, send type of value as a parameter instead.
        return std::any_cast<std::string>(value);
    }

    bool FlowController::hasVariable(const std::string& name) { return _symbolTable.varExistsInScope(name); }

    TypeChecker::CommanderInt FlowController::getVariableValue(const std::string& name) {
        return std::any_cast<TypeChecker::CommanderInt>(_getVariable(name));
    }
}  // namespace FlowController
