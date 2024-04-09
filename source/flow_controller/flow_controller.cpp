/**
 * @file flow_controller.cpp
 * @brief Implements the Flow Controller and runtime
 *
 * Statements:
 *      TODO: Finish the following: Alias
 */

/*
 * add(a, b, c) return a + b;
 */

#include "source/flow_controller/flow_controller.hpp"
#include "source/flow_controller/operations.hpp"

#include "source/flow_controller/types.hpp"
#if defined(__APPLE__) || defined(__linux__) || defined(__unix__)
#include "source/job_runner/job_runner_linux.hpp"
using Runner = JobRunner::JobRunnerLinux;
#elif defined(_MSC_VER)
#include "source/job_runner/job_runner_windows.hpp"
using Runner = JobRunner::JobRunnerWindows;
#else
#error OS/Compiler not supported
#endif


#include "source/console/console.hpp"
#include "source/parser/ast_node.hpp"
#include "source/parser/parser.hpp"
#include "source/type_checker/type.hpp"
#include "source/util/commander_exception.hpp"

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
                case Parser::CMD:
                case Parser::BASIC_CMD:
                case Parser::PIPE_CMD:
                case Parser::ASYNC_CMD: {
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
                case Parser::TYPES:
                case Parser::TYPE:
                case Parser::STRING_EXPRS:
                case Parser::LVALUE:
                    // Ignore these ones. Types are handled in type checker, and lvalues and string exprs are handle in
                    // _expr
                    break;
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
        const bool isBackground = node->nodeType() == Parser::ASYNC_CMD;
        switch (node->nodeType()) {
            case Parser::BASIC_CMD: {
                auto cmd = std::static_pointer_cast<Parser::BasicCmdNode>(node);
                args = _parseArguments(cmd->arguments);
                // Run the command
                auto job = std::make_shared<Process::Process>(args, Process::ProcessType::EXTERNAL, isBackground,
                                                              !isBackground && saveInfo);
                auto jobResult = _runCommand(job);
                return std::make_shared<CommanderTuple>(_parseJobReturnInfo(jobResult));
            }
            case Parser::ASYNC_CMD: {
                auto asyncCmd = std::static_pointer_cast<Parser::AsyncCmdNode>(node);
                auto cmd = std::static_pointer_cast<Parser::BasicCmdNode>(asyncCmd->cmd);
                args = _parseArguments(cmd->arguments);
                // Run the command
                auto job = std::make_shared<Process::Process>(args, Process::ProcessType::EXTERNAL, isBackground,
                                                              !isBackground && saveInfo);
                auto jobResult = _runCommand(job);
                return std::make_shared<CommanderTuple>(_parseJobReturnInfo(jobResult));
            }
            case Parser::PIPE_CMD: {
                auto pipeCmd = std::static_pointer_cast<Parser::PipeCmdNode>(node);
                const std::vector<std::vector<std::string>> pipeArgs;
                std::vector<Process::ProcessPtr> processes;

                std::vector<Parser::BasicCmdNodePtr> jobs;
                _getJobs(pipeCmd, jobs);

                std::vector<std::string> pArgs;
                for (const auto& job : jobs) {
                    pArgs = _parseArguments(job->arguments);
                    auto process = std::make_shared<Process::Process>(pArgs, Process::ProcessType::EXTERNAL,
                                                                      isBackground, !isBackground && saveInfo);
                    processes.emplace_back(process);
                }

                auto pipeline = std::make_shared<Process::Process>(processes);
                auto jobResult = _runCommand(pipeline);
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
                CommanderTypePtr value = _getVariable(expr->variable);
                return value;
            }
            case Parser::LVALUE_EXPR: {
                auto expr = std::static_pointer_cast<Parser::LValueExprNode>(node);
                return _expr(expr->expr);
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
                    if (index->value >= array->values.size()) {
                        throw Util::CommanderException("Index array out of bounds. Index: "
                                                       + std::to_string(index->value)
                                                       + ", Array Length: " + std::to_string(array->values.size()));
                    }
                    return array->values[index->value];
                }
                const CommanderTuplePtr tuple = std::static_pointer_cast<CommanderTuple>(dataStructure);
                if (index->value >= tuple->values.size()) {
                    throw Util::CommanderException("Index tuple out of bounds. Index: " + std::to_string(index->value)
                                                   + ", Tuple Length: " + std::to_string(tuple->values.size()));
                }
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
            case Parser::LAMBDA_EXPR: {
                auto expr = std::static_pointer_cast<Parser::LambdaExprNode>(node);
                return std::make_shared<CommanderLambda>(
                        expr->bindings, expr->body,
                        std::static_pointer_cast<TypeChecker::FunctionTy>(expr->type)->returnType);
            }
            case Parser::CMD_EXPR: {
                auto expr = std::static_pointer_cast<Parser::CmdExprNode>(node);
                return _cmd(expr->cmd, true);
            }
            case Parser::CALL_EXPR:
            case Parser::API_CALL_EXPR: {
                CommanderTypePtr returnValue = _builtin(node);
                if (returnValue) { return returnValue; }
                std::vector<Parser::ExprNodePtr> exprs;
                std::shared_ptr<CommanderLambda> function;
                if (node->nodeType() == Parser::CALL_EXPR) {
                    auto expr = std::static_pointer_cast<Parser::CallExprNode>(node);
                    exprs = expr->args->exprs;
                    function = std::static_pointer_cast<CommanderLambda>(_expr(expr->func));
                } else {
                    auto callExpr = std::static_pointer_cast<Parser::ApiCallExprNode>(node);
                    exprs = callExpr->args->exprs;
                    function = std::static_pointer_cast<CommanderLambda>(_getVariable(callExpr->func));
                }
                _symbolTable.pushSymbolTable();  // new scope for function
                int bindingIndex = 0;
                for (auto& arg : exprs) {
                    // args and bindings should be lined up 1 to 1
                    const CommanderTypePtr argValue = _expr(arg);
                    std::string const argName = function->bindings->bindings[bindingIndex++]->variable;
                    _setVariable(argName, _copy(argValue));
                }
                returnValue = _stmt(function->body);

                _symbolTable.popSymbolTable();  // remove function scope!
                return returnValue;
            }
            case Parser::SCAN_EXPR: {
                auto expr = std::static_pointer_cast<Parser::ScanExprNode>(node);
                auto prompt = std::static_pointer_cast<CommanderString>(_expr(expr->prompt));
                return std::make_shared<CommanderString>(Console::readLine(prompt->value));
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
                const CommanderTypePtr conditionResult = _expr(stmtNode->condition);
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

                for (_expr(stmtNode->initial);
                     std::static_pointer_cast<CommanderBool>(_expr(stmtNode->condition))->value;
                     _expr(stmtNode->update)) {
                    _stmt(stmtNode->body);
                    if (_break) {
                        _break = false;
                        break;
                    }
                    if (_continue) { _continue = false; }
                }

                _symbolTable.popSymbolTable();  // pop scope from for

                return nullptr;
            }
            case Parser::WHILE_STMT: {
                auto stmtNode = std::static_pointer_cast<Parser::WhileStmtNode>(node);

                while (std::static_pointer_cast<CommanderBool>(_expr(stmtNode->condition))->value) {
                    _stmt(stmtNode->body);
                    if (_break) {
                        _break = false;
                        break;
                    }
                    if (_continue) { _continue = false; }
                }

                return nullptr;
            }
            case Parser::DO_WHILE_STMT: {
                auto stmtNode = std::static_pointer_cast<Parser::DoWhileStmtNode>(node);

                do {
                    _stmt(stmtNode->body);
                    if (_break) {
                        _break = false;
                        break;
                    }
                    if (_continue) { _continue = false; }
                } while (std::static_pointer_cast<CommanderBool>(_expr(stmtNode->condition))->value);

                return nullptr;
            }
            case Parser::RETURN_STMT: {
                auto stmtNode = std::static_pointer_cast<Parser::ReturnStmtNode>(node);
                return _expr(stmtNode->retExpr);
            }
            case Parser::SCOPE_STMT: {
                auto stmtNode = std::static_pointer_cast<Parser::ScopeStmtNode>(node);
                _symbolTable.pushSymbolTable();  // new scope
                CommanderTypePtr returnValue = std::make_shared<CommanderTuple>(std::vector<CommanderTypePtr> {});
                for (auto& statement : stmtNode->stmts->stmts) {
                    if (statement->nodeType() == Parser::CONTINUE_STMT || statement->nodeType() == Parser::BREAK_STMT
                        || _break || _continue) {
                        _break = _break || statement->nodeType() == Parser::BREAK_STMT;
                        _continue = _continue || statement->nodeType() == Parser::CONTINUE_STMT;
                        break;
                    }
                    if (statement->nodeType() == Parser::RETURN_STMT) {
                        returnValue = _expr(std::static_pointer_cast<Parser::ReturnStmtNode>(statement)->retExpr);
                    } else {
                        _stmt(statement);
                    }
                }
                _symbolTable.popSymbolTable();  // pop the created scope
                return returnValue;
            }
            case Parser::CMD_STMT: {
                auto cmd = std::static_pointer_cast<Parser::CmdStmtNode>(node);
                return _cmd(cmd->command);
            }
            case Parser::EXPR_STMT: {
                auto expr = std::static_pointer_cast<Parser::ExprStmtNode>(node);
                return _expr(expr->expression);
            }
            case Parser::ALIAS_STMT: {
                auto stmt = std::static_pointer_cast<Parser::AliasStmtNode>(node);
                _setVariable(stmt->alias,
                             std::make_shared<CommanderCommand>(stmt->command, _getCommandString(stmt->command)));
                return nullptr;
            }
            case Parser::IMPORT_STMT: {
                auto stmt = std::static_pointer_cast<Parser::ImportStmtNode>(node);
                runtime(stmt->prgm);
                return nullptr;
            }
            case Parser::PRINT_STMT: {
                auto stmt = std::static_pointer_cast<Parser::PrintStmtNode>(node);
                const CommanderTypePtr value = _expr(stmt->expression);
                std::cout << value->getStringRepresentation();
                return nullptr;
            }
            case Parser::PRINTLN_STMT: {
                auto stmt = std::static_pointer_cast<Parser::PrintlnStmtNode>(node);
                const CommanderTypePtr value = _expr(stmt->expression);
                std::cout << value->getStringRepresentation() << "\n";
                return nullptr;
            }
            case Parser::WRITE_STMT: {
                auto expr = std::static_pointer_cast<Parser::WriteStmtNode>(node);
                const CommanderStringPtr data = std::static_pointer_cast<CommanderString>(_expr(expr->fileData));
                const CommanderStringPtr path = std::static_pointer_cast<CommanderString>(_expr(expr->filePath));
                Util::writeToFile(data->value, path->value);
                return nullptr;
            }
            case Parser::TYPE_STMT:
            case Parser::BREAK_STMT:
            case Parser::CONTINUE_STMT:
                // Ignore these statements
                return nullptr;
            case Parser::TIMEOUT_STMT:
                // TODO: Implement
                return nullptr;
            case Parser::ASSERT_STMT: {
                auto stmt = std::static_pointer_cast<Parser::AssertStmtNode>(node);
                const CommanderBoolPtr value = std::static_pointer_cast<CommanderBool>(_expr(stmt->expr));
                if (!value->value) { throw Util::CommanderException("Assertion Error: " + _string(stmt->message)); }
                return nullptr;
            }
            case Parser::FUNCTION_STMT: {
                auto func = std::static_pointer_cast<Parser::FunctionStmtNode>(node);
                _setVariable(func->name,
                             std::make_shared<CommanderLambda>(func->bindings, func->body, func->returnType->type));
                return nullptr;
            }
            default: {
                throw Util::CommanderException("Flow Controller: Unknown statement encountered");
            }
        }
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

    CommanderTypePtr FlowController::_unaryOp(std::shared_ptr<Parser::UnOpExprNode>& unOp) {
        switch (unOp->opType) {
            case Parser::NEGATE: {
                CommanderTypePtr const expr = _expr(std::static_pointer_cast<Parser::ExprNode>(unOp->node));
                switch (expr->getType()) {
                    case TypeChecker::INT: {
                        return std::make_shared<CommanderInt>(-std::static_pointer_cast<CommanderInt>(expr)->value);
                    }
                    case TypeChecker::FLOAT: {
                        return std::make_shared<CommanderFloat>(-std::static_pointer_cast<CommanderFloat>(expr)->value);
                    }
                    default:
                        throw Util::CommanderException("Trying to negate bad type "
                                                       + TypeChecker::typeToString(expr->getType()));
                }
            }
            case Parser::NOT: {
                auto expr = std::static_pointer_cast<CommanderBool>(
                        _expr(std::static_pointer_cast<Parser::ExprNode>(unOp->node)));
                switch (expr->getType()) {
                    case TypeChecker::BOOL: {
                        return std::make_shared<CommanderBool>(!std::static_pointer_cast<CommanderBool>(expr)->value);
                    }
                    default:
                        throw Util::CommanderException("Trying to use ! operator on bad type "
                                                       + TypeChecker::typeToString(expr->getType()));
                }
            }
            case Parser::PRE_INCREMENT:
            case Parser::POST_INCREMENT:
            case Parser::PRE_DECREMENT:
            case Parser::POST_DECREMENT: {
                if (unOp->node->nodeType() == Parser::VAR_LVALUE) {
                    const std::string varName = std::static_pointer_cast<Parser::VarLValueNode>(unOp->node)->variable;
                    auto value = _getVariable(varName);
                    switch (value->getType()) {
                        case TypeChecker::INT: {
                            auto intType = std::static_pointer_cast<CommanderInt>(value);
                            switch (unOp->opType) {
                                case Parser::PRE_INCREMENT:
                                case Parser::POST_INCREMENT:
                                    intType->value++;
                                    break;
                                case Parser::PRE_DECREMENT:
                                case Parser::POST_DECREMENT:
                                    intType->value--;
                                    break;
                                default:
                                    break;
                            }
                            _setVariable(varName, intType);
                            if (unOp->opType == Parser::POST_DECREMENT) {
                                return std::make_shared<CommanderInt>(intType->value + 1);
                            }
                            if (unOp->opType == Parser::POST_INCREMENT) {
                                return std::make_shared<CommanderInt>(intType->value - 1);
                            }
                            return intType;
                        }
                        case TypeChecker::FLOAT: {
                            auto floatType = std::static_pointer_cast<CommanderFloat>(value);
                            switch (unOp->opType) {
                                case Parser::PRE_INCREMENT:
                                case Parser::POST_INCREMENT:
                                    floatType->value++;
                                    break;
                                case Parser::PRE_DECREMENT:
                                case Parser::POST_DECREMENT:
                                    floatType->value--;
                                    break;
                                default:
                                    break;
                            }
                            _setVariable(varName, floatType);
                            if (unOp->opType == Parser::POST_DECREMENT) {
                                return std::make_shared<CommanderFloat>(floatType->value + 1.0);
                            }
                            if (unOp->opType == Parser::POST_INCREMENT) {
                                return std::make_shared<CommanderFloat>(floatType->value - 1.0);
                            }
                            return floatType;
                        }
                        default:
                            throw Util::CommanderException("Trying to increment or decrement bad type "
                                                           + TypeChecker::typeToString(value->getType()));
                    }
                }
                Parser::IndexLValueNodePtr indexLValue = std::static_pointer_cast<Parser::IndexLValueNode>(unOp->node);
                CommanderIntPtr index = std::static_pointer_cast<CommanderInt>(_expr(indexLValue->index));
                CommanderTypePtr dataStructure = _expr(std::make_shared<Parser::IndexExprNode>(indexLValue));
                std::vector<CommanderTypePtr>& values
                        = dataStructure->getType() == TypeChecker::ARRAY
                                ? std::static_pointer_cast<CommanderArray>(dataStructure)->values
                                : std::static_pointer_cast<CommanderTuple>(dataStructure)->values;
                CommanderTypePtr value = values[index->value];
                switch (value->getType()) {
                    case TypeChecker::INT: {
                        auto intType = std::static_pointer_cast<CommanderInt>(value);
                        switch (unOp->opType) {
                            case Parser::PRE_INCREMENT:
                            case Parser::POST_INCREMENT:
                                intType->value++;
                                break;
                            case Parser::PRE_DECREMENT:
                            case Parser::POST_DECREMENT:
                                intType->value--;
                                break;
                            default:
                                break;
                        }
                        values[index->value] = intType;
                        if (unOp->opType == Parser::POST_DECREMENT) {
                            return std::make_shared<CommanderInt>(intType->value + 1);
                        }
                        if (unOp->opType == Parser::POST_INCREMENT) {
                            return std::make_shared<CommanderInt>(intType->value - 1);
                        }
                        return intType;
                    }
                    case TypeChecker::FLOAT: {
                        auto floatType = std::static_pointer_cast<CommanderFloat>(value);
                        switch (unOp->opType) {
                            case Parser::PRE_INCREMENT:
                            case Parser::POST_INCREMENT:
                                floatType->value++;
                                break;
                            case Parser::PRE_DECREMENT:
                            case Parser::POST_DECREMENT:
                                floatType->value--;
                                break;
                            default:
                                break;
                        }
                        values[index->value] = floatType;
                        if (unOp->opType == Parser::POST_DECREMENT) {
                            return std::make_shared<CommanderFloat>(floatType->value + 1.0);
                        }
                        if (unOp->opType == Parser::POST_INCREMENT) {
                            return std::make_shared<CommanderFloat>(floatType->value - 1.0);
                        }
                        return floatType;
                    }
                    default:
                        throw Util::CommanderException("Trying to increment or decrement bad type "
                                                       + TypeChecker::typeToString(value->getType()));
                }
            }
            default: {
                throw Util::CommanderException("Flow Controller: Unknown unary expression encountered");
            }
        }
    }

    CommanderTypePtr FlowController::_binaryOp(Parser::BinOpExprNodePtr& binOp) {
        CommanderTypePtr right = _expr(binOp->right);

        switch (binOp->opType) {
            case Parser::LESSER: {
                return lesserOperation(_expr(std::static_pointer_cast<Parser::ExprNode>(binOp->left)), right);
            }
            case Parser::GREATER: {
                return greaterOperation(_expr(std::static_pointer_cast<Parser::ExprNode>(binOp->left)), right);
            }
            case Parser::EQUAL: {
                return equalOperation(_expr(std::static_pointer_cast<Parser::ExprNode>(binOp->left)), right);
            }
            case Parser::NOT_EQUAL: {
                return notEqualOperation(_expr(std::static_pointer_cast<Parser::ExprNode>(binOp->left)), right);
            }
            case Parser::LESSER_EQUAL: {
                return lesserEqualOperation(_expr(std::static_pointer_cast<Parser::ExprNode>(binOp->left)), right);
            }
            case Parser::GREATER_EQUAL: {
                return greaterEqualOperation(_expr(std::static_pointer_cast<Parser::ExprNode>(binOp->left)), right);
            }
            case Parser::MODULO: {
                return moduloOperation(_expr(std::static_pointer_cast<Parser::ExprNode>(binOp->left)), right);
            }
            case Parser::DIVIDE: {
                return divideOperation(_expr(std::static_pointer_cast<Parser::ExprNode>(binOp->left)), right);
            }
            case Parser::MULTIPLY: {
                return multiplyOperation(_expr(std::static_pointer_cast<Parser::ExprNode>(binOp->left)), right);
            }
            case Parser::SUBTRACT: {
                return subtractOperation(_expr(std::static_pointer_cast<Parser::ExprNode>(binOp->left)), right);
            }
            case Parser::ADD: {
                return addOperation(_expr(std::static_pointer_cast<Parser::ExprNode>(binOp->left)), right);
            }
            case Parser::EXPONENTIATE: {
                return exponentiateOperation(_expr(std::static_pointer_cast<Parser::ExprNode>(binOp->left)), right);
            }
            case Parser::AND: {
                return andOperation(_expr(std::static_pointer_cast<Parser::ExprNode>(binOp->left)), right);
            }
            case Parser::OR: {
                return orOperation(_expr(std::static_pointer_cast<Parser::ExprNode>(binOp->left)), right);
            }
            case Parser::SET:
            case Parser::ADD_SET:
            case Parser::SUBTRACT_SET:
            case Parser::MULTIPLY_SET:
            case Parser::DIVIDE_SET:
            case Parser::MODULO_SET:
            case Parser::EXPONENTIATE_SET: {
                if (binOp->left->nodeType() == Parser::VAR_LVALUE || binOp->left->nodeType() == Parser::BINDING) {
                    std::string varName = binOp->left->nodeType() == Parser::VAR_LVALUE
                                                ? std::static_pointer_cast<Parser::VarLValueNode>(binOp->left)->variable
                                                : std::static_pointer_cast<Parser::BindingNode>(binOp->left)->variable;
                    if (binOp->opType == Parser::SET) {
                        _setVariable(varName, right);
                        return right;
                    }
                    CommanderTypePtr result;
                    CommanderTypePtr const leftVar = _getVariable(varName);
                    switch (binOp->opType) {
                        case Parser::ADD_SET:
                            result = addOperation(leftVar, right);
                            break;
                        case Parser::SUBTRACT_SET:
                            result = subtractOperation(leftVar, right);
                            break;
                        case Parser::MULTIPLY_SET:
                            result = multiplyOperation(leftVar, right);
                            break;
                        case Parser::DIVIDE_SET:
                            result = divideOperation(leftVar, right);
                            break;
                        case Parser::MODULO_SET:
                            result = moduloOperation(leftVar, right);
                            break;
                        case Parser::EXPONENTIATE_SET:
                            result = exponentiateOperation(leftVar, right);
                            break;
                        default:
                            break;
                    }
                    _setVariable(varName, result);
                    return result;
                }
                Parser::IndexLValueNodePtr indexLValue = std::static_pointer_cast<Parser::IndexLValueNode>(binOp->left);
                CommanderIntPtr index = std::static_pointer_cast<CommanderInt>(_expr(indexLValue->index));
                CommanderTypePtr dataStructure = _expr(std::make_shared<Parser::LValueExprNode>(indexLValue->lvalue));
                std::vector<CommanderTypePtr>& values
                        = dataStructure->getType() == TypeChecker::ARRAY
                                ? std::static_pointer_cast<CommanderArray>(dataStructure)->values
                                : std::static_pointer_cast<CommanderTuple>(dataStructure)->values;
                if (binOp->opType == Parser::SET) {
                    values[index->value] = right;
                    return right;
                }
                CommanderTypePtr result;
                switch (binOp->opType) {
                    case Parser::ADD_SET:
                        result = addOperation(values[index->value], right);
                        break;
                    case Parser::SUBTRACT_SET:
                        result = subtractOperation(values[index->value], right);
                        break;
                    case Parser::MULTIPLY_SET:
                        result = multiplyOperation(values[index->value], right);
                        break;
                    case Parser::DIVIDE_SET:
                        result = divideOperation(values[index->value], right);
                        break;
                    case Parser::MODULO_SET:
                        result = moduloOperation(values[index->value], right);
                        break;
                    case Parser::EXPONENTIATE_SET:
                        result = exponentiateOperation(values[index->value], right);
                        break;
                    default:
                        break;
                }
                values[index->value] = result;
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
            if (callExpr->func->nodeType() != Parser::VAR_EXPR
                && (callExpr->func->nodeType() != Parser::LVALUE_EXPR
                    || std::static_pointer_cast<Parser::LValueExprNode>(callExpr->func)->expr->nodeType()
                               != Parser::VAR_EXPR)) {
                return nullptr;
            }
            name = callExpr->func->nodeType() == Parser::VAR_EXPR
                         ? std::static_pointer_cast<Parser::VarExprNode>(callExpr->func)->variable
                         : std::static_pointer_cast<Parser::VarExprNode>(
                                   std::static_pointer_cast<Parser::LValueExprNode>(callExpr->func)->expr)
                                   ->variable;
            args = callExpr->args->exprs;
        } else if (node->nodeType() == Parser::API_CALL_EXPR) {
            Parser::ApiCallExprNodePtr apiExpr = std::static_pointer_cast<Parser::ApiCallExprNode>(node);
            name = apiExpr->func;
            args = apiExpr->args->exprs;
        }
        if (name == "parseInt") { return Function::parseInt(_copy(_expr(args[0]))); }
        if (name == "parseFloat") { return Function::parseFloat(_copy(_expr(args[0]))); }
        if (name == "parseBool") { return Function::parseBool(_copy(_expr(args[0]))); }
        if (name == "toString") { return Function::toString(_copy(_expr(args[0]))); }
        if (name == "sqrt") { return Function::sqrt(_copy(_expr(args[0]))); }
        if (name == "ln") { return Function::ln(_copy(_expr(args[0]))); }
        if (name == "log") { return Function::log(_copy(_expr(args[0]))); }
        if (name == "abs") { return Function::abs(_copy(_expr(args[0]))); }
        if (name == "floor") { return Function::floor(_copy(_expr(args[0]))); }
        if (name == "ceil") { return Function::ceil(_copy(_expr(args[0]))); }
        if (name == "round") { return Function::round(_copy(_expr(args[0]))); }
        if (name == "sin") { return Function::sin(_copy(_expr(args[0]))); }
        if (name == "cos") { return Function::cos(_copy(_expr(args[0]))); }
        if (name == "tan") { return Function::tan(_copy(_expr(args[0]))); }
        if (name == "csc") { return Function::csc(_copy(_expr(args[0]))); }
        if (name == "sec") { return Function::sec(_copy(_expr(args[0]))); }
        if (name == "cot") { return Function::cot(_copy(_expr(args[0]))); }
        if (name == "sinh") { return Function::sinh(_copy(_expr(args[0]))); }
        if (name == "cosh") { return Function::cosh(_copy(_expr(args[0]))); }
        if (name == "tanh") { return Function::tanh(_copy(_expr(args[0]))); }
        if (name == "csch") { return Function::csch(_copy(_expr(args[0]))); }
        if (name == "sech") { return Function::sech(_copy(_expr(args[0]))); }
        if (name == "coth") { return Function::coth(_copy(_expr(args[0]))); }
        if (name == "arcsin") { return Function::arcsin(_copy(_expr(args[0]))); }
        if (name == "arccos") { return Function::arccos(_copy(_expr(args[0]))); }
        if (name == "arctan") { return Function::arctan(_copy(_expr(args[0]))); }
        if (name == "arccsc") { return Function::arccsc(_copy(_expr(args[0]))); }
        if (name == "arcsec") { return Function::arcsec(_copy(_expr(args[0]))); }
        if (name == "arccot") { return Function::arccot(_copy(_expr(args[0]))); }
        if (name == "arcsinh") { return Function::arcsinh(_copy(_expr(args[0]))); }
        if (name == "arccosh") { return Function::arccosh(_copy(_expr(args[0]))); }
        if (name == "arctanh") { return Function::arctanh(_copy(_expr(args[0]))); }
        if (name == "arccsch") { return Function::arccsch(_copy(_expr(args[0]))); }
        if (name == "arcsech") { return Function::arcsech(_copy(_expr(args[0]))); }
        if (name == "arccoth") { return Function::arccoth(_copy(_expr(args[0]))); }
        if (name == "random") { return Function::randomFloat(); }
        if (name == "time") { return Function::time(); }
        if (name == "date") { return Function::date(); }
        if (name == "sleep") { return Function::sleep(std::static_pointer_cast<CommanderInt>(_copy(_expr(args[0])))); }
        if (name == "charAt") {
            return Function::charAt(std::static_pointer_cast<CommanderString>(_copy(_expr(args[0]))),
                                    std::static_pointer_cast<CommanderInt>(_copy(_expr(args[1]))));
        }
        if (name == "startsWith") {
            return Function::startsWith(std::static_pointer_cast<CommanderString>(_copy(_expr(args[0]))),
                                        std::static_pointer_cast<CommanderString>(_copy(_expr(args[1]))));
        }
        if (name == "endsWith") {
            return Function::endsWith(std::static_pointer_cast<CommanderString>(_copy(_expr(args[0]))),
                                      std::static_pointer_cast<CommanderString>(_copy(_expr(args[1]))));
        }
        if (name == "includes") { return Function::includes(_copy(_expr(args[0])), _copy(_expr(args[1]))); }
        if (name == "indexOf") { return Function::indexOf(_copy(_expr(args[0])), _copy(_expr(args[1]))); }
        if (name == "length") { return Function::length(_copy(_expr(args[0]))); }
        if (name == "replace") {
            return Function::replace(std::static_pointer_cast<CommanderString>(_copy(_expr(args[0]))),
                                     std::static_pointer_cast<CommanderString>(_copy(_expr(args[1]))),
                                     std::static_pointer_cast<CommanderString>(_copy(_expr(args[2]))));
        }
        if (name == "replaceAll") {
            return Function::replaceAll(std::static_pointer_cast<CommanderString>(_copy(_expr(args[0]))),
                                        std::static_pointer_cast<CommanderString>(_copy(_expr(args[1]))),
                                        std::static_pointer_cast<CommanderString>(_copy(_expr(args[2]))));
        }
        if (name == "substring") {
            if (args.size() == 2) {
                return Function::substring(std::static_pointer_cast<CommanderString>(_copy(_expr(args[0]))),
                                           std::static_pointer_cast<CommanderInt>(_copy(_expr(args[1]))));
            } else {
                return Function::substring(std::static_pointer_cast<CommanderString>(_copy(_expr(args[0]))),
                                           std::static_pointer_cast<CommanderInt>(_copy(_expr(args[1]))),
                                           std::static_pointer_cast<CommanderInt>(_copy(_expr(args[2]))));
            }
        }
        if (name == "trim") { return Function::trim(std::static_pointer_cast<CommanderString>(_copy(_expr(args[0])))); }
        if (name == "lower") {
            return Function::lower(std::static_pointer_cast<CommanderString>(_copy(_expr(args[0]))));
        }
        if (name == "upper") {
            return Function::upper(std::static_pointer_cast<CommanderString>(_copy(_expr(args[0]))));
        }
        if (name == "split") {
            return Function::split(std::static_pointer_cast<CommanderString>(_copy(_expr(args[0]))),
                                   std::static_pointer_cast<CommanderString>(_copy(_expr(args[1]))));
        }
        if (name == "sort") {
            return Function::sort(std::static_pointer_cast<CommanderArray>(_copy(_expr(args[0]))),
                                  std::static_pointer_cast<CommanderLambda>(_copy(_expr(args[1]))));
        }
        if (name == "filter") {
            return Function::filter(std::static_pointer_cast<CommanderArray>(_copy(_expr(args[0]))),
                                    std::static_pointer_cast<CommanderLambda>(_copy(_expr(args[1]))));
        }
        if (name == "map") {
            return Function::map(std::static_pointer_cast<CommanderArray>(_copy(_expr(args[0]))),
                                 std::static_pointer_cast<CommanderLambda>(_copy(_expr(args[1]))));
        }
        if (name == "foreach") {
            return Function::foreach(std::static_pointer_cast<CommanderArray>(_copy(_expr(args[0]))),
                                     std::static_pointer_cast<CommanderLambda>(_copy(_expr(args[1]))));
        }
        if (name == "append") {
            return Function::append(std::static_pointer_cast<CommanderArray>(_copy(_expr(args[0]))),
                                    _copy(_expr(args[1])));
        }
        if (name == "remove") {
            return Function::remove(std::static_pointer_cast<CommanderArray>(_copy(_expr(args[0]))),
                                    _copy(_expr(args[1])));
        }
        return nullptr;
    }

    //  ==========================
    //  ||   Helper Methods     ||
    //  ==========================
    JobRunner::JobInfo FlowController::_runCommand(Process::ProcessPtr process) {
        Runner runner(std::move(process));
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

        // check if first arg is an alias, if so replace
        if (_symbolTable.varExistsInScope(result[0])) {
            auto alias = std::static_pointer_cast<CommanderCommand>(
                    *_symbolTable.getVariable<CommanderTypePtr>(result[0]));
            std::vector<std::string> aliasArgs = _parseArguments(
                    std::static_pointer_cast<Parser::BasicCmdNode>(alias->cmdNode)->arguments);
            result.erase(result.begin());  // remove the alias name!
            result.insert(result.begin(), aliasArgs.begin(), aliasArgs.end());
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

    void FlowController::_getJobs(const Parser::CmdNodePtr& head, std::vector<Parser::BasicCmdNodePtr>& jobs) {
        if (head == nullptr) return;

        // leaf nodes are basic_cmd nodes
        if (head->nodeType() == Parser::BASIC_CMD) {
            jobs.emplace_back(std::static_pointer_cast<Parser::BasicCmdNode>(head));
            return;
        }

        if (head->nodeType() == Parser::PIPE_CMD) {
            auto tmp = std::static_pointer_cast<Parser::PipeCmdNode>(head);
            _getJobs(tmp->leftCmd, jobs);
            // in current state of parser,
            // right cmds are always leaf nodes
            jobs.emplace_back(std::static_pointer_cast<Parser::BasicCmdNode>(tmp->rightCmd));
        }
    }

    std::string FlowController::_getCommandString(const Parser::CmdNodePtr& command) {
        switch (command->nodeType()) {
            case Parser::BASIC_CMD: {
                auto basicCmd = std::static_pointer_cast<Parser::BasicCmdNode>(command);
                std::vector<std::string> args = _parseArguments(basicCmd->arguments);
                if (args.size() == 0) { return ""; }
                std::stringstream builder;
                builder << args[0];
                for (int i = 1; i < args.size(); i++) { builder << " " << args[i]; }
                return builder.str();
            }
            case Parser::PIPE_CMD: {
                auto pipeCmd = std::static_pointer_cast<Parser::PipeCmdNode>(command);
                return _getCommandString(pipeCmd->leftCmd) + " | " + _getCommandString(pipeCmd->rightCmd);
            }
            default: {
                auto asyncCmd = std::static_pointer_cast<Parser::AsyncCmdNode>(command);
                return _getCommandString(asyncCmd->cmd) + " &";
            }
        }
    }

    CommanderTypePtr FlowController::_copy(const CommanderTypePtr& commanderTypePtr) {
        switch (commanderTypePtr->getType()) {
            case TypeChecker::INT:
                return std::make_shared<CommanderInt>(std::static_pointer_cast<CommanderInt>(commanderTypePtr)->value);
            case TypeChecker::FLOAT:
                return std::make_shared<CommanderFloat>(
                        std::static_pointer_cast<CommanderFloat>(commanderTypePtr)->value);
            case TypeChecker::BOOL:
                return std::make_shared<CommanderBool>(
                        std::static_pointer_cast<CommanderBool>(commanderTypePtr)->value);
            default:
                return commanderTypePtr;
        }
    }
}  // namespace FlowController
