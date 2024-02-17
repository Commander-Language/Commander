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
#include "source/flow_controller/types.hpp"

namespace FlowController {

    //  ==========================
    //  ||    Flow Controller   ||
    //  ==========================
    FlowController::FlowController(Parser::ASTNodeList &nodes) : _nodes(std::move(nodes)) {
        _symbolTable.pushSymbolTable();  // push in the global scope
    }

    void FlowController::runtime() {
        for (auto &node: _nodes) {
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
    void FlowController::_binding(const Parser::BindingNodePtr &node) {
        // TODO: Find better default value for each type
        _setVariable(node->variable, nullptr);
    }

    void FlowController::_bindings(const Parser::BindingsNodePtr &node) {
        for (auto &binding: node->bindings) { _binding(binding); }
    }

    void FlowController::_cmd(const Parser::CmdNodePtr &node) {
        switch (node->nodeType()) {
            case Parser::CMD_CMD: {
                auto cmd = std::static_pointer_cast<Parser::CmdCmdNode>(node);
            }
            case Parser::PIPE_CMD: {
                auto pipeCmd = std::static_pointer_cast<Parser::CmdCmdNode>(node);
            }
            case Parser::ASYNC_CMD: {
                auto asyncCmd = std::static_pointer_cast<Parser::CmdCmdNode>(node);
            }
            default:
                // TODO: Better error
                throw Util::CommanderException("Not a command");
        }
    }

    CommanderTypePtr FlowController::_expr(const Parser::ExprNodePtr &node) {
        switch (node->nodeType()) {
            case Parser::INT_EXPR: {
                auto expr = std::static_pointer_cast<Parser::IntExprNode>(node);
                return std::make_shared<CommanderInt>(expr->value);
            }
            case Parser::FLOAT_EXPR: {
                auto expr = std::static_pointer_cast<Parser::FloatExprNode>(node);
                return std::make_shared<CommanderFloat>(expr->value);
            }
            case Parser::STRING_EXPR: {
                auto expr = std::static_pointer_cast<Parser::StringExprNode>(node);
                return std::make_shared<CommanderString>(_string(expr->stringNode));
            }
            case Parser::BOOL_EXPR: {
                auto expr = std::static_pointer_cast<Parser::BoolExprNode>(node);
                return std::make_shared<CommanderBool>(expr->value);
            }
            case Parser::VAR_EXPR: {
                auto expr = std::static_pointer_cast<Parser::VarExprNode>(node);
                CommanderTypePtr value = _getVariable(
                        std::static_pointer_cast<Parser::IdentVariableNode>(expr->variable)->varName);
                return value;
            }
            case Parser::ARRAY_EXPR: {
                auto expr = std::static_pointer_cast<Parser::ArrayExprNode>(node);
                std::vector<CommanderTypePtr> array;
                for (auto &exprs: expr->expressions->exprs) {
                    array.push_back(_expr(exprs));
                }
                return std::make_shared<CommanderArray>(array);
            }
            case Parser::INDEX_EXPR: {
                // TODO: Index expressions
                return nullptr;
            }
            case Parser::TUPLE_EXPR: {
                auto expr = std::static_pointer_cast<Parser::TupleExprNode>(node);
                std::vector<CommanderTypePtr> tuple;
                for (auto &exprs: expr->expressions->exprs) { tuple.emplace_back(_expr(exprs)); }
                return std::make_shared<CommanderTuple>(tuple);
            }
            case Parser::TERNARY_EXPR: {
                auto expr = std::static_pointer_cast<Parser::TernaryExprNode>(node);
                auto condition = std::static_pointer_cast<CommanderBool>(_expr(expr->condition));

                CommanderTypePtr ifTrue = _expr(expr->trueExpr);
                CommanderTypePtr ifFalse = _expr(expr->falseExpr);

                if (condition->value)
                    return ifTrue;
                return ifFalse;
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
                auto expr = std::static_pointer_cast<Parser::CallExprNode>(node);
                auto function = std::static_pointer_cast<CommanderLambda>(_expr(expr->func));

                _symbolTable.pushSymbolTable();  // new scope for function

                int bindingIndex = 0;
                for (auto &arg: expr->args->exprs) {
                    // args and bindings should be lined up 1 to 1
                    CommanderTypePtr argValue = _expr(arg);
                    std::string argName = function->bindings->bindings[bindingIndex]->variable;
                    _setVariable(argName, argValue);

                    bindingIndex++;
                }
                CommanderTypePtr returnValue = _stmt(function->body);

                _symbolTable.popSymbolTable();  // remove function scope!
                return returnValue;
            }
            case Parser::LAMBDA_EXPR: {
                auto expr = std::static_pointer_cast<Parser::LambdaExprNode>(node);
                return std::make_shared<CommanderLambda>(expr->bindings, expr->body);
            }
            case Parser::CMD_EXPR: {
                // TODO: Implement
                break;
            }
            default: {
                throw Util::CommanderException("Flow Controller: Unknown expression encountered");
            }
        }
        return {};  // TODO: Find better default return
    }

    void FlowController::_exprs(const Parser::ExprsNodePtr &node) {
        for (auto &expr: node->exprs) { _expr(expr); }
    }

    void FlowController::_prgm(const std::shared_ptr<Parser::PrgmNode> &node) {
        for (auto &stmt: node->stmts->stmts) { _stmt(stmt); }
    }

    CommanderTypePtr FlowController::_stmt(const Parser::StmtNodePtr &node) {
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
                auto cmd = std::static_pointer_cast<Parser::CmdStmtNode>(node);
                _cmd(cmd->command);
            }
                //Util::println(std::to_string(std::any_cast<TypeChecker::CommanderBool>(value)));
            case Parser::EXPR_STMT: {
                auto expr = std::static_pointer_cast<Parser::ExprStmtNode>(node);
                CommanderTypePtr value = _expr(expr->expression);
                switch (expr->expression->type->getType()) {
                    case TypeChecker::INT:
                        Util::println(std::static_pointer_cast<CommanderInt>(value)->getStringRepresentation());
                        break;
                    case TypeChecker::FLOAT:
                        Util::println(std::static_pointer_cast<CommanderFloat>(value)->getStringRepresentation());
                        break;
                    case TypeChecker::BOOL:
                        Util::println(std::static_pointer_cast<CommanderBool>(value)->getStringRepresentation());
                        break;
                    case TypeChecker::TUPLE:
                        Util::println(std::static_pointer_cast<CommanderTuple>(value)->getStringRepresentation());
                        break;
                    case TypeChecker::ARRAY:
                        Util::println(std::static_pointer_cast<CommanderArray>(value)->getStringRepresentation());
                        break;
                    case TypeChecker::FUNCTION:
                        Util::println(std::static_pointer_cast<CommanderLambda>(value)->getStringRepresentation());
                        break;
                    case TypeChecker::STRING:
                        Util::println(std::static_pointer_cast<CommanderString>(value)->getStringRepresentation());
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

    void FlowController::_stmts(const Parser::StmtsNodePtr &node) {
        for (auto &stmt: node->stmts) { _stmt(stmt); }
    }

    std::string FlowController::_string(const Parser::StringNodePtr &node) {
        auto stringExp = std::dynamic_pointer_cast<Parser::StringExprNode>(node);
        auto stringNode = stringExp->stringNode;

        std::string stringResult;
        for (auto &ptr: stringNode->expressions->expressions) {
            stringResult.append(_expr(ptr)->getStringRepresentation());
        }
        return stringResult;
    }

    void FlowController::_types(const Parser::TypesNodePtr &node) {
        for (auto &type: node->types) { _type(type); }
    }

    void FlowController::_type(const Parser::TypeNodePtr &node) {
        // TODO: Implement
    }

    void FlowController::_variable(const Parser::VariableNodePtr &) {}

    CommanderTypePtr FlowController::_unaryOp(std::shared_ptr<Parser::UnOpExprNode> &unOp) {
        switch (unOp->opType) {
            case Parser::NEGATE: {
                CommanderTypePtr const expr = _expr(unOp->expr);
                switch (expr->getType()) {
                    case TypeChecker::INT: {
                        auto intType = std::static_pointer_cast<CommanderInt>(expr);
                        intType->value *= -1;
                        return intType;
                    }
                    case TypeChecker::FLOAT: {
                        auto floatType = std::static_pointer_cast<CommanderFloat>(expr);
                        floatType->value *= -1.0F;
                        return floatType;
                    }
                    default:
                        throw Util::CommanderException(
                                "Trying to negate bad type " + TypeChecker::typeToString(expr->getType()));
                }
            }
            case Parser::NOT: {
                auto expr = std::static_pointer_cast<CommanderBool>(_expr(unOp->expr));
                switch (expr->getType()) {
                    case TypeChecker::BOOL: {
                        auto boolType = std::static_pointer_cast<CommanderBool>(expr);
                        boolType->value = !boolType->value;
                        return boolType;
                    }
                    default:
                        throw Util::CommanderException(
                                "Trying to use ! operator on bad type " + TypeChecker::typeToString(expr->getType()));
                }
            }
            case Parser::PRE_INCREMENT: {
                CommanderTypePtr const expr = _expr(unOp->expr);
                switch (expr->getType()) {
                    case TypeChecker::INT: {
                        auto intType = std::static_pointer_cast<CommanderInt>(expr);
                        intType->value++;
                        return intType;
                    }
                    case TypeChecker::FLOAT: {
                        auto floatType = std::static_pointer_cast<CommanderFloat>(expr);
                        floatType->value++;
                        return floatType;
                    }
                    default:
                        throw Util::CommanderException(
                                "Trying to pre increment bad type " + TypeChecker::typeToString(expr->getType()));
                }
            }
            case Parser::POST_INCREMENT: {
                CommanderTypePtr const expr = _expr(unOp->expr);
                switch (expr->getType()) {
                    case TypeChecker::INT: {
                        auto intType = std::static_pointer_cast<CommanderInt>(expr);
                        auto hold = std::make_shared<CommanderInt>(intType->value);
                        intType->value++;
                        return hold;
                    }
                    case TypeChecker::FLOAT: {
                        auto floatType = std::static_pointer_cast<CommanderFloat>(expr);
                        auto hold = std::make_shared<CommanderFloat>(floatType->value);
                        floatType->value++;
                        return hold;
                    }
                    default:
                        throw Util::CommanderException(
                                "Trying to post increment bad type " + TypeChecker::typeToString(expr->getType()));
                }
            }
            case Parser::PRE_DECREMENT: {
                CommanderTypePtr const expr = _expr(unOp->expr);
                switch (expr->getType()) {
                    case TypeChecker::INT: {
                        auto intType = std::static_pointer_cast<CommanderInt>(expr);
                        intType->value--;
                        return intType;
                    }
                    case TypeChecker::FLOAT: {
                        auto floatType = std::static_pointer_cast<CommanderFloat>(expr);
                        floatType->value--;
                        return floatType;
                    }
                    default:
                        throw Util::CommanderException(
                                "Trying to pre decrement bad type " + TypeChecker::typeToString(expr->getType()));
                }
            }
            case Parser::POST_DECREMENT: {
                CommanderTypePtr const expr = _expr(unOp->expr);
                switch (expr->getType()) {
                    case TypeChecker::INT: {
                        auto intType = std::static_pointer_cast<CommanderInt>(expr);
                        auto hold = std::make_shared<CommanderInt>(intType->value);
                        intType->value--;
                        return hold;
                    }
                    case TypeChecker::FLOAT: {
                        auto floatType = std::static_pointer_cast<CommanderFloat>(expr);
                        auto hold = std::make_shared<CommanderFloat>(floatType->value);
                        floatType->value--;
                        return hold;
                    }
                    default:
                        throw Util::CommanderException(
                                "Trying to post decrement bad type " + TypeChecker::typeToString(expr->getType()));
                }
            }
            default: {
                throw Util::CommanderException("Flow Controller: Unknown unary expression encountered");
            }
        }
    }

    CommanderTypePtr FlowController::_binaryOp(std::shared_ptr<Parser::BinOpExprNode> &binOp) {
        using TypeChecker::Type;

        CommanderTypePtr right = _expr(binOp->rightExpr);
        TypeChecker::Type const rightType = right->getType();

        CommanderTypePtr left;
        TypeChecker::Type leftType;

        Parser::IdentVariableNodePtr variable;
        if (binOp->leftExpr != nullptr) {
            left = _expr(binOp->leftExpr);
            leftType = left->getType();
        } else {
            variable = std::static_pointer_cast<Parser::IdentVariableNode>(binOp->leftVariable);
        }
        switch (binOp->opType) {
            case Parser::LESSER: {
                if (leftType == Type::INT && rightType == Type::INT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderBool>(leftInt->value < rightInt->value);
                }
                if (leftType == Type::INT && rightType == Type::FLOAT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderBool>(leftInt->value < rightFloat->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::INT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderBool>(leftFloat->value < rightInt->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderBool>(leftFloat->value < rightFloat->value);
                }
            }
            case Parser::GREATER: {
                if (leftType == Type::INT && rightType == Type::INT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderBool>(leftInt->value > rightInt->value);
                }
                if (leftType == Type::INT && rightType == Type::FLOAT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderBool>(leftInt->value > rightFloat->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::INT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderBool>(leftFloat->value > rightInt->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderBool>(leftFloat->value > rightFloat->value);
                }
            }
            case Parser::EQUAL: {
                if (leftType == Type::INT && rightType == Type::INT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderBool>(leftInt->value == rightInt->value);
                }
                if (leftType == Type::INT && rightType == Type::FLOAT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderBool>(leftInt->value == rightFloat->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::INT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderBool>(leftFloat->value == rightInt->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderBool>(leftFloat->value == rightFloat->value);
                }
            }
            case Parser::NOT_EQUAL: {
                if (leftType == Type::INT && rightType == Type::INT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderBool>(leftInt->value != rightInt->value);
                }
                if (leftType == Type::INT && rightType == Type::FLOAT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderBool>(leftInt->value != rightFloat->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::INT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderBool>(leftFloat->value != rightInt->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderBool>(leftFloat->value != rightFloat->value);
                }
            }
            case Parser::LESSER_EQUAL: {
                if (leftType == Type::INT && rightType == Type::INT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderBool>(leftInt->value <= rightInt->value);
                }
                if (leftType == Type::INT && rightType == Type::FLOAT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderBool>(leftInt->value <= rightFloat->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::INT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderBool>(leftFloat->value <= rightInt->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderBool>(leftFloat->value <= rightFloat->value);
                }
            }
            case Parser::GREATER_EQUAL: {
                if (leftType == Type::INT && rightType == Type::INT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderBool>(leftInt->value >= rightInt->value);
                }
                if (leftType == Type::INT && rightType == Type::FLOAT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderBool>(leftInt->value >= rightFloat->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::INT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderBool>(leftFloat->value >= rightInt->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderBool>(leftFloat->value >= rightFloat->value);
                }
            }
            case Parser::MODULO: {
                if (leftType == Type::INT && rightType == Type::INT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderInt>(leftInt->value % rightInt->value);
                }
                if (leftType == Type::INT && rightType == Type::FLOAT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    // TODO: Define expected behavior for modulus and floats, for now cast to int
                    // Probably will have to create our own modulus
                    return std::make_shared<CommanderFloat>(leftInt->value % (PrimitiveInt) rightFloat->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::INT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    // TODO: Define expected behavior for modulus and floats, for now cast to int
                    // Probably will have to create our own modulus
                    return std::make_shared<CommanderFloat>((PrimitiveInt) leftFloat->value % rightInt->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    // TODO: Define expected behavior for modulus and floats, for now cast to int
                    // Probably will have to create our own modulus
                    return std::make_shared<CommanderFloat>(
                            (PrimitiveInt) leftFloat->value % (PrimitiveInt) rightFloat->value);
                }
            }
            case Parser::DIVIDE: {
                if (rightType == Type::INT) {
                    if (std::static_pointer_cast<CommanderInt>(right)->value == 0)
                        throw Util::CommanderException("DivisionError: unable to divide by zero");
                }
                if (rightType == Type::FLOAT) {
                    if (std::static_pointer_cast<CommanderFloat>(right)->value == 0)
                        throw Util::CommanderException("DivisionError: unable to divide by zero");
                }

                if (leftType == Type::INT && rightType == Type::INT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderInt>(leftInt->value / rightInt->value);
                }
                if (leftType == Type::INT && rightType == Type::FLOAT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderFloat>(leftInt->value / rightFloat->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::INT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderFloat>(leftFloat->value / rightInt->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderFloat>(leftFloat->value / rightFloat->value);
                }
            }
            case Parser::MULTIPLY: {
                if (leftType == Type::INT && rightType == Type::INT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderInt>(leftInt->value * rightInt->value);
                }
                if (leftType == Type::INT && rightType == Type::FLOAT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderFloat>(leftInt->value * rightFloat->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::INT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderFloat>(leftFloat->value * rightInt->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderFloat>(leftFloat->value * rightFloat->value);
                }
            }
            case Parser::SUBTRACT: {
                if (leftType == Type::INT && rightType == Type::INT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderInt>(leftInt->value - rightInt->value);
                }
                if (leftType == Type::INT && rightType == Type::FLOAT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderFloat>(leftInt->value - rightFloat->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::INT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderFloat>(leftFloat->value - rightInt->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderFloat>(leftFloat->value - rightFloat->value);
                }
            }
            case Parser::ADD: {
                if (leftType == Type::INT && rightType == Type::INT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderInt>(leftInt->value + rightInt->value);
                }
                if (leftType == Type::INT && rightType == Type::FLOAT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderFloat>(leftInt->value + rightFloat->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::INT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderFloat>(leftFloat->value + rightInt->value);
                }
                if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderFloat>(leftFloat->value + rightFloat->value);
                }
            }
            case Parser::EXPONENTIATE: {
                if (leftType == Type::INT && rightType == Type::INT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderInt>(std::pow(leftInt->value, rightInt->value));
                }
                if (leftType == Type::INT && rightType == Type::FLOAT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderFloat>(std::pow(leftInt->value, rightFloat->value));
                }
                if (leftType == Type::FLOAT && rightType == Type::INT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    return std::make_shared<CommanderFloat>(std::pow(leftFloat->value, rightInt->value));
                }
                if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    return std::make_shared<CommanderFloat>(std::pow(leftFloat->value, rightFloat->value));
                }
            }
            case Parser::AND: {
                auto leftBool = std::static_pointer_cast<CommanderBool>(left);
                auto rightBool = std::static_pointer_cast<CommanderBool>(right);

                return std::make_shared<CommanderBool>(leftBool->value && rightBool->value);
            }
            case Parser::OR: {
                auto leftBool = std::static_pointer_cast<CommanderBool>(left);
                auto rightBool = std::static_pointer_cast<CommanderBool>(right);

                return std::make_shared<CommanderBool>(leftBool->value || rightBool->value);
            }
            case Parser::SET: {
                _setVariable(variable->varName, right);
                return right;
            }
            case Parser::ADD_SET: {
                CommanderTypePtr const leftVar = _getVariable(variable->varName);
                Type const leftVarType = leftVar->getType();

                if (leftVarType == Type::INT && rightType == Type::INT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(leftVar);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    auto result = std::make_shared<CommanderInt>(leftInt->value + rightInt->value);
                    _setVariable(variable->varName, result);
                    return result;
                }
                if (leftVarType == Type::INT && rightType == Type::FLOAT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(leftVar);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    auto result = std::make_shared<CommanderFloat>(leftInt->value + rightFloat->value);
                    _setVariable(variable->varName, result);
                    return result;
                }
                if (leftVarType == Type::FLOAT && rightType == Type::INT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(leftVar);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    auto result = std::make_shared<CommanderFloat>(leftFloat->value + rightInt->value);
                    _setVariable(variable->varName, result);
                    return result;
                }
                if (leftVarType == Type::FLOAT && rightType == Type::FLOAT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(leftVar);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    auto result = std::make_shared<CommanderFloat>(leftFloat->value + rightFloat->value);
                    _setVariable(variable->varName, result);
                    return result;
                }
            }
            case Parser::SUBTRACT_SET: {
                CommanderTypePtr const leftVar = _getVariable(variable->varName);
                Type const leftVarType = leftVar->getType();

                if (leftVarType == Type::INT && rightType == Type::INT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(leftVar);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    auto result = std::make_shared<CommanderInt>(leftInt->value - rightInt->value);
                    _setVariable(variable->varName, result);
                    return result;
                }
                if (leftVarType == Type::INT && rightType == Type::FLOAT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(leftVar);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    auto result = std::make_shared<CommanderFloat>(leftInt->value - rightFloat->value);
                    _setVariable(variable->varName, result);
                    return result;
                }
                if (leftVarType == Type::FLOAT && rightType == Type::INT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(leftVar);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    auto result = std::make_shared<CommanderFloat>(leftFloat->value - rightInt->value);
                    _setVariable(variable->varName, result);
                    return result;
                }
                if (leftVarType == Type::FLOAT && rightType == Type::FLOAT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(leftVar);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    auto result = std::make_shared<CommanderFloat>(leftFloat->value - rightFloat->value);
                    _setVariable(variable->varName, result);
                    return result;
                }
            }
            case Parser::MULTIPLY_SET: {
                CommanderTypePtr const leftVar = _getVariable(variable->varName);
                Type const leftVarType = leftVar->getType();

                if (leftVarType == Type::INT && rightType == Type::INT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(leftVar);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    auto result = std::make_shared<CommanderInt>(leftInt->value * rightInt->value);
                    _setVariable(variable->varName, result);
                    return result;
                }
                if (leftVarType == Type::INT && rightType == Type::FLOAT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(leftVar);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    auto result = std::make_shared<CommanderFloat>(leftInt->value * rightFloat->value);
                    _setVariable(variable->varName, result);
                    return result;
                }
                if (leftVarType == Type::FLOAT && rightType == Type::INT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(leftVar);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    auto result = std::make_shared<CommanderFloat>(leftFloat->value * rightInt->value);
                    _setVariable(variable->varName, result);
                    return result;
                }
                if (leftVarType == Type::FLOAT && rightType == Type::FLOAT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(leftVar);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    auto result = std::make_shared<CommanderFloat>(leftFloat->value * rightFloat->value);
                    _setVariable(variable->varName, result);
                    return result;
                }
            }
            case Parser::DIVIDE_SET: {
                CommanderTypePtr const leftVar = _getVariable(variable->varName);
                Type const leftVarType = leftVar->getType();

                if (rightType == Type::INT) {
                    if (std::static_pointer_cast<CommanderInt>(right)->value == 0)
                        throw Util::CommanderException("DivisionError: unable to divide by zero");
                }
                if (rightType == Type::FLOAT) {
                    if (std::static_pointer_cast<CommanderFloat>(right)->value == 0)
                        throw Util::CommanderException("DivisionError: unable to divide by zero");
                }

                if (leftVarType == Type::INT && rightType == Type::INT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(leftVar);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    auto result = std::make_shared<CommanderInt>(leftInt->value / rightInt->value);
                    _setVariable(variable->varName, result);
                    return result;
                }
                if (leftVarType == Type::INT && rightType == Type::FLOAT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(leftVar);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    auto result = std::make_shared<CommanderFloat>(leftInt->value / rightFloat->value);
                    _setVariable(variable->varName, result);
                    return result;
                }
                if (leftVarType == Type::FLOAT && rightType == Type::INT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(leftVar);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    auto result = std::make_shared<CommanderFloat>(leftFloat->value / rightInt->value);
                    _setVariable(variable->varName, result);
                    return result;
                }
                if (leftVarType == Type::FLOAT && rightType == Type::FLOAT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(leftVar);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    auto result = std::make_shared<CommanderFloat>(leftFloat->value / rightFloat->value);
                    _setVariable(variable->varName, result);
                    return result;
                }
            }
            case Parser::MODULO_SET: {
                CommanderTypePtr const leftVar = _getVariable(variable->varName);
                Type const leftVarType = leftVar->getType();

                if (leftVarType == Type::INT && rightType == Type::INT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(leftVar);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    auto result = std::make_shared<CommanderInt>(leftInt->value % rightInt->value);
                    _setVariable(variable->varName, result);
                    return result;
                }
                if (leftVarType == Type::INT && rightType == Type::FLOAT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(leftVar);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    auto result = std::make_shared<CommanderFloat>(leftInt->value % (PrimitiveInt) rightFloat->value);
                    _setVariable(variable->varName, result);
                    return result;
                }
                if (leftVarType == Type::FLOAT && rightType == Type::INT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(leftVar);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    auto result = std::make_shared<CommanderFloat>((PrimitiveInt) leftFloat->value % rightInt->value);
                    _setVariable(variable->varName, result);
                    return result;
                }
                if (leftVarType == Type::FLOAT && rightType == Type::FLOAT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(leftVar);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    auto result = std::make_shared<CommanderFloat>(
                            (PrimitiveInt) leftFloat->value % (PrimitiveInt) rightFloat->value);
                    _setVariable(variable->varName, result);
                    return result;
                }
            }
            case Parser::EXPONENTIATE_SET: {
                CommanderTypePtr const leftVar = _getVariable(variable->varName);
                Type const leftVarType = leftVar->getType();

                if (leftVarType == Type::INT && rightType == Type::INT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(leftVar);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    auto result = std::make_shared<CommanderInt>(std::pow(leftInt->value, rightInt->value));
                    _setVariable(variable->varName, result);
                    return result;
                }
                if (leftVarType == Type::INT && rightType == Type::FLOAT) {
                    auto leftInt = std::static_pointer_cast<CommanderInt>(leftVar);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    auto result = std::make_shared<CommanderFloat>(std::pow(leftInt->value, rightFloat->value));
                    _setVariable(variable->varName, result);
                    return result;
                }
                if (leftVarType == Type::FLOAT && rightType == Type::INT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(leftVar);
                    auto rightInt = std::static_pointer_cast<CommanderInt>(right);

                    auto result = std::make_shared<CommanderFloat>(std::pow(leftFloat->value, rightInt->value));
                    _setVariable(variable->varName, result);
                    return result;
                }
                if (leftVarType == Type::FLOAT && rightType == Type::FLOAT) {
                    auto leftFloat = std::static_pointer_cast<CommanderFloat>(leftVar);
                    auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

                    auto result = std::make_shared<CommanderFloat>(std::pow(leftFloat->value, rightFloat->value));
                    _setVariable(variable->varName, result);
                    return result;
                }
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

    void FlowController::_setVariable(const std::string &name, const CommanderTypePtr &value) {
        _symbolTable.addOrUpdateVariable(name, value);
    }

    CommanderTypePtr FlowController::_getVariable(const std::string &name) {
        if (_symbolTable.getVariable<CommanderTypePtr>(name) == nullptr)
            throw Util::CommanderException("Symbol Error: Not found \"" + name + "\"");
        return *_symbolTable.getVariable<CommanderTypePtr>(name);
    }
}  // namespace FlowController
