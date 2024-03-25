/**
 * @file type_checker.cpp
 * @brief type_checker.cpp contains the implementation for the prototype methods in type_checker.hpp
 * @details For method prototypes, see type_checker.hpp
 */

#include "type_checker.hpp"

namespace TypeChecker {

    TypeChecker::TypeChecker() {
        for (const auto& pair : Function::functionTypes) {
            _table.addVariable(pair.first, std::make_shared<FunctionInfo>(
                                                   std::vector<TyPtr> {pair.second.begin(), pair.second.end()}));
        }
    }

    void TypeChecker::typeCheck(const Parser::ASTNodeList& astNodeList) {
        for (const Parser::ASTNodePtr& node : astNodeList) { typeCheck(node); }
    }

    TyPtr TypeChecker::typeCheck(const Parser::ASTNodePtr& astNode) {
        if (!astNode) { return nullptr; }
        switch (astNode->nodeType()) {
            case Parser::BINDING: {
                Parser::BindingNodePtr const binding = std::static_pointer_cast<Parser::BindingNode>(astNode);
                assertNotExists(binding->variable);
                TyPtr bindingType = typeCheck(binding->type);
                _table.addVariable(binding->variable, std::make_shared<VariableInfo>(binding->constant, bindingType));
                return bindingType;
            }
            case Parser::BINDINGS: {
                Parser::BindingsNodePtr const bindings = std::static_pointer_cast<Parser::BindingsNode>(astNode);
                for (const Parser::BindingNodePtr& currentBinding : bindings->bindings) { typeCheck(currentBinding); }
                return nullptr;
            }
            case Parser::BASIC_CMD: {
                Parser::BasicCmdNodePtr const command = std::static_pointer_cast<Parser::BasicCmdNode>(astNode);
                for (const Parser::ASTNodePtr& node : command->arguments) {
                    TyPtr const nodeType = typeCheck(node);
                    if (!nodeType || nodeType->getType() != Type::STRING) {
                        // TODO: Improve error
                        throw Util::CommanderException("Command has an argument that is not a string.");
                    }
                }
                std::vector<TyPtr> contentTypes;
                contentTypes.push_back(STRING_TY);
                contentTypes.push_back(STRING_TY);
                contentTypes.push_back(INT_TY);
                return std::make_shared<TupleTy>(contentTypes);
            }
            case Parser::PIPE_CMD: {
                Parser::PipeCmdNodePtr const pipeCommand = std::static_pointer_cast<Parser::PipeCmdNode>(astNode);
                typeCheck(pipeCommand->leftCmd);
                return typeCheck(pipeCommand->rightCmd);
            }
            case Parser::ASYNC_CMD: {
                Parser::AsyncCmdNodePtr const asyncCommand = std::static_pointer_cast<Parser::AsyncCmdNode>(astNode);
                return typeCheck(asyncCommand->cmd);
            }
            case Parser::VAR_LVALUE: {
                Parser::VarLValueNodePtr const lvalue = std::static_pointer_cast<Parser::VarLValueNode>(astNode);
                if (lvalue->type) { return lvalue->type; }
                return (lvalue->type = getVarType(lvalue->variable));
            }
            case Parser::INDEX_LVALUE: {
                Parser::IndexLValueNodePtr const lvalue = std::static_pointer_cast<Parser::IndexLValueNode>(astNode);
                if (lvalue->type) { return lvalue->type; }
                return (lvalue->type = typeCheck(std::make_shared<Parser::IndexExprNode>(lvalue)));
            }
            case Parser::INT_EXPR: {
                Parser::IntExprNodePtr const exprNode = std::static_pointer_cast<Parser::IntExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                return (exprNode->type = INT_TY);
            }
            case Parser::FLOAT_EXPR: {
                Parser::FloatExprNodePtr const exprNode = std::static_pointer_cast<Parser::FloatExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                return (exprNode->type = FLOAT_TY);
            }
            case Parser::STRING_EXPR: {
                if (astNode == nullptr) return nullptr;
                Parser::StringExprNodePtr const exprNode = std::static_pointer_cast<Parser::StringExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                typeCheck(exprNode->stringNode);
                return (exprNode->type = STRING_TY);
            }
            case Parser::BOOL_EXPR: {
                Parser::BoolExprNodePtr const exprNode = std::static_pointer_cast<Parser::BoolExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                return (exprNode->type = BOOL_TY);
            }
            case Parser::VAR_EXPR: {
                Parser::VarExprNodePtr const exprNode = std::static_pointer_cast<Parser::VarExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                TyPtr const varType = getVarType(exprNode->variable);
                if (!varType) {
                    // TODO: Improve error
                    throw Util::CommanderException("No variable by the name '" + exprNode->variable
                                                   + "' has not been initialized yet (in the current scope).");
                }
                return (exprNode->type = varType);
            }
            case Parser::LVALUE_EXPR: {
                Parser::LValueExprNodePtr const exprNode = std::static_pointer_cast<Parser::LValueExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                return (exprNode->type = typeCheck(exprNode->expr));
            }
            case Parser::ARRAY_EXPR: {
                Parser::ArrayExprNodePtr const exprNode = std::static_pointer_cast<Parser::ArrayExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                TyPtr const type = exprNode->expressions->exprs.empty() ? nullptr
                                                                        : typeCheck(exprNode->expressions->exprs[0]);
                if (!exprNode->expressions->exprs.empty()) {
                    if (!type) {
                        // TODO: Improve error
                        throw Util::CommanderException("Array has an unknown type.");
                    }
                    for (const Parser::ExprNodePtr& exprNodePtr : exprNode->expressions->exprs) {
                        if (!areTypesEqual(type, typeCheck(exprNodePtr))) {
                            // TODO: Improve error
                            throw Util::CommanderException("Array contains multiple different types.");
                        }
                    }
                }
                return (exprNode->type = std::make_shared<ArrayTy>(type));
            }
            case Parser::INDEX_EXPR: {
                Parser::IndexExprNodePtr const exprNode = std::static_pointer_cast<Parser::IndexExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                TyPtr const indexType = typeCheck(exprNode->index);
                if (!indexType || indexType->getType() != Type::INT) {
                    // TODO: Improve error (not just position, but what type; same for the other errors)
                    throw Util::CommanderException(
                            "Tried to index an array or tuple with a different type than an int");
                }
                TyPtr const exprType = typeCheck(exprNode->expr);
                if (!exprType) { return (exprNode->type = nullptr); }
                switch (exprType->getType()) {
                    case ARRAY: {
                        ArrayTyPtr arrayTy = std::static_pointer_cast<ArrayTy>(exprType);
                        if (!arrayTy->baseType) {
                            // TODO: Improve error
                            throw Util::CommanderException("Unable to index empty array");
                        }
                        return (exprNode->type = arrayTy->baseType);
                    }
                    case TUPLE: {
                        TupleTyPtr tupleTy = std::static_pointer_cast<TupleTy>(exprType);
                        if (tupleTy->contentTypes.size() == 0) {
                            // TODO: Improve error
                            throw Util::CommanderException("Unable to index empty tuple");
                        }
                        if (exprNode->index->nodeType() == Parser::INT_EXPR) {
                            int64_t index = std::static_pointer_cast<Parser::IntExprNode>(exprNode->index)->value;
                            if (tupleTy->contentTypes.size() <= index) {
                                // TODO: Improve error
                                throw Util::CommanderException("Index out of bounds on tuple");
                            }
                            return (exprNode->type = tupleTy->contentTypes[index]);
                        }
                        return (exprNode->type = nullptr);
                    }
                    default:
                        // TODO: Improve error
                        throw Util::CommanderException("Tried to index a type that isn't an array or tuple");
                }
            }
            case Parser::TUPLE_EXPR: {
                Parser::TupleExprNodePtr const exprNode = std::static_pointer_cast<Parser::TupleExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                std::vector<TyPtr> expressionTypes;
                for (const Parser::ExprNodePtr& exprNodePtr : exprNode->expressions->exprs) {
                    TyPtr const exprType = typeCheck(exprNodePtr);
                    if (!exprType) {
                        // TODO: Improve error
                        throw Util::CommanderException("Unknown type for expression in tuple");
                    }
                    expressionTypes.push_back(exprType);
                }
                return (exprNode->type = std::make_shared<TupleTy>(expressionTypes));
            }
            case Parser::TERNARY_EXPR: {
                Parser::TernaryExprNodePtr const exprNode = std::static_pointer_cast<Parser::TernaryExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                TyPtr const conditionType = typeCheck(exprNode->condition);
                if (!conditionType || conditionType->getType() != Type::BOOL) {
                    // TODO: Improve error
                    throw Util::CommanderException("Ternary condition does not evaluate to a boolean");
                }
                TyPtr const trueType = typeCheck(exprNode->trueExpr);
                TyPtr const falseType = typeCheck(exprNode->falseExpr);
                if (!trueType || !falseType || !areTypesEqual(trueType, falseType)) {
                    // TODO: Improve error
                    throw Util::CommanderException("True and false expressions in ternary do not match the same type.");
                }
                return (exprNode->type = trueType);
            }
            case Parser::UNOP_EXPR: {
                Parser::UnOpExprNodePtr const exprNode = std::static_pointer_cast<Parser::UnOpExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                TyPtr const expressionType = typeCheck(exprNode->node);
                bool const isTuple
                        = exprNode->node->nodeType() == Parser::INDEX_LVALUE
                       && std::static_pointer_cast<Parser::IndexLValueNode>(exprNode->node)->lvalue->type
                       && std::static_pointer_cast<Parser::IndexLValueNode>(exprNode->node)->lvalue->type->getType()
                                  == TUPLE;
                switch (exprNode->opType) {
                    case Parser::NOT:
                        if (expressionType->getType() != Type::BOOL) {
                            // TODO: Improve error
                            throw Util::CommanderException("Expected a bool in not operation.");
                        }
                        break;
                    case Parser::NEGATE:
                    case Parser::PRE_INCREMENT:
                    case Parser::POST_INCREMENT:
                    case Parser::PRE_DECREMENT:
                    case Parser::POST_DECREMENT:
                        if (!isTuple && expressionType->getType() != Type::INT
                            && expressionType->getType() != Type::FLOAT) {
                            // TODO: Improve error
                            throw Util::CommanderException("Expected int or float in unop expression.");
                        }
                        if (exprNode->opType != Parser::NEGATE) {
                            if (exprNode->node->nodeType() != Parser::INDEX_LVALUE
                                && exprNode->node->nodeType() != Parser::VAR_LVALUE) {
                                // TODO: Improve error
                                throw Util::CommanderException(
                                        "Expected variable or index lvalue in decrement or increment operation.");
                            }
                            if (exprNode->node->nodeType() == Parser::VAR_LVALUE) {
                                VarInfoPtr const varInfo = _table.getVariable(
                                        std::static_pointer_cast<Parser::VarLValueNode>(exprNode->node)->variable);
                                if (varInfo->constant) {
                                    // TODO: Improve error
                                    throw Util::CommanderException(
                                            "Unable to increment or decrement constant variable.");
                                }
                            }
                        }
                }
                return (exprNode->type = expressionType);
            }
            case Parser::BINOP_EXPR: {
                Parser::BinOpExprNodePtr const exprNode = std::static_pointer_cast<Parser::BinOpExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                bool const isLvalue = exprNode->left->nodeType() == Parser::VAR_LVALUE
                                   || exprNode->left->nodeType() == Parser::INDEX_LVALUE;
                bool const isBinding = exprNode->left->nodeType() == Parser::BINDING;
                bool const isFirst
                        = isBinding
                               && !_table.hasVariable(
                                       std::static_pointer_cast<Parser::BindingNode>(exprNode->left)->variable)
                       || exprNode->left->nodeType() == Parser::VAR_LVALUE
                                  && !_table.hasVariable(
                                          std::static_pointer_cast<Parser::VarLValueNode>(exprNode->left)->variable);
                TyPtr const leftTy
                        = !isBinding || isFirst
                                ? typeCheck(exprNode->left)
                                : getVarType(std::static_pointer_cast<Parser::BindingNode>(exprNode->left)->variable);
                TyPtr const rightTy = typeCheck(exprNode->right);
                // Right type must be known in all operations
                if (!rightTy) {
                    // TODO: Improve error
                    throw Util::CommanderException("Right expression evaluates to unknown type in binary operation");
                }
                Type const rightType = rightTy->getType();
                bool const isInt = rightType == Type::INT;
                bool const isFloat = rightType == Type::FLOAT;
                bool const isBool = rightType == Type::BOOL;
                bool const isString = rightType == Type::STRING;
                bool areEqual = areTypesEqual(leftTy, rightTy);
                bool areIntFloat = leftTy
                                && ((leftTy->getType() == Type::INT && isFloat)
                                    || (leftTy->getType() == Type::FLOAT && isInt));
                switch (exprNode->opType) {
                    case Parser::SET: {
                        if (!isLvalue && !isBinding) {
                            // TODO: Improve error
                            throw Util::CommanderException(
                                    "Invalid set expression; must have variable or index lvalue on left side of '='");
                        }
                        if (isBinding && std::static_pointer_cast<Parser::BindingNode>(exprNode->left)->type
                            && !areTypesEqual(
                                    typeCheck(std::static_pointer_cast<Parser::BindingNode>(exprNode->left)->type),
                                    rightTy)) {
                            // TODO: Improve error
                            throw Util::CommanderException(
                                    "Expected one type, but got another, while setting a variable");
                        }
                        if (isBinding || exprNode->left->nodeType() == Parser::VAR_LVALUE) {
                            std::string varName
                                    = isBinding
                                            ? std::static_pointer_cast<Parser::BindingNode>(exprNode->left)->variable
                                            : std::static_pointer_cast<Parser::VarLValueNode>(exprNode->left)->variable;
                            bool isConstant = !isFirst && _table.getVariable(varName)->constant;
                            if (isConstant) {
                                // TODO: Improve error
                                throw Util::CommanderException("Unable to update constant variable.");
                            }
                            _table.addVariable(
                                    varName,
                                    std::make_shared<VariableInfo>(
                                            isBinding
                                                    && std::static_pointer_cast<Parser::BindingNode>(exprNode->left)
                                                               ->constant,
                                            rightTy));
                        } else if (leftTy) {
                            Parser::IndexLValueNodePtr lvalue = std::static_pointer_cast<Parser::IndexLValueNode>(
                                    exprNode->left);
                            if (!areEqual && lvalue->lvalue->type->getType() == ARRAY) {
                                // TODO: Improve error
                                throw Util::CommanderException("Array index cannot be set due to incompatible types.");
                            }
                            if (lvalue->lvalue->type->getType() == TUPLE) {
                                int64_t index = std::static_pointer_cast<Parser::IntExprNode>(lvalue->index)->value;
                                TupleTyPtr tupleTy = std::static_pointer_cast<TupleTy>(lvalue->lvalue->type);
                                tupleTy->contentTypes[index] = rightTy;
                            }
                        }
                        return (exprNode->type = rightTy);
                    }
                    case Parser::LESSER:
                    case Parser::GREATER:
                    case Parser::LESSER_EQUAL:
                    case Parser::GREATER_EQUAL:
                        // Int/float | float/float | Int/int | string/string --> bool
                        if (areIntFloat || (areEqual && (isInt || isFloat || isString))) {
                            return (exprNode->type = BOOL_TY);
                        }
                        // TODO: Improve error
                        throw Util::CommanderException("Inequality operation has incompatible types.");
                    case Parser::AND:
                    case Parser::OR:
                        // bool/bool --> bool
                        if (areEqual && isBool) { return (exprNode->type = BOOL_TY); }
                        // TODO: Improve error
                        throw Util::CommanderException("Boolean operation has incompatible types.");
                    case Parser::EQUAL:
                    case Parser::NOT_EQUAL:
                        // any/any --> bool
                        if (areIntFloat || areEqual) { return (exprNode->type = BOOL_TY); }
                        // TODO: Improve error
                        throw Util::CommanderException("Equality operation has incompatible types.");
                    case Parser::ADD_SET:
                    case Parser::SUBTRACT_SET:
                    case Parser::MULTIPLY_SET:
                    case Parser::DIVIDE_SET:
                    case Parser::MODULO_SET:
                    case Parser::EXPONENTIATE_SET:
                        if (!isLvalue) {
                            // TODO: Improve error
                            throw Util::CommanderException(
                                    "Invalid set expression; must have variable or index lvalue on left side of '='");
                        }
                        if (exprNode->left->nodeType() == Parser::VAR_LVALUE) {
                            if (!leftTy) {
                                // TODO: Improve error
                                throw Util::CommanderException("Variable is not initialized");
                            }
                            if (_table.getVariable(
                                              std::static_pointer_cast<Parser::VarLValueNode>(exprNode->left)->variable)
                                        ->constant) {
                                // TODO: Improve error
                                throw Util::CommanderException("Unable to update constant variable.");
                            }
                        } else if (!leftTy) {
                            // This must be a non-empty tuple being indexed, so since we don't know the left ty assume
                            // it matches the right ty
                            areIntFloat = isInt || isFloat;
                            areEqual = true;
                        }
                        // NO break here! Should continue to the next lines of code that handle add, subtract, etc.
                    case Parser::ADD:
                    case Parser::MODULO:
                    case Parser::DIVIDE:
                    case Parser::MULTIPLY:
                    case Parser::SUBTRACT:
                    case Parser::EXPONENTIATE:
                        // Int/float | float/float --> float
                        // Int/int --> int
                        // string/string --> string (in the case of ADD and ADD_EQUAL)
                        if (areIntFloat || (areEqual && isFloat)) { return (exprNode->type = FLOAT_TY); }
                        if (areEqual && isInt) { return (exprNode->type = INT_TY); }
                        if (areEqual && isString
                            && (exprNode->opType == Parser::ADD || exprNode->opType == Parser::ADD_SET)) {
                            return (exprNode->type = STRING_TY);
                        }
                        // TODO: Improve error
                        throw Util::CommanderException("Binary operation has incompatible types.");
                }
                // All cases are dealt with in switch statement, so it shouldn't ever get here
                throw Util::CommanderException("Unexpected error occurred with parsing binary expression");
            }
            case Parser::CALL_EXPR: {
                Parser::CallExprNodePtr const exprNode = std::static_pointer_cast<Parser::CallExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                TyPtr type = typeCheck(exprNode->func);
                if (!type || type->getType() != Type::FUNCTION) {
                    // TODO: Improve Error
                    throw Util::CommanderException("Tried to call something that wasn't a function.");
                }
                std::vector<TyPtr> functionTypes;
                if (exprNode->func->nodeType() == Parser::VAR_EXPR
                    || (exprNode->func->nodeType() == Parser::LVALUE_EXPR
                        && std::static_pointer_cast<Parser::LValueExprNode>(exprNode->func)->expr->nodeType()
                                   == Parser::VAR_EXPR)) {
                    std::string varName
                            = exprNode->func->nodeType() == Parser::VAR_EXPR
                                    ? std::static_pointer_cast<Parser::VarExprNode>(exprNode->func)->variable
                                    : std::static_pointer_cast<Parser::VarExprNode>(
                                              std::static_pointer_cast<Parser::LValueExprNode>(exprNode->func)->expr)
                                              ->variable;
                    VarInfoPtr const functionInfo = _table.getVariable(varName);
                    if (functionInfo->infoType() != FUNCTION_INFO) {
                        // TODO: Improve error
                        throw Util::CommanderException("Variable of name " + varName
                                                       + " does not refer to a function.");
                    }
                    functionTypes = functionInfo->types;
                } else {
                    functionTypes.push_back(type);
                }
                std::vector<TyPtr> argTypes;
                for (const Parser::ExprNodePtr& expr : exprNode->args->exprs) { argTypes.push_back(typeCheck(expr)); }
                FunctionTyPtr functionType = nullptr;
                for (const TyPtr& typ : functionTypes) {
                    FunctionTyPtr const functionTy = std::static_pointer_cast<FunctionTy>(typ);
                    size_t const size = functionTy->parameters.size();
                    if (size != argTypes.size()) { continue; }
                    for (int i = 0; i < size; i++) {
                        if (!areTypesEqual(functionTy->parameters[i], argTypes[i])) { continue; }
                    }
                    functionType = functionTy;
                    break;
                }
                if (!functionType) {
                    // TODO: Improve Error
                    throw Util::CommanderException("No function exists that matches this function signature.");
                }
                return (exprNode->type = functionType->returnType);
            }
            case Parser::API_CALL_EXPR: {
                Parser::ApiCallExprNodePtr const exprNode = std::static_pointer_cast<Parser::ApiCallExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                exprNode->args->exprs.insert(exprNode->args->exprs.begin(), exprNode->expression);
                // TODO: Everything after this is pretty much the same as call expr, so perhaps put this code into it's
                // own method
                TyPtr type = getVarType(exprNode->func);
                if (!type || type->getType() != Type::FUNCTION) {
                    // TODO: Improve Error
                    throw Util::CommanderException("Tried to call something that wasn't a function.");
                }
                std::vector<TyPtr> functionTypes;
                VarInfoPtr const functionInfo = _table.getVariable(exprNode->func);
                if (functionInfo->infoType() != FUNCTION_INFO) {
                    // TODO: Improve error
                    throw Util::CommanderException("Variable of name " + exprNode->func
                                                   + " does not refer to a function.");
                }
                functionTypes = functionInfo->types;
                std::vector<TyPtr> argTypes;
                for (const Parser::ExprNodePtr& expr : exprNode->args->exprs) { argTypes.push_back(typeCheck(expr)); }
                FunctionTyPtr functionType = nullptr;
                for (const TyPtr& typ : functionTypes) {
                    FunctionTyPtr const functionTy = std::static_pointer_cast<FunctionTy>(typ);
                    size_t const size = functionTy->parameters.size();
                    if (size != argTypes.size()) { continue; }
                    for (int i = 0; i < size; i++) {
                        if (!areTypesEqual(functionTy->parameters[i], argTypes[i])) { continue; }
                    }
                    functionType = functionTy;
                    break;
                }
                if (!functionType) {
                    // TODO: Improve Error
                    throw Util::CommanderException("No function exists that matches this function signature.");
                }
                if (exprNode->func == "append") {
                    ArrayTyPtr arrayTy = std::static_pointer_cast<ArrayTy>(argTypes[0]);
                    if (!arrayTy->baseType) { arrayTy->baseType = argTypes[1]; }
                }
                return (exprNode->type = functionType->returnType);
            }
            case Parser::LAMBDA_EXPR: {
                Parser::LambdaExprNodePtr const exprNode = std::static_pointer_cast<Parser::LambdaExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                pushScope();
                std::vector<TyPtr> bindings;
                for (const Parser::BindingNodePtr& binding : exprNode->bindings->bindings) {
                    bindings.push_back(typeCheck(binding));
                }
                TyPtr returnType = typeCheck(exprNode->returnType);
                if (!returnType) {
                    returnType = typeCheck(exprNode->body);
                    if (!returnType) { returnType = VOID_TY; }
                } else if (!areTypesEqual(returnType, typeCheck(exprNode->body))) {
                    // TODO: Improve error
                    throw Util::CommanderException("Return types for the lambda function don't match.");
                }
                popScope();
                return (exprNode->type = std::make_shared<FunctionTy>(bindings, returnType));
            }
            case Parser::CMD_EXPR: {
                Parser::CmdExprNodePtr const exprNode = std::static_pointer_cast<Parser::CmdExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                typeCheck(exprNode->cmd);
                std::vector<TyPtr> contentTypes;
                contentTypes.push_back(STRING_TY);
                contentTypes.push_back(STRING_TY);
                contentTypes.push_back(INT_TY);
                return (exprNode->type = std::make_shared<TupleTy>(contentTypes));
            }
            case Parser::SCAN_EXPR: {
                Parser::ScanExprNodePtr const exprNode = std::static_pointer_cast<Parser::ScanExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                TyPtr const promptType = typeCheck(exprNode->prompt);
                if (!promptType || promptType->getType() != Type::STRING) {
                    // TODO: Improve error
                    throw Util::CommanderException(
                            "Expected a string prompt for scan expression, but got something else.");
                }
                return (exprNode->type = STRING_TY);
            }
            case Parser::READ_EXPR: {
                Parser::ReadExprNodePtr const exprNode = std::static_pointer_cast<Parser::ReadExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                TyPtr const pathType = typeCheck(exprNode->filePath);
                if (!pathType || pathType->getType() != Type::STRING) {
                    // TODO: Improve error
                    throw Util::CommanderException(
                            "Expected a string file path for read expression, but got something else.");
                }
                return (exprNode->type = STRING_TY);
            }
            case Parser::EXPRS: {
                Parser::ExprsNodePtr const exprsPtr = std::static_pointer_cast<Parser::ExprsNode>(astNode);
                for (const Parser::ExprNodePtr& exprPtr : exprsPtr->exprs) { typeCheck(exprPtr); }
                return nullptr;
            }
            case Parser::PRGM: {
                Parser::PrgmNodePtr const program = std::static_pointer_cast<Parser::PrgmNode>(astNode);
                return typeCheck(program->stmts);
            }
            case Parser::IF_STMT: {
                Parser::IfStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::IfStmtNode>(astNode);
                TyPtr const conditionType = typeCheck(stmtNode->condition);
                if (!conditionType || conditionType->getType() != Type::BOOL) {
                    // TODO: Improve error
                    throw Util::CommanderException("The condition does not evaluate to a bool type.");
                }
                typeCheck(stmtNode->trueStmt);
                if (stmtNode->falseStmt) { typeCheck(stmtNode->falseStmt); }
                return nullptr;
            }
            case Parser::FOR_STMT: {
                Parser::ForStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::ForStmtNode>(astNode);
                pushScope();
                typeCheck(stmtNode->initial);
                TyPtr const conditionType = typeCheck(stmtNode->condition);
                if (!conditionType || conditionType->getType() != Type::BOOL) {
                    // TODO: Improve error
                    throw Util::CommanderException("The condition does not evaluate to a bool type.");
                }
                typeCheck(stmtNode->update);
                typeCheck(stmtNode->body);
                popScope();
                return nullptr;
            }
            case Parser::WHILE_STMT: {
                Parser::WhileStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::WhileStmtNode>(astNode);
                TyPtr const conditionType = typeCheck(stmtNode->condition);
                if (!conditionType || conditionType->getType() != Type::BOOL) {
                    // TODO: Improve error
                    throw Util::CommanderException("The condition does not evaluate to a bool type.");
                }
                typeCheck(stmtNode->body);
                return nullptr;
            }
            case Parser::DO_WHILE_STMT: {
                Parser::DoWhileStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::DoWhileStmtNode>(astNode);
                typeCheck(stmtNode->body);
                TyPtr const conditionType = typeCheck(stmtNode->condition);
                if (!conditionType || conditionType->getType() != Type::BOOL) {
                    // TODO: Improve error
                    throw Util::CommanderException("The condition does not evaluate to a bool type.");
                }
                return nullptr;
            }
            case Parser::RETURN_STMT: {
                Parser::ReturnStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::ReturnStmtNode>(astNode);
                return typeCheck(stmtNode->retExpr);
            }
            case Parser::BREAK_STMT:
            case Parser::CONTINUE_STMT:
                return nullptr;
            case Parser::SCOPE_STMT: {
                Parser::ScopeStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::ScopeStmtNode>(astNode);
                pushScope();
                TyPtr returnTy = typeCheck(stmtNode->stmts);
                popScope();
                return returnTy;
            }
            case Parser::CMD_STMT: {
                Parser::CmdStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::CmdStmtNode>(astNode);
                return typeCheck(stmtNode->command);
            }
            case Parser::EXPR_STMT: {
                Parser::ExprStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::ExprStmtNode>(astNode);
                return typeCheck(stmtNode->expression);
            }
            case Parser::ALIAS_STMT: {
                Parser::AliasStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::AliasStmtNode>(astNode);
                assertNotExists(stmtNode->alias);
                _table.addVariable(stmtNode->alias, std::make_shared<AliasInfo>());
                return nullptr;
            }
            case Parser::IMPORT_STMT: {
                Parser::ImportStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::ImportStmtNode>(astNode);
                TyPtr const pathType = typeCheck(stmtNode->filePath);
                if (!pathType || pathType->getType() != Type::STRING) {
                    // TODO: Improve error
                    throw Util::CommanderException(
                            "Expected a string file path for import statement, but got something else.");
                }
                Parser::StringNodePtr currentNode = stmtNode->filePath;
                while (!currentNode->isLiteral()) {
                    if (currentNode->expressions->expressions.size() != 1) {
                        // TODO: Improve error
                        throw Util::CommanderException(
                                "Expected string literal for import statement, but got an interpolated string");
                    }
                    currentNode = std::static_pointer_cast<Parser::StringExprNode>(
                                          currentNode->expressions->expressions[0])
                                          ->stringNode;
                }
                std::string filePath = currentNode->literal;
                Lexer::TokenList tokens;
                Lexer::tokenize(tokens, filePath);
                Parser::Parser parser;
                stmtNode->prgm = parser.parse(tokens);
                typeCheck(stmtNode->prgm);
                return nullptr;
            }
            case Parser::PRINT_STMT: {
                Parser::PrintStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::PrintStmtNode>(astNode);
                typeCheck(stmtNode->expression);
                return nullptr;
            }
            case Parser::PRINTLN_STMT: {
                Parser::PrintlnStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::PrintlnStmtNode>(astNode);
                typeCheck(stmtNode->expression);
                return nullptr;
            }
            case Parser::WRITE_STMT: {
                Parser::WriteStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::WriteStmtNode>(astNode);
                TyPtr const dataType = typeCheck(stmtNode->fileData);
                if (!dataType || dataType->getType() != Type::STRING) {
                    // TODO: Improve error
                    throw Util::CommanderException(
                            "Expected a string data for write statement, but got something else.");
                }
                TyPtr const pathType = typeCheck(stmtNode->filePath);
                if (!pathType || pathType->getType() != Type::STRING) {
                    // TODO: Improve error
                    throw Util::CommanderException(
                            "Expected a string file path for write statement, but got something else.");
                }
                return nullptr;
            }
            case Parser::TYPE_STMT: {
                Parser::TypeStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::TypeStmtNode>(astNode);
                assertNotExists(stmtNode->alias);
                _table.addVariable(stmtNode->alias, std::make_shared<TypeInfo>(typeCheck(stmtNode->type)));
                return nullptr;
            }
            case Parser::FUNCTION_STMT: {
                Parser::FunctionStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::FunctionStmtNode>(
                        astNode);
                assertNotExists(stmtNode->name);
                pushScope();
                std::vector<TyPtr> bindings;
                for (const Parser::BindingNodePtr& binding : stmtNode->bindings->bindings) {
                    bindings.push_back(typeCheck(binding));
                }
                TyPtr returnType = typeCheck(stmtNode->returnType);
                if (!returnType) {
                    returnType = typeCheck(stmtNode->body);
                    if (!returnType) { returnType = VOID_TY; }
                } else if (!areTypesEqual(returnType, typeCheck(stmtNode->body))) {
                    // TODO: Improve error
                    throw Util::CommanderException("Return types for the function " + stmtNode->name + " don't match.");
                }
                popScope();
                _table.addVariable(stmtNode->name, std::make_shared<FunctionInfo>(std::vector<TyPtr> {
                                                           std::make_shared<FunctionTy>(bindings, returnType)}));
                return nullptr;
            }
            case Parser::TIMEOUT_STMT: {
                Parser::TimeoutStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::TimeoutStmtNode>(astNode);
                typeCheck(stmtNode->message);
                return typeCheck(stmtNode->stmt);
            }
            case Parser::ASSERT_STMT: {
                Parser::AssertStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::AssertStmtNode>(astNode);
                typeCheck(stmtNode->message);
                TyPtr type = typeCheck(stmtNode->expr);
                if (!type || type->getType() != BOOL) {
                    // TODO: Improve error
                    throw Util::CommanderException(
                            "Assert statement requires boolean expression, but got different type.");
                }
                return nullptr;
            }
            case Parser::STMTS: {
                Parser::StmtsNodePtr const stmtsPtr = std::static_pointer_cast<Parser::StmtsNode>(astNode);
                for (const Parser::StmtNodePtr& stmtPtr : stmtsPtr->stmts) { typeCheck(stmtPtr); }
                TyPtr returnType = VOID_TY;
                Parser::StmtNodePtr const lastStmt = stmtsPtr->stmts.back();
                if (lastStmt->nodeType() == Parser::RETURN_STMT) { returnType = typeCheck(lastStmt); }
                return returnType;
            }
            case Parser::STRING: {
                Parser::StringNodePtr const stringPtr = std::static_pointer_cast<Parser::StringNode>(astNode);
                if (!stringPtr->isLiteral()) { typeCheck(stringPtr->expressions); }
                return STRING_TY;
            }
            case Parser::STRING_EXPRS: {
                Parser::StringExprsNodePtr const stringsPtr = std::static_pointer_cast<Parser::StringExprsNode>(
                        astNode);
                for (const Parser::ExprNodePtr& exprPtr : stringsPtr->expressions) { typeCheck(exprPtr); }
                return nullptr;
            }
            case Parser::INT_TYPE: {
                Parser::IntTypeNodePtr const intPtr = std::static_pointer_cast<Parser::IntTypeNode>(astNode);
                if (intPtr->type) { return intPtr->type; }
                return (intPtr->type = INT_TY);
            }
            case Parser::FLOAT_TYPE: {
                Parser::FloatTypeNodePtr const floatPtr = std::static_pointer_cast<Parser::FloatTypeNode>(astNode);
                if (floatPtr->type) { return floatPtr->type; }
                return (floatPtr->type = FLOAT_TY);
            }
            case Parser::BOOL_TYPE: {
                Parser::BoolTypeNodePtr const boolPtr = std::static_pointer_cast<Parser::BoolTypeNode>(astNode);
                if (boolPtr->type) { return boolPtr->type; }
                return (boolPtr->type = BOOL_TY);
            }
            case Parser::STRING_TYPE: {
                Parser::StringTypeNodePtr const stringTypePtr = std::static_pointer_cast<Parser::StringTypeNode>(
                        astNode);
                if (stringTypePtr->type) { return stringTypePtr->type; }
                return (stringTypePtr->type = STRING_TY);
            }
            case Parser::ARRAY_TYPE: {
                Parser::ArrayTypeNodePtr const arrayTypePtr = std::static_pointer_cast<Parser::ArrayTypeNode>(astNode);
                if (arrayTypePtr->type) { return arrayTypePtr->type; }
                return (arrayTypePtr->type = std::make_shared<ArrayTy>(typeCheck(arrayTypePtr->subtype)));
            }
            case Parser::TUPLE_TYPE: {
                Parser::TupleTypeNodePtr const tupleTypePtr = std::static_pointer_cast<Parser::TupleTypeNode>(astNode);
                if (tupleTypePtr->type) { return tupleTypePtr->type; }
                std::vector<TyPtr> types;
                for (const Parser::TypeNodePtr& typeNodePtr : tupleTypePtr->subtypes->types) {
                    types.push_back(typeCheck(typeNodePtr));
                }
                return (tupleTypePtr->type = std::make_shared<TupleTy>(types));
            }
            case Parser::FUNCTION_TYPE: {
                Parser::FunctionTypeNodePtr const functionTypePtr = std::static_pointer_cast<Parser::FunctionTypeNode>(
                        astNode);
                if (functionTypePtr->type) { return functionTypePtr->type; }
                std::vector<TyPtr> types;
                for (const Parser::TypeNodePtr& typeNodePtr : functionTypePtr->params->types) {
                    types.push_back(typeCheck(typeNodePtr));
                }
                return (functionTypePtr->type = std::make_shared<FunctionTy>(types,
                                                                             typeCheck(functionTypePtr->returnType)));
            }
            case Parser::VARIABLE_TYPE: {
                Parser::VariableTypeNodePtr const variableTypePtr = std::static_pointer_cast<Parser::VariableTypeNode>(
                        astNode);
                if (variableTypePtr->type) { return variableTypePtr->type; }
                VarInfoPtr typeInfo = _table.getVariable(variableTypePtr->name);
                if (typeInfo->infoType() != TYPE_INFO) {
                    // TODO: Improve error
                    throw Util::CommanderException("Variable of name " + variableTypePtr->name
                                                   + " does not refer to a type.");
                }
                return (variableTypePtr->type = typeInfo->types[0]);
            }
            case Parser::TYPES: {
                Parser::TypesNodePtr const typesPtr = std::static_pointer_cast<Parser::TypesNode>(astNode);
                for (const Parser::TypeNodePtr& typePtr : typesPtr->types) { typeCheck(typePtr); }
                return nullptr;
            }
            default:
                throw Util::CommanderException(
                        "This should not have happened. Abstract ExprNode, TypeNode, or StmtNode was created...");
        }
    }

    void TypeChecker::assertNotExists(const std::string& variable) {
        if (_table.hasVariable(variable)) {
            // TODO: Improve error
            throw Util::CommanderException("Variable of name " + variable + " already exists in current scope.");
        }
    }

    void TypeChecker::pushScope() { _table = VariableTable(_table); }

    VariableTablePtr TypeChecker::popScope() {
        VariableTablePtr scope = std::make_shared<VariableTable>(_table);
        auto parent = _table.getParent();
        if (parent) { _table = *parent; }
        return scope;
    }

    TyPtr TypeChecker::getVarType(const std::string& varName) {
        VarInfoPtr variableInfo = _table.getVariable(varName);
        // Guaranteed to have at least 1 type in the types list, if variableInfo exists that is
        return variableInfo ? variableInfo->types[0] : nullptr;
    }

}  //  namespace TypeChecker