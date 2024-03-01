/**
 * @file flow_controller.cpp
 * @brief Implements the Flow Controller and runtime
 *
 * Node Helper Functions:
 *      TODO: Finish the following:  _type,
 * Statements:
 *      TODO: Finish the following: If, Alias
 */

#include "source/flow_controller/flow_controller.hpp"
#include "source/flow_controller/operations.hpp"


#include "source/job_runner/job_runner.hpp"
#include "source/parser/ast_node.hpp"
#include "source/parser/parser.hpp"
#include "source/type_checker/type.hpp"
#include "source/util/commander_exception.hpp"
#include "source/flow_controller/types.hpp"
#include <cmath>

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

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

    void FlowController::_bindings(const Parser::BindingsNodePtr &nodes) {
        for (auto &binding: nodes->bindings) { _binding(binding); }
    }


    CommanderTypePtr FlowController::_cmd(const Parser::CmdNodePtr &node, bool saveInfo) {
        std::vector<std::string> args;

        switch (node->nodeType()) {
            case Parser::CMD_CMD: {
                auto cmd = std::static_pointer_cast<Parser::CmdCmdNode>(node);
                args = _parseArguments(cmd->arguments);

                // Run the command
                auto job = std::make_shared<JobRunner::Process>(args, JobRunner::ProcessType::EXTERNAL, false,
                                                                saveInfo);
                auto jobResult = _runCommand(job);
                return std::make_shared<CommanderTuple>(_parseJobReturnInfo(jobResult));
            }
            case Parser::PIPE_CMD: {
                auto pipeCmd = std::static_pointer_cast<Parser::PipeCmdNode>(node);
                std::vector<std::vector<std::string>> pipeArgs;
                std::vector<JobRunner::ProcessPtr> processes;

                std::vector<Parser::CmdCmdNodePtr> jobs;
                _getJobs(pipeCmd, jobs);

                std::vector<std::string> pArgs;
                for (const auto &job: jobs) {
                    pArgs = _parseArguments(job->arguments);
                    auto process = std::make_shared<JobRunner::Process>(pArgs, JobRunner::ProcessType::EXTERNAL, false,
                                                                        saveInfo);
                    processes.emplace_back(process);
                }

                auto pipeline = std::make_shared<JobRunner::Process>(processes);
                auto jobResult = _runCommand(pipeline);
                return std::make_shared<CommanderTuple>(_parseJobReturnInfo(jobResult));
            }
            case Parser::ASYNC_CMD: {
                auto asyncCmd = std::static_pointer_cast<Parser::AsyncCmdNode>(node);
                auto cmd = std::static_pointer_cast<Parser::CmdCmdNode>(asyncCmd->cmd);
                // Parse the arguments
                args = _parseArguments(cmd->arguments);

                // Run the command
                // Note: hard-coded false for async command because we shouldn't save info for an async command
                auto job = std::make_shared<JobRunner::Process>(args, JobRunner::ProcessType::EXTERNAL, true, false);
                auto jobResult = _runCommand(job);
                return std::make_shared<CommanderTuple>(_parseJobReturnInfo(jobResult));
            }
            default:
                throw Util::CommanderException(
                        "Unknown command type encountered: " + Parser::nodeTypeToString(node->nodeType()));
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
                    std::string const argName = function->bindings->bindings[bindingIndex]->variable;
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
                auto expr = std::static_pointer_cast<Parser::CmdExprNode>(node);
                return _cmd(expr->cmd, true);
            }
            default: {
                throw Util::CommanderException("Flow Controller: Unknown expression encountered");
            }
        }
    }

    void FlowController::_exprs(const Parser::ExprsNodePtr &nodes) {
        for (auto &expr: nodes->exprs) { _expr(expr); }
    }

    void FlowController::_prgm(const std::shared_ptr<Parser::PrgmNode> &node) {
        for (auto &stmt: node->stmts->stmts) { _stmt(stmt); }
    }

    CommanderTypePtr FlowController::_stmt(const Parser::StmtNodePtr &node) {
        if (node == nullptr)
            return nullptr;

        switch (node->nodeType()) {
            case Parser::IF_STMT: {
                // TODO: Implement
                auto stmtNode = std::static_pointer_cast<Parser::IfStmtNode>(node);
                return nullptr;
            }
            case Parser::FOR_STMT: {
                auto stmtNode = std::static_pointer_cast<Parser::ForStmtNode>(node);

                _symbolTable.pushSymbolTable(); // for gets new scope
                _stmt(stmtNode->initial);

                CommanderTypePtr exprResult = _expr(stmtNode->condition);
                // Type checked?
                auto condition = std::static_pointer_cast<CommanderBool>(exprResult);

                // infinite loop error checking?
                while (condition->value) {
                    _stmt(stmtNode->body);
                    _stmt(stmtNode->update);

                    // update condition value
                    exprResult = _expr(stmtNode->condition);
                    condition = std::static_pointer_cast<CommanderBool>(exprResult);
                }

                _symbolTable.popSymbolTable(); // pop scope from for

                return nullptr;
            }
            case Parser::WHILE_STMT: {
                auto stmtNode = std::static_pointer_cast<Parser::WhileStmtNode>(node);

                _symbolTable.pushSymbolTable(); // while gets new scope

                CommanderTypePtr exprResult = _expr(stmtNode->condition);
                // Type checked?
                auto condition = std::static_pointer_cast<CommanderBool>(exprResult);

                while (condition->value) {
                    _stmt(stmtNode->body);
                    exprResult = _expr(stmtNode->condition);
                    condition = std::static_pointer_cast<CommanderBool>(exprResult);
                }

                _symbolTable.popSymbolTable(); // pop scope from while

                return nullptr;
            }
            case Parser::DO_WHILE_STMT: {
                auto stmtNode = std::static_pointer_cast<Parser::DoWhileStmtNode>(node);

                _symbolTable.pushSymbolTable(); // do-while gets new scope

                // do
                _stmt(stmtNode->body);

                CommanderTypePtr exprResult = _expr(stmtNode->condition);
                // Type checked?
                auto condition = std::static_pointer_cast<CommanderBool>(exprResult);

                while (condition->value) {
                    _stmt(stmtNode->body);
                    exprResult = _expr(stmtNode->condition);
                    condition = std::static_pointer_cast<CommanderBool>(exprResult);
                }

                _symbolTable.popSymbolTable(); // pop scope from do-while

                return nullptr;
            }
            case Parser::RETURN_STMT: {
                auto stmtNode = std::static_pointer_cast<Parser::ReturnStmtNode>(node);
                return _expr(stmtNode->retExpr);
            }
            case Parser::SCOPE_STMT: {
                auto stmtNode = std::static_pointer_cast<Parser::ScopeStmtNode>(node);
                _symbolTable.pushSymbolTable(); // new scope
                for (auto &statement: stmtNode->stmts->stmts) {
                    _stmt(statement);
                }
                _symbolTable.popSymbolTable(); // pop the created scope

            }
            case Parser::CMD_STMT: {
                auto cmd = std::static_pointer_cast<Parser::CmdStmtNode>(node);
                return _cmd(cmd->command);
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

    void FlowController::_stmts(const Parser::StmtsNodePtr &nodes) {
        for (auto &stmt: nodes->stmts) { _stmt(stmt); }
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

    CommanderTypePtr FlowController::_binaryOp(Parser::BinOpExprNodePtr &binOp) {
        CommanderTypePtr right = _expr(binOp->rightExpr);

        CommanderTypePtr left;
        Parser::IdentVariableNodePtr variable;
        if (binOp->leftExpr != nullptr)
            left = _expr(binOp->leftExpr);
        else
            variable = std::static_pointer_cast<Parser::IdentVariableNode>(binOp->leftVariable);

        switch (binOp->opType) {
            case Parser::LESSER: {
                return lesserOperation(left, right);
            }
            case Parser::GREATER: {
                return greaterOperation(left, right);
            }
            case Parser::EQUAL: {
                return equalOperation(left, right);
            }
            case Parser::NOT_EQUAL: {
                return notEqualOperation(left, right);
            }
            case Parser::LESSER_EQUAL: {
                return lesserEqualOperation(left, right);
            }
            case Parser::GREATER_EQUAL: {
                return greaterEqualOperation(left, right);
            }
            case Parser::MODULO: {
                return moduloOperation(left, right);
            }
            case Parser::DIVIDE: {
                return divideOperation(left, right);
            }
            case Parser::MULTIPLY: {
                return multiplyOperation(left, right);
            }
            case Parser::SUBTRACT: {
                return subtractOperation(left, right);
            }
            case Parser::ADD: {
                return addOperation(left, right);
            }
            case Parser::EXPONENTIATE: {
                return exponentiateOperation(left, right);
            }
            case Parser::AND: {
                return andOperation(left, right);
            }
            case Parser::OR: {
                return orOperation(left, right);
            }
            case Parser::SET: {
                _setVariable(variable->varName, right);
                return right;
            }
            case Parser::ADD_SET: {
                CommanderTypePtr const leftVar = _getVariable(variable->varName);
                CommanderTypePtr result = addOperation(leftVar, right);
                _setVariable(variable->varName, result);
                return result;
            }
            case Parser::SUBTRACT_SET: {
                CommanderTypePtr const leftVar = _getVariable(variable->varName);
                CommanderTypePtr result = subtractOperation(leftVar, right);
                _setVariable(variable->varName, result);
                return result;
            }
            case Parser::MULTIPLY_SET: {
                CommanderTypePtr const leftVar = _getVariable(variable->varName);
                CommanderTypePtr result = multiplyOperation(leftVar, right);
                _setVariable(variable->varName, result);
                return result;
            }
            case Parser::DIVIDE_SET: {
                CommanderTypePtr const leftVar = _getVariable(variable->varName);
                CommanderTypePtr result = divideOperation(leftVar, right);
                _setVariable(variable->varName, result);
                return result;
            }
            case Parser::MODULO_SET: {
                CommanderTypePtr const leftVar = _getVariable(variable->varName);
                CommanderTypePtr result = moduloOperation(leftVar, right);
                _setVariable(variable->varName, result);
                return result;
            }
            case Parser::EXPONENTIATE_SET: {
                CommanderTypePtr const leftVar = _getVariable(variable->varName);
                CommanderTypePtr result = exponentiateOperation(leftVar, right);
                _setVariable(variable->varName, result);
                return result;
            }
            default: {
                throw Util::CommanderException("Flow Controller: Unknown binary expression encountered");
            }
        }
    }

    //  ==========================
    //  ||   Helper Methods     ||
    //  ==========================
    JobRunner::JobInfo
    FlowController::_runCommand(JobRunner::ProcessPtr process) {
        JobRunner::JobRunner runner(std::move(process));
        return runner.execProcess();
    }

    void FlowController::_setVariable(const std::string &name, const CommanderTypePtr &value) {
        _symbolTable.addOrUpdateVariable(name, value);
    }

    CommanderTypePtr FlowController::_getVariable(const std::string &name) {
        if (_symbolTable.getVariable<CommanderTypePtr>(name) == nullptr)
            throw Util::CommanderException("Symbol Error: Not found \"" + name + "\"");
        return *_symbolTable.getVariable<CommanderTypePtr>(name);
    }

    std::vector<std::string> FlowController::_parseArguments(const std::vector<Parser::ASTNodePtr> &args) {
        std::vector<std::string> result;
        for (const auto &arg: args) {
            if (arg->nodeType() == Parser::ASTNodeType::VAR_EXPR) {
                auto var = std::static_pointer_cast<Parser::VarExprNode>(arg);
                result.emplace_back(_expr(var)->getStringRepresentation());
            } else if (arg->nodeType() == Parser::ASTNodeType::STRING) {
                auto string = std::static_pointer_cast<Parser::StringNode>(arg);
                result.emplace_back(_string(string));
            }
        }
        return result;
    }

    std::vector<CommanderTypePtr> FlowController::_parseJobReturnInfo(const JobRunner::JobInfo &info) {
        std::vector<CommanderTypePtr> result;
        result.emplace_back(std::make_shared<CommanderString>(std::get<0>(info)));
        result.emplace_back(std::make_shared<CommanderString>(std::get<1>(info)));
        result.emplace_back(std::make_shared<CommanderInt>(std::get<2>(info)));
        return result;
    }

    void FlowController::_getJobs(const Parser::CmdNodePtr &head, std::vector<Parser::CmdCmdNodePtr> &jobs) {
        if (head == nullptr)
            return;

        // leaf nodes are cmd_cmd nodes
        if (head->nodeType() == Parser::CMD_CMD) {
            jobs.emplace_back(std::static_pointer_cast<Parser::CmdCmdNode>(head));
            return;
        }

        if (head->nodeType() == Parser::PIPE_CMD) {
            auto tmp = std::static_pointer_cast<Parser::PipeCmdNode>(head);
            _getJobs(tmp->leftCmd, jobs);
            // in current state of parser,
            // right cmds are always leaf nodes
            jobs.emplace_back(std::static_pointer_cast<Parser::CmdCmdNode>(tmp->rightCmd));
        }
    }
}  // namespace FlowController
