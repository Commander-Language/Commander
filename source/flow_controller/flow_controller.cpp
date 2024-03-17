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


#include "source/flow_controller/types.hpp"
#include "source/job_runner/job_runner.hpp"
#include "source/parser/ast_node.hpp"
#include "source/parser/parser.hpp"
#include "source/type_checker/type.hpp"
#include "source/util/commander_exception.hpp"
#include "source/util/scan.hpp"
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
    FlowController::FlowController() {
        _symbolTable.pushSymbolTable();  // push in the global scope
    }

    void FlowController::runtime(const Parser::ASTNodeList& nodes) {
        for (auto& node : nodes) {
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
        _setVariable(node->variable, nullptr);
    }

    void FlowController::_bindings(const Parser::BindingsNodePtr& nodes) {
        for (auto& binding : nodes->bindings) { _binding(binding); }
    }


    CommanderTypePtr FlowController::_cmd(const Parser::CmdNodePtr& node, bool saveInfo) {
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
                const std::vector<std::vector<std::string>> pipeArgs;
                std::vector<JobRunner::ProcessPtr> processes;

                std::vector<Parser::CmdCmdNodePtr> jobs;
                _getJobs(pipeCmd, jobs);

                std::vector<std::string> pArgs;
                for (const auto& job : jobs) {
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
                throw Util::CommanderException("Unknown command type encountered: "
                                               + Parser::nodeTypeToString(node->nodeType()));
        }
    }

    CommanderTypePtr FlowController::_expr(const Parser::ExprNodePtr& node) {
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
                for (auto& exprs : expr->expressions->exprs) { array.push_back(_expr(exprs)); }
                return std::make_shared<CommanderArray>(array);
            }
            case Parser::INDEX_EXPR: {
                auto expr = std::static_pointer_cast<Parser::IndexExprNode>(node);
                const CommanderIntPtr index = std::static_pointer_cast<CommanderInt>(_expr(expr->index));
                CommanderTypePtr dataStructure = _expr(expr->expr);
                if (dataStructure->getType() == TypeChecker::ARRAY) {
                    const CommanderArrayPtr array = std::static_pointer_cast<CommanderArray>(dataStructure);
                    return array->values[index->value];
                }
                const CommanderTuplePtr tuple = std::static_pointer_cast<CommanderTuple>(dataStructure);
                return tuple->values[index->value];
            }
            case Parser::TUPLE_EXPR: {
                auto expr = std::static_pointer_cast<Parser::TupleExprNode>(node);
                std::vector<CommanderTypePtr> tuple;
                for (auto& exprs : expr->expressions->exprs) { tuple.emplace_back(_expr(exprs)); }
                return std::make_shared<CommanderTuple>(tuple);
            }
            case Parser::TERNARY_EXPR: {
                auto expr = std::static_pointer_cast<Parser::TernaryExprNode>(node);
                auto condition = std::static_pointer_cast<CommanderBool>(_expr(expr->condition));

                CommanderTypePtr ifTrue = _expr(expr->trueExpr);
                CommanderTypePtr ifFalse = _expr(expr->falseExpr);

                if (condition->value) return ifTrue;
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
                CommanderTypePtr returnValue = _builtin(node);
                if (returnValue) { return returnValue; }
                auto expr = std::static_pointer_cast<Parser::CallExprNode>(node);
                auto function = std::static_pointer_cast<CommanderLambda>(_expr(expr->func));

                _symbolTable.pushSymbolTable();  // new scope for function

                int bindingIndex = 0;
                for (auto& arg : expr->args->exprs) {
                    // args and bindings should be lined up 1 to 1
                    const CommanderTypePtr argValue = _expr(arg);
                    std::string const argName = function->bindings->bindings[bindingIndex]->variable;
                    _setVariable(argName, argValue);

                    bindingIndex++;
                }
                returnValue = _stmt(function->body);

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
            case Parser::API_CALL_EXPR: {
                CommanderTypePtr returnValue = _builtin(node);
                if (returnValue) { return returnValue; }
                auto callExpr = std::static_pointer_cast<Parser::ApiCallExprNode>(node);
                auto expr = _expr(callExpr->expression);
                auto function = std::static_pointer_cast<CommanderLambda>(
                        _getVariable(std::static_pointer_cast<Parser::IdentVariableNode>(callExpr->func)->varName));

                _symbolTable.pushSymbolTable();  // new scope for function
                _setVariable(function->bindings->bindings[0]->variable, expr);
                int bindingIndex = 1;
                for (auto& arg : callExpr->args->exprs) {
                    // args and bindings should be lined up 1 to 1
                    const CommanderTypePtr argValue = _expr(arg);
                    std::string const argName = function->bindings->bindings[bindingIndex]->variable;
                    _setVariable(argName, argValue);

                    bindingIndex++;
                }
                returnValue = _stmt(function->body);

                _symbolTable.popSymbolTable();  // remove function scope!
                return returnValue;
            }
            case Parser::SCAN_EXPR: {
                auto expr = std::static_pointer_cast<Parser::ScanExprNode>(node);
                auto prompt = std::static_pointer_cast<CommanderString>(_expr(expr->prompt));
                return std::make_shared<CommanderString>(Util::scan(prompt->value));
            }
            case Parser::READ_EXPR: {
                auto expr = std::static_pointer_cast<Parser::ReadExprNode>(node);
                const CommanderStringPtr path = std::static_pointer_cast<CommanderString>(_expr(expr->filePath));
                return std::make_shared<CommanderString>(CommanderString {Util::readFile(path->value)});
            }
            default: {
                throw Util::CommanderException("Flow Controller: Unknown expression encountered");
            }
        }
    }

    void FlowController::_exprs(const Parser::ExprsNodePtr& nodes) {
        for (auto& expr : nodes->exprs) { _expr(expr); }
    }

    void FlowController::_prgm(const std::shared_ptr<Parser::PrgmNode>& node) {
        for (auto& stmt : node->stmts->stmts) { _stmt(stmt); }
    }

    CommanderTypePtr FlowController::_stmt(const Parser::StmtNodePtr& node) {
        if (node == nullptr) return nullptr;

        switch (node->nodeType()) {
            case Parser::IF_STMT: {
                auto stmtNode = std::static_pointer_cast<Parser::IfStmtNode>(node);
                CommanderTypePtr conditionResult = _expr(stmtNode->condition);
                Parser::StmtNodePtr statements;
                if (std::static_pointer_cast<CommanderBool>(conditionResult)->value) {
                    statements = stmtNode->trueStmt;
                } else {
                    statements = stmtNode->falseStmt;
                }
                return _stmt(statements);
            }
            case Parser::FOR_STMT: {
                auto stmtNode = std::static_pointer_cast<Parser::ForStmtNode>(node);

                _symbolTable.pushSymbolTable();  // for gets new scope
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

                _symbolTable.popSymbolTable();  // pop scope from for

                return nullptr;
            }
            case Parser::WHILE_STMT: {
                auto stmtNode = std::static_pointer_cast<Parser::WhileStmtNode>(node);

                //_symbolTable.pushSymbolTable();  // while gets new scope

                CommanderTypePtr exprResult = _expr(stmtNode->condition);
                // Type checked?
                auto condition = std::static_pointer_cast<CommanderBool>(exprResult);

                while (condition->value) {
                    _stmt(stmtNode->body);
                    exprResult = _expr(stmtNode->condition);
                    condition = std::static_pointer_cast<CommanderBool>(exprResult);
                }

                //_symbolTable.popSymbolTable();  // pop scope from while

                return nullptr;
            }
            case Parser::DO_WHILE_STMT: {
                auto stmtNode = std::static_pointer_cast<Parser::DoWhileStmtNode>(node);

                _symbolTable.pushSymbolTable();  // do-while gets new scope

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

                _symbolTable.popSymbolTable();  // pop scope from do-while

                return nullptr;
            }
            case Parser::RETURN_STMT: {
                auto stmtNode = std::static_pointer_cast<Parser::ReturnStmtNode>(node);
                return _expr(stmtNode->retExpr);
            }
            case Parser::SCOPE_STMT: {
                auto stmtNode = std::static_pointer_cast<Parser::ScopeStmtNode>(node);
                _symbolTable.pushSymbolTable();  // new scope
                for (auto& statement : stmtNode->stmts->stmts) { _stmt(statement); }
                _symbolTable.popSymbolTable();  // pop the created scope
                return nullptr;
            }
            case Parser::CMD_STMT: {
                auto cmd = std::static_pointer_cast<Parser::CmdStmtNode>(node);
                if (Util::usingNCurses) {
                    auto returnInfo = _cmd(cmd->command, true);
                    auto tuple = std::static_pointer_cast<CommanderTuple>(returnInfo);

                    Util::print(tuple->values[0]->getStringRepresentation());
                    Util::print(tuple->values[1]->getStringRepresentation());
                    return returnInfo;
                }
                return _cmd(cmd->command);
            }
                // Util::println(std::to_string(std::any_cast<TypeChecker::CommanderBool>(value)));
            case Parser::EXPR_STMT: {
                auto expr = std::static_pointer_cast<Parser::ExprStmtNode>(node);
                return _expr(expr->expression);
            }
            case Parser::ALIAS_STMT: {
                // TODO: Implement
                throw Util::CommanderException("Flow Controller: Unimplemented statement encountered");
            }
            case Parser::IMPORT_STMT: {
                // TODO: Implement
                throw Util::CommanderException("Flow Controller: Unimplemented statement encountered");
            }
            case Parser::PRINT_STMT: {
                auto expr = std::static_pointer_cast<Parser::PrintStmtNode>(node);
                const CommanderTypePtr value = _expr(expr->expression);
                Util::print(value->getStringRepresentation());
                return nullptr;
            }
            case Parser::PRINTLN_STMT: {
                auto expr = std::static_pointer_cast<Parser::PrintlnStmtNode>(node);
                const CommanderTypePtr value = _expr(expr->expression);
                Util::println(value->getStringRepresentation());
                return nullptr;
            }
            case Parser::WRITE_STMT: {
                auto expr = std::static_pointer_cast<Parser::WriteStmtNode>(node);
                const CommanderStringPtr data = std::static_pointer_cast<CommanderString>(_expr(expr->fileData));
                const CommanderStringPtr path = std::static_pointer_cast<CommanderString>(_expr(expr->filePath));
                Util::writeToFile(data->value, path->value);
                return nullptr;
            }
            case Parser::TYPE_STMT: {
                // TODO: Implement
                throw Util::CommanderException("Flow Controller: Unimplemented statement encountered");
            }
            case Parser::FUNCTION_STMT: {
                // TODO: Implement
                throw Util::CommanderException("Flow Controller: Unimplemented statement encountered");
            }
            default: {
                throw Util::CommanderException("Flow Controller: Unknown binary expression encountered");
            }
        }
        return nullptr;
    }

    void FlowController::_stmts(const Parser::StmtsNodePtr& nodes) {
        for (auto& stmt : nodes->stmts) { _stmt(stmt); }
    }

    std::string FlowController::_string(const Parser::StringNodePtr& node) {
        // auto stringExp = std::dynamic_pointer_cast<Parser::StringExprNode>(node);
        if (node->isLiteral()) { return node->literal; }
        std::string stringResult;
        for (auto& ptr : node->expressions->expressions) { stringResult.append(_expr(ptr)->getStringRepresentation()); }
        return stringResult;
    }

    void FlowController::_types(const Parser::TypesNodePtr& node) {
        for (auto& type : node->types) { _type(type); }
    }

    void FlowController::_type(const Parser::TypeNodePtr& node) {
        // TODO: Implement
    }

    void FlowController::_variable(const Parser::VariableNodePtr&) {}

    CommanderTypePtr FlowController::_unaryOp(std::shared_ptr<Parser::UnOpExprNode>& unOp) {
        switch (unOp->opType) {
            case Parser::NEGATE: {
                // might need to set variable
                if (unOp->variable != nullptr) {
                    auto var = std::static_pointer_cast<Parser::VariableNode>(unOp->variable);
                    auto varI = std::static_pointer_cast<Parser::IdentVariableNode>(var);
                    auto value = _getVariable(varI->varName);

                    switch (value->getType()) {
                        case TypeChecker::INT: {
                            auto intType = std::static_pointer_cast<CommanderInt>(value);
                            intType->value *= -1;
                            _setVariable(varI->varName, intType);
                            return intType;
                        }
                        case TypeChecker::FLOAT: {
                            auto floatType = std::static_pointer_cast<CommanderFloat>(value);
                            floatType->value *= -1.0F;
                            _setVariable(varI->varName, floatType);
                            return floatType;
                        }
                        default:
                            throw Util::CommanderException("Trying to negate bad type "
                                                           + TypeChecker::typeToString(value->getType()));
                    }
                }

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
                        throw Util::CommanderException("Trying to negate bad type "
                                                       + TypeChecker::typeToString(expr->getType()));
                }
            }
            case Parser::NOT: {
                auto expr = std::static_pointer_cast<CommanderBool>(_expr(unOp->expr));
                // might need to set variable
                if (unOp->variable != nullptr) {
                    auto var = std::static_pointer_cast<Parser::VariableNode>(unOp->variable);
                    auto varI = std::static_pointer_cast<Parser::IdentVariableNode>(var);
                    auto value = _getVariable(varI->varName);

                    auto boolType = std::static_pointer_cast<CommanderBool>(value);
                    boolType->value = !boolType->value;
                    _setVariable(varI->varName, boolType);
                    return boolType;
                }
                switch (expr->getType()) {
                    case TypeChecker::BOOL: {
                        auto boolType = std::static_pointer_cast<CommanderBool>(expr);
                        boolType->value = !boolType->value;
                        return boolType;
                    }
                    default:
                        throw Util::CommanderException("Trying to use ! operator on bad type "
                                                       + TypeChecker::typeToString(expr->getType()));
                }
            }
            case Parser::PRE_INCREMENT: {
                // might need to set variable
                if (unOp->variable != nullptr) {
                    auto var = std::static_pointer_cast<Parser::VariableNode>(unOp->variable);
                    auto varI = std::static_pointer_cast<Parser::IdentVariableNode>(var);
                    auto value = _getVariable(varI->varName);

                    switch (value->getType()) {
                        case TypeChecker::INT: {
                            auto intType = std::static_pointer_cast<CommanderInt>(value);
                            intType->value++;
                            _setVariable(varI->varName, intType);
                            return intType;
                        }
                        case TypeChecker::FLOAT: {
                            auto floatType = std::static_pointer_cast<CommanderFloat>(value);
                            floatType->value++;
                            _setVariable(varI->varName, floatType);
                            return floatType;
                        }
                        default:
                            throw Util::CommanderException("Trying to negate bad type "
                                                           + TypeChecker::typeToString(value->getType()));
                    }
                }
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
                        throw Util::CommanderException("Trying to pre increment bad type "
                                                       + TypeChecker::typeToString(expr->getType()));
                }
            }
            case Parser::POST_INCREMENT: {
                if (unOp->variable != nullptr) {
                    auto var = std::static_pointer_cast<Parser::VariableNode>(unOp->variable);
                    auto varI = std::static_pointer_cast<Parser::IdentVariableNode>(var);
                    auto value = _getVariable(varI->varName);

                    switch (value->getType()) {
                        case TypeChecker::INT: {
                            auto intType = std::static_pointer_cast<CommanderInt>(value);
                            auto hold = std::make_shared<CommanderInt>(intType->value);
                            intType->value++;
                            _setVariable(varI->varName, intType);
                            return hold;
                        }
                        case TypeChecker::FLOAT: {
                            auto floatType = std::static_pointer_cast<CommanderFloat>(value);
                            auto hold = std::make_shared<CommanderFloat>(floatType->value);
                            floatType->value++;
                            _setVariable(varI->varName, floatType);
                            return hold;
                        }
                        default:
                            throw Util::CommanderException("Trying to negate bad type "
                                                           + TypeChecker::typeToString(value->getType()));
                    }
                }
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
                        throw Util::CommanderException("Trying to post increment bad type "
                                                       + TypeChecker::typeToString(expr->getType()));
                }
            }
            case Parser::PRE_DECREMENT: {
                // might need to set variable
                if (unOp->variable != nullptr) {
                    auto var = std::static_pointer_cast<Parser::VariableNode>(unOp->variable);
                    auto varI = std::static_pointer_cast<Parser::IdentVariableNode>(var);
                    auto value = _getVariable(varI->varName);

                    switch (value->getType()) {
                        case TypeChecker::INT: {
                            auto intType = std::static_pointer_cast<CommanderInt>(value);
                            intType->value--;
                            _setVariable(varI->varName, intType);
                            return intType;
                        }
                        case TypeChecker::FLOAT: {
                            auto floatType = std::static_pointer_cast<CommanderFloat>(value);
                            floatType->value--;
                            _setVariable(varI->varName, floatType);
                            return floatType;
                        }
                        default:
                            throw Util::CommanderException("Trying to negate bad type "
                                                           + TypeChecker::typeToString(value->getType()));
                    }
                }
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
                        throw Util::CommanderException("Trying to pre decrement bad type "
                                                       + TypeChecker::typeToString(expr->getType()));
                }
            }
            case Parser::POST_DECREMENT: {
                if (unOp->variable != nullptr) {
                    auto var = std::static_pointer_cast<Parser::VariableNode>(unOp->variable);
                    auto varI = std::static_pointer_cast<Parser::IdentVariableNode>(var);
                    auto value = _getVariable(varI->varName);

                    switch (value->getType()) {
                        case TypeChecker::INT: {
                            auto intType = std::static_pointer_cast<CommanderInt>(value);
                            auto hold = std::make_shared<CommanderInt>(intType->value);
                            intType->value--;
                            _setVariable(varI->varName, intType);
                            return hold;
                        }
                        case TypeChecker::FLOAT: {
                            auto floatType = std::static_pointer_cast<CommanderFloat>(value);
                            auto hold = std::make_shared<CommanderFloat>(floatType->value);
                            floatType->value--;
                            _setVariable(varI->varName, floatType);
                            return hold;
                        }
                        default:
                            throw Util::CommanderException("Trying to negate bad type "
                                                           + TypeChecker::typeToString(value->getType()));
                    }
                }
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
                        throw Util::CommanderException("Trying to post decrement bad type "
                                                       + TypeChecker::typeToString(expr->getType()));
                }
            }
            default: {
                throw Util::CommanderException("Flow Controller: Unknown unary expression encountered");
            }
        }
    }

    CommanderTypePtr FlowController::_binaryOp(Parser::BinOpExprNodePtr& binOp) {
        CommanderTypePtr right = _expr(binOp->rightExpr);

        CommanderTypePtr left;
        Parser::IdentVariableNodePtr variable;
        if (binOp->leftExpr != nullptr) left = _expr(binOp->leftExpr);
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

    CommanderTypePtr FlowController::_builtin(Parser::ExprNodePtr node) {
        std::string name;
        std::vector<Parser::ExprNodePtr> args;
        if (node->nodeType() == Parser::CALL_EXPR) {
            Parser::CallExprNodePtr callExpr = std::static_pointer_cast<Parser::CallExprNode>(node);
            if (callExpr->func->nodeType() != Parser::VAR_EXPR) { return nullptr; }
            name = std::static_pointer_cast<Parser::IdentVariableNode>(
                           std::static_pointer_cast<Parser::VarExprNode>(callExpr->func)->variable)
                           ->varName;
            args = callExpr->args->exprs;
        } else if (node->nodeType() == Parser::API_CALL_EXPR) {
            Parser::ApiCallExprNodePtr apiExpr = std::static_pointer_cast<Parser::ApiCallExprNode>(node);
            name = std::static_pointer_cast<Parser::IdentVariableNode>(apiExpr->func)->varName;
            args.push_back(apiExpr->expression);
            args.insert(args.end(), apiExpr->args->exprs.begin(), apiExpr->args->exprs.end());
        }
        if (name == "parseInt") { return Function::parseInt(_expr(args[0])); }
        if (name == "parseFloat") { return Function::parseFloat(_expr(args[0])); }
        if (name == "parseBool") { return Function::parseBool(_expr(args[0])); }
        if (name == "toString") { return Function::toString(_expr(args[0])); }
        if (name == "sqrt") { return Function::sqrt(_expr(args[0])); }
        if (name == "ln") { return Function::ln(_expr(args[0])); }
        if (name == "log") { return Function::log(_expr(args[0])); }
        if (name == "abs") { return Function::abs(_expr(args[0])); }
        if (name == "floor") { return Function::floor(_expr(args[0])); }
        if (name == "ceil") { return Function::ceil(_expr(args[0])); }
        if (name == "round") { return Function::round(_expr(args[0])); }
        if (name == "sin") { return Function::sin(_expr(args[0])); }
        if (name == "cos") { return Function::cos(_expr(args[0])); }
        if (name == "tan") { return Function::tan(_expr(args[0])); }
        if (name == "csc") { return Function::csc(_expr(args[0])); }
        if (name == "sec") { return Function::sec(_expr(args[0])); }
        if (name == "cot") { return Function::cot(_expr(args[0])); }
        if (name == "sinh") { return Function::sinh(_expr(args[0])); }
        if (name == "cosh") { return Function::cosh(_expr(args[0])); }
        if (name == "tanh") { return Function::tanh(_expr(args[0])); }
        if (name == "csch") { return Function::csch(_expr(args[0])); }
        if (name == "sech") { return Function::sech(_expr(args[0])); }
        if (name == "coth") { return Function::coth(_expr(args[0])); }
        if (name == "arcsin") { return Function::arcsin(_expr(args[0])); }
        if (name == "arccos") { return Function::arccos(_expr(args[0])); }
        if (name == "arctan") { return Function::arctan(_expr(args[0])); }
        if (name == "arccsc") { return Function::arccsc(_expr(args[0])); }
        if (name == "arcsec") { return Function::arcsec(_expr(args[0])); }
        if (name == "arccot") { return Function::arccot(_expr(args[0])); }
        if (name == "arcsinh") { return Function::arcsinh(_expr(args[0])); }
        if (name == "arccosh") { return Function::arccosh(_expr(args[0])); }
        if (name == "arctanh") { return Function::arctanh(_expr(args[0])); }
        if (name == "arccsch") { return Function::arccsch(_expr(args[0])); }
        if (name == "arcsech") { return Function::arcsech(_expr(args[0])); }
        if (name == "arccoth") { return Function::arccoth(_expr(args[0])); }
        if (name == "random") { return Function::randomFloat(); }
        if (name == "time") { return Function::time(); }
        if (name == "date") { return Function::date(); }
        if (name == "sleep") { return Function::sleep(std::static_pointer_cast<CommanderInt>(_expr(args[0]))); }
        if (name == "charAt") {
            return Function::charAt(std::static_pointer_cast<CommanderString>(_expr(args[0])),
                                    std::static_pointer_cast<CommanderInt>(_expr(args[1])));
        }
        if (name == "startsWith") {
            return Function::startsWith(std::static_pointer_cast<CommanderString>(_expr(args[0])),
                                        std::static_pointer_cast<CommanderString>(_expr(args[1])));
        }
        if (name == "endsWith") {
            return Function::endsWith(std::static_pointer_cast<CommanderString>(_expr(args[0])),
                                      std::static_pointer_cast<CommanderString>(_expr(args[1])));
        }
        if (name == "includes") { return Function::includes(_expr(args[0]), _expr(args[1])); }
        if (name == "indexOf") { return Function::indexOf(_expr(args[0]), _expr(args[1])); }
        if (name == "length") { return Function::length(_expr(args[0])); }
        if (name == "replace") {
            return Function::replace(std::static_pointer_cast<CommanderString>(_expr(args[0])),
                                     std::static_pointer_cast<CommanderString>(_expr(args[1])),
                                     std::static_pointer_cast<CommanderString>(_expr(args[2])));
        }
        if (name == "replaceAll") {
            return Function::replaceAll(std::static_pointer_cast<CommanderString>(_expr(args[0])),
                                        std::static_pointer_cast<CommanderString>(_expr(args[1])),
                                        std::static_pointer_cast<CommanderString>(_expr(args[2])));
        }
        if (name == "substring") {
            if (args.size() == 2) {
                return Function::substring(std::static_pointer_cast<CommanderString>(_expr(args[0])),
                                           std::static_pointer_cast<CommanderInt>(_expr(args[1])));
            } else {
                return Function::substring(std::static_pointer_cast<CommanderString>(_expr(args[0])),
                                           std::static_pointer_cast<CommanderInt>(_expr(args[1])),
                                           std::static_pointer_cast<CommanderInt>(_expr(args[2])));
            }
        }
        if (name == "trim") { return Function::trim(std::static_pointer_cast<CommanderString>(_expr(args[0]))); }
        if (name == "lower") { return Function::lower(std::static_pointer_cast<CommanderString>(_expr(args[0]))); }
        if (name == "upper") { return Function::upper(std::static_pointer_cast<CommanderString>(_expr(args[0]))); }
        if (name == "split") {
            return Function::split(std::static_pointer_cast<CommanderString>(_expr(args[0])),
                                   std::static_pointer_cast<CommanderString>(_expr(args[1])));
        }
        if (name == "sort") {
            return Function::sort(std::static_pointer_cast<CommanderArray>(_expr(args[0])),
                                  std::static_pointer_cast<CommanderLambda>(_expr(args[1])));
        }
        if (name == "filter") {
            return Function::filter(std::static_pointer_cast<CommanderArray>(_expr(args[0])),
                                    std::static_pointer_cast<CommanderLambda>(_expr(args[1])));
        }
        if (name == "map") {
            return Function::map(std::static_pointer_cast<CommanderArray>(_expr(args[0])),
                                 std::static_pointer_cast<CommanderLambda>(_expr(args[1])));
        }
        if (name == "foreach") {
            return Function::foreach(std::static_pointer_cast<CommanderArray>(_expr(args[0])),
                                     std::static_pointer_cast<CommanderLambda>(_expr(args[1])));
        }
        if (name == "append") {
            return Function::append(std::static_pointer_cast<CommanderArray>(_expr(args[0])), _expr(args[1]));
        }
        if (name == "remove") {
            return Function::remove(std::static_pointer_cast<CommanderArray>(_expr(args[0])), _expr(args[1]));
        }
        return nullptr;
    }

    //  ==========================
    //  ||   Helper Methods     ||
    //  ==========================
    JobRunner::JobInfo FlowController::_runCommand(JobRunner::ProcessPtr process) {
        JobRunner::JobRunner runner(std::move(process));
        return runner.execProcess();
    }

    void FlowController::_setVariable(const std::string& name, const CommanderTypePtr& value) {
        _symbolTable.addOrUpdateVariable(name, value);
    }

    CommanderTypePtr FlowController::_getVariable(const std::string& name) {
        if (_symbolTable.getVariable<CommanderTypePtr>(name) == nullptr)
            throw Util::CommanderException("Symbol Error: Not found \"" + name + "\"");
        return *_symbolTable.getVariable<CommanderTypePtr>(name);
    }

    std::vector<std::string> FlowController::_parseArguments(const std::vector<Parser::ASTNodePtr>& args) {
        std::vector<std::string> result;
        for (const auto& arg : args) {
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

    std::vector<CommanderTypePtr> FlowController::_parseJobReturnInfo(const JobRunner::JobInfo& info) {
        std::vector<CommanderTypePtr> result;
        result.emplace_back(std::make_shared<CommanderString>(std::get<0>(info)));
        result.emplace_back(std::make_shared<CommanderString>(std::get<1>(info)));
        result.emplace_back(std::make_shared<CommanderInt>(std::get<2>(info)));
        return result;
    }

    void FlowController::_getJobs(const Parser::CmdNodePtr& head, std::vector<Parser::CmdCmdNodePtr>& jobs) {
        if (head == nullptr) return;

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
