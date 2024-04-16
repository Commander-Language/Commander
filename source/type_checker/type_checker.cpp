/**
 * @file type_checker.cpp
 * @brief type_checker.cpp contains the implementation for the prototype methods in type_checker.hpp
 * @details For method prototypes, see type_checker.hpp
 */

#include "type_checker.hpp"

namespace TypeChecker {

    TypeChecker::TypeChecker(Parser::Parser& parser) {
        _table = std::make_shared<VariableTable>();
        for (const auto& pair : Function::functionTypes) {
            _table->addVariable(pair.first, std::make_shared<FunctionInfo>(
                                                    std::vector<TyPtr> {pair.second.begin(), pair.second.end()}));
        }
        _parser = std::make_shared<Parser::Parser>(parser);
    }

    VariableTablePtr TypeChecker::typeCheck(const Parser::ASTNodeList& astNodeList) {
        for (const Parser::ASTNodePtr& node : astNodeList) { typeCheck(node); }
        return _table;
    }

    TyPtr TypeChecker::typeCheck(const Parser::ASTNodePtr& astNode) {
        if (!astNode) { return nullptr; }
        switch (astNode->nodeType()) {
            case Parser::BINDING: {
                Parser::BindingNodePtr const binding = std::static_pointer_cast<Parser::BindingNode>(astNode);
                assertVariableNotExists(binding->variable, binding->position);
                TyPtr bindingType = typeCheck(binding->type);
                _table->addVariable(binding->variable, std::make_shared<VariableInfo>(binding->constant, bindingType));
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
                    assertType(typeCheck(node), STRING_TY, node->position);
                }
                return nullptr;
            }
            case Parser::PIPE_CMD: {
                Parser::PipeCmdNodePtr const pipeCommand = std::static_pointer_cast<Parser::PipeCmdNode>(astNode);
                typeCheck(pipeCommand->leftCmd);
                typeCheck(pipeCommand->rightCmd);
                return nullptr;
            }
            case Parser::ASYNC_CMD: {
                Parser::AsyncCmdNodePtr const asyncCommand = std::static_pointer_cast<Parser::AsyncCmdNode>(astNode);
                typeCheck(asyncCommand->cmd);
                return nullptr;
            }
            case Parser::VAR_EXPR:
            case Parser::VAR_LVALUE: {
                const bool isLvalue = astNode->nodeType() == Parser::VAR_LVALUE;
                TyPtr type = isLvalue ? std::static_pointer_cast<Parser::VarLValueNode>(astNode)->type
                                      : std::static_pointer_cast<Parser::VarExprNode>(astNode)->type;
                if (type) { return type; }
                const std::string varName = isLvalue
                                                  ? std::static_pointer_cast<Parser::VarLValueNode>(astNode)->variable
                                                  : std::static_pointer_cast<Parser::VarExprNode>(astNode)->variable;
                assertVariableExists(varName, astNode->position);
                type = getVarType(varName, VARIABLE_INFO, astNode->position);
                if (isLvalue) { return (std::static_pointer_cast<Parser::VarLValueNode>(astNode)->type = type); }
                return (std::static_pointer_cast<Parser::VarExprNode>(astNode)->type = type);
            }
            case Parser::INDEX_EXPR:
            case Parser::INDEX_LVALUE: {
                const bool isLvalue = astNode->nodeType() == Parser::INDEX_LVALUE;
                TyPtr type = isLvalue ? std::static_pointer_cast<Parser::IndexLValueNode>(astNode)->type
                                      : std::static_pointer_cast<Parser::IndexExprNode>(astNode)->type;
                if (type) { return type; }
                const Parser::ExprNodePtr index
                        = isLvalue ? std::static_pointer_cast<Parser::IndexLValueNode>(astNode)->index
                                   : std::static_pointer_cast<Parser::IndexExprNode>(astNode)->index;
                assertType(typeCheck(index), INT_TY, index->position);
                TyPtr const exprType
                        = isLvalue ? typeCheck(std::static_pointer_cast<Parser::IndexLValueNode>(astNode)->lvalue)
                                   : typeCheck(std::static_pointer_cast<Parser::IndexExprNode>(astNode)->expr);
                if (!exprType) { return nullptr; }
                if (exprType->getType() != ARRAY && exprType->getType() != TUPLE) {
                    throw Util::CommanderException("Expected ARRAY or TUPLE type, but got"
                                                           + getErrorTypeString(exprType) + " instead.",
                                                   astNode->position);
                }
                switch (exprType->getType()) {
                    case ARRAY:
                        type = std::static_pointer_cast<ArrayTy>(exprType)->baseType;
                        break;
                    default: {
                        TupleTyPtr const tupleTy = std::static_pointer_cast<TupleTy>(exprType);
                        if (tupleTy->contentTypes.empty()) {
                            throw Util::CommanderException("Unable to index empty tuple", astNode->position);
                        }
                        if (index->nodeType() == Parser::INT_EXPR) {
                            int64_t const idx = std::static_pointer_cast<Parser::IntExprNode>(index)->value;
                            if (tupleTy->contentTypes.size() <= idx) {
                                throw Util::CommanderException("Index out of bounds on tuple. Tuple size is "
                                                                       + std::to_string(tupleTy->contentTypes.size())
                                                                       + " while index is " + std::to_string(idx),
                                                               index->position);
                            }
                            type = tupleTy->contentTypes[idx];
                        }
                        break;
                    }
                }
                if (isLvalue) { return (std::static_pointer_cast<Parser::IndexLValueNode>(astNode)->type = type); }
                return (std::static_pointer_cast<Parser::IndexExprNode>(astNode)->type = type);
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
            case Parser::LVALUE_EXPR: {
                Parser::LValueExprNodePtr const exprNode = std::static_pointer_cast<Parser::LValueExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                return (exprNode->type = typeCheck(exprNode->expr));
            }
            case Parser::ARRAY_EXPR: {
                Parser::ArrayExprNodePtr const exprNode = std::static_pointer_cast<Parser::ArrayExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                TyPtr baseType = nullptr;
                for (const Parser::ExprNodePtr& exprNodePtr : exprNode->expressions->exprs) {
                    TyPtr const type = typeCheck(exprNodePtr);
                    if (!baseType && type) { baseType = type; }
                    if (!areTypesEqual(baseType, type)) {
                        throw Util::CommanderException("Array contains multiple different types, including"
                                                               + getErrorTypeString(baseType) + " and"
                                                               + getErrorTypeString(type),
                                                       exprNode->position);
                    }
                }
                return (exprNode->type = std::make_shared<ArrayTy>(baseType));
            }
            case Parser::TUPLE_EXPR: {
                Parser::TupleExprNodePtr const exprNode = std::static_pointer_cast<Parser::TupleExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                std::vector<TyPtr> expressionTypes;
                for (const Parser::ExprNodePtr& exprNodePtr : exprNode->expressions->exprs) {
                    expressionTypes.push_back(typeCheck(exprNodePtr));
                }
                return (exprNode->type = std::make_shared<TupleTy>(expressionTypes));
            }
            case Parser::TERNARY_EXPR: {
                Parser::TernaryExprNodePtr const exprNode = std::static_pointer_cast<Parser::TernaryExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                TyPtr const conditionType = typeCheck(exprNode->condition);
                assertType(typeCheck(exprNode->condition), BOOL_TY, exprNode->condition->position);
                TyPtr const trueType = typeCheck(exprNode->trueExpr);
                TyPtr const falseType = typeCheck(exprNode->falseExpr);
                if (!areTypesEqual(trueType, falseType)) {
                    throw Util::CommanderException(
                            "Ternary true and false expressions do not match the same type. True expression has type"
                                    + getErrorTypeString(trueType) + " but false expression has type"
                                    + getErrorTypeString(falseType),
                            exprNode->falseExpr->position);
                }
                return (exprNode->type = trueType ? trueType : falseType);
            }
            case Parser::UNOP_EXPR: {
                Parser::UnOpExprNodePtr const exprNode = std::static_pointer_cast<Parser::UnOpExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                TyPtr const expressionType = typeCheck(exprNode->node);
                switch (exprNode->opType) {
                    case Parser::NOT:
                        assertType(expressionType, BOOL_TY, exprNode->node->position);
                        return (exprNode->type = BOOL_TY);
                    case Parser::NEGATE:
                    case Parser::PRE_INCREMENT:
                    case Parser::POST_INCREMENT:
                    case Parser::PRE_DECREMENT:
                    case Parser::POST_DECREMENT:
                        if (expressionType && expressionType->getType() != Type::INT
                            && expressionType->getType() != Type::FLOAT) {
                            throw Util::CommanderException("Expected INT or FLOAT type, but got"
                                                                   + getErrorTypeString(expressionType) + " instead.",
                                                           exprNode->node->position);
                        }
                        if (exprNode->opType == Parser::NEGATE) { break; }
                        if (exprNode->node->nodeType() != Parser::INDEX_LVALUE
                            && exprNode->node->nodeType() != Parser::VAR_LVALUE) {
                            throw Util::CommanderException(
                                    "Expected variable or index lvalue in decrement or increment operation, but "
                                    "found " + Parser::nodeTypeToString(exprNode->node->nodeType())
                                            + " instead",
                                    exprNode->node->position);
                        }
                        if (exprNode->node->nodeType() == Parser::VAR_LVALUE) {
                            std::string const varName
                                    = std::static_pointer_cast<Parser::VarLValueNode>(exprNode->node)->variable;
                            if (_table->getVariable(varName)->constant) {
                                throw Util::CommanderException("Unable to increment or decrement constant variable '"
                                                                       + varName + "'",
                                                               exprNode->node->position);
                            }
                        }
                }
                return (exprNode->type = expressionType);
            }
            case Parser::BINOP_EXPR: {
                Parser::BinOpExprNodePtr const exprNode = std::static_pointer_cast<Parser::BinOpExprNode>(astNode);
                // DON'T do this, in case variables need to be set again for functions: if (exprNode->type) { return
                // exprNode->type; }
                bool const isVarLValue = exprNode->left->nodeType() == Parser::VAR_LVALUE;
                bool const isLvalue = isVarLValue || exprNode->left->nodeType() == Parser::INDEX_LVALUE;
                bool const isBinding = exprNode->left->nodeType() == Parser::BINDING;
                bool const isFirst
                        = isBinding
                               && !_table->hasVariable(
                                       std::static_pointer_cast<Parser::BindingNode>(exprNode->left)->variable)
                       || isVarLValue
                                  && !_table->hasVariable(
                                          std::static_pointer_cast<Parser::VarLValueNode>(exprNode->left)->variable);
                TyPtr leftTy = nullptr;
                if (isBinding == isFirst) {
                    leftTy = typeCheck(exprNode->left);
                } else if (!isFirst && (isBinding || isVarLValue)) {
                    leftTy = getVarType(
                            isBinding ? std::static_pointer_cast<Parser::BindingNode>(exprNode->left)->variable
                                      : std::static_pointer_cast<Parser::VarLValueNode>(exprNode->left)->variable,
                            VARIABLE_INFO, exprNode->left->position);
                }
                TyPtr rightTy = typeCheck(exprNode->right);
                const bool areEqual = areTypesEqual(leftTy, rightTy);
                const bool areIntFloat = (!leftTy || leftTy->getType() == FLOAT || leftTy->getType() == INT)
                                      && (!rightTy || rightTy->getType() == FLOAT || rightTy->getType() == INT);
                const bool areString = (!leftTy || leftTy->getType() == STRING)
                                    && (!rightTy || rightTy->getType() == STRING);
                const bool areBool = (!leftTy || leftTy->getType() == BOOL) && (!rightTy || rightTy->getType() == BOOL);
                const bool areInt = (!leftTy || leftTy->getType() == INT) && (!rightTy || rightTy->getType() == INT);
                const bool areUnknown = !leftTy && !rightTy;
                switch (exprNode->opType) {
                    case Parser::LESSER:
                    case Parser::GREATER:
                    case Parser::LESSER_EQUAL:
                    case Parser::GREATER_EQUAL:
                        // Int/float | float/float | Int/int | string/string --> bool
                        if (areIntFloat || areString) { return (exprNode->type = BOOL_TY); }
                        throw Util::CommanderException(
                                "Inequality operation '" + Parser::binOpToString(exprNode->opType)
                                        + "' has incompatible types. Left and right types should be INT, FLOAT, or "
                                          "STRING. Left: "
                                        + getErrorTypeString(leftTy) + ", Right: " + getErrorTypeString(rightTy),
                                exprNode->position);
                    case Parser::EQUAL:
                    case Parser::NOT_EQUAL:
                        // any/any --> bool
                        if (areIntFloat || areEqual) { return (exprNode->type = BOOL_TY); }
                        throw Util::CommanderException(
                                "Equality operation '" + Parser::binOpToString(exprNode->opType)
                                        + "' has incompatible types. Left and right types should be the same. Left: "
                                        + getErrorTypeString(leftTy) + ", Right: " + getErrorTypeString(rightTy),
                                exprNode->position);
                    case Parser::AND:
                    case Parser::OR:
                        // bool/bool --> bool
                        if (areBool) { return (exprNode->type = BOOL_TY); }
                        throw Util::CommanderException(
                                "Boolean operation '" + Parser::binOpToString(exprNode->opType)
                                        + "' has incompatible types. Both left and right types should be BOOL. Left: "
                                        + getErrorTypeString(leftTy) + ", Right: " + getErrorTypeString(rightTy),
                                exprNode->position);
                    case Parser::EXPONENTIATE_SET:
                    case Parser::MULTIPLY_SET:
                    case Parser::DIVIDE_SET:
                    case Parser::MODULO_SET:
                    case Parser::ADD_SET:
                    case Parser::SUBTRACT_SET:
                        if (!isLvalue) {
                            throw Util::CommanderException(
                                    "Invalid set expression. Must have variable or index lvalue on left side of '"
                                            + Parser::binOpToString(exprNode->opType) + "', but found "
                                            + Parser::nodeTypeToString(exprNode->left->nodeType()) + " instead",
                                    exprNode->left->position);
                        }
                        if (!areEqual && areIntFloat && leftTy->getType() == INT) {
                            throw Util::CommanderException("Cannot update int to float in operation set expression.",
                                                           exprNode->position);
                        }
                        if (!areEqual && exprNode->opType == Parser::ADD_SET && rightTy->getType() == STRING) {
                            throw Util::CommanderException("Cannot update" + getErrorTypeString(leftTy)
                                                                   + " to STRING in add set expression.",
                                                           exprNode->position);
                        }
                        if (isVarLValue) {
                            std::string const varName
                                    = std::static_pointer_cast<Parser::VarLValueNode>(exprNode->left)->variable;
                            assertVariableExists(varName, exprNode->left->position);
                            if (_table->getVariable(varName)->constant) {
                                throw Util::CommanderException("Unable to update constant variable '" + varName + "'",
                                                               exprNode->left->position);
                            }
                        }
                        if (!leftTy) {
                            std::static_pointer_cast<Parser::LValueNode>(exprNode->left)->type = rightTy;
                            if (isVarLValue) {
                                _table->getVariable(
                                              std::static_pointer_cast<Parser::VarLValueNode>(exprNode->left)->variable)
                                        ->types[0]
                                        = rightTy;
                            } else {
                                Parser::IndexLValueNodePtr const lvalue
                                        = std::static_pointer_cast<Parser::IndexLValueNode>(exprNode->left);
                                if (lvalue->lvalue->type && lvalue->lvalue->type->getType() == ARRAY) {
                                    std::static_pointer_cast<ArrayTy>(lvalue->lvalue->type)->baseType = rightTy;
                                } else if (lvalue->lvalue->type && lvalue->index->nodeType() == Parser::INT_EXPR) {
                                    int64_t const index
                                            = std::static_pointer_cast<Parser::IntExprNode>(lvalue->index)->value;
                                    TupleTyPtr const tupleTy = std::static_pointer_cast<TupleTy>(lvalue->lvalue->type);
                                    tupleTy->contentTypes[index] = rightTy;
                                }
                            }
                        }
                        // NO break here! Should continue to the next lines of code that handle add, subtract, etc.
                    case Parser::EXPONENTIATE:
                    case Parser::MULTIPLY:
                    case Parser::DIVIDE:
                    case Parser::MODULO:
                    case Parser::ADD:
                    case Parser::SUBTRACT:
                        // Unknown/Unknown --> Unknown
                        // Int/int --> int
                        // Int/float | float/float --> float
                        // string/string | any/string | string/any --> string (in the case of ADD and ADD_EQUAL)
                        if (areUnknown) { return nullptr; }
                        if (areInt) { return (exprNode->type = INT_TY); }
                        if (areIntFloat) { return (exprNode->type = FLOAT_TY); }
                        if ((leftTy && leftTy->getType() == STRING || rightTy && rightTy->getType() == STRING)
                            && (exprNode->opType == Parser::ADD || exprNode->opType == Parser::ADD_SET)) {
                            return (exprNode->type = STRING_TY);
                        }
                        throw Util::CommanderException(
                                "Binary operation '" + Parser::binOpToString(exprNode->opType)
                                        + "' has incompatible types. Left and right types should be INT, FLOAT, or "
                                          "STRING (if doing ADD for string concatenation). Left: "
                                        + getErrorTypeString(leftTy) + ", Right: " + getErrorTypeString(rightTy),
                                exprNode->position);
                    default:
                        if (!isLvalue && !isBinding) {
                            throw Util::CommanderException(
                                    "Invalid set expression. Must have variable or index lvalue on left side of '=', "
                                    "but found "
                                            + Parser::nodeTypeToString(exprNode->left->nodeType()) + " instead",
                                    exprNode->left->position);
                        }
                        if (!areEqual
                            || isBinding && std::static_pointer_cast<Parser::BindingNode>(exprNode->left)->type
                                       && !areTypesEqual(
                                               typeCheck(std::static_pointer_cast<Parser::BindingNode>(exprNode->left)
                                                                 ->type),
                                               rightTy)) {
                            throw Util::CommanderException(
                                    "Expected the type"
                                            + getErrorTypeString(
                                                    isBinding ? typeCheck(std::static_pointer_cast<Parser::BindingNode>(
                                                                                  exprNode->left)
                                                                                  ->type)
                                                              : leftTy)
                                            + " for the expression right of '=' to match left, but got"
                                            + getErrorTypeString(rightTy) + " instead",
                                    exprNode->right->position);
                        }
                        if (isBinding || isVarLValue) {
                            std::string const varName
                                    = isBinding
                                            ? std::static_pointer_cast<Parser::BindingNode>(exprNode->left)->variable
                                            : std::static_pointer_cast<Parser::VarLValueNode>(exprNode->left)->variable;
                            bool const isConstant = !isFirst && _table->getVariable(varName)->constant;
                            if (isConstant) {
                                throw Util::CommanderException("Unable to update constant variable '" + varName + "'",
                                                               exprNode->left->position);
                            }
                            rightTy = isBinding && std::static_pointer_cast<Parser::BindingNode>(exprNode->left)->type
                                            ? typeCheck(
                                                    std::static_pointer_cast<Parser::BindingNode>(exprNode->left)->type)
                                            : rightTy;
                            if (isFirst) {
                                _table->addVariable(
                                        varName,
                                        std::make_shared<VariableInfo>(
                                                isBinding
                                                        && std::static_pointer_cast<Parser::BindingNode>(exprNode->left)
                                                                   ->constant,
                                                rightTy));
                            } else {
                                getVarType(varName, VARIABLE_INFO, exprNode->left->position);
                                const VarInfoPtr info = _table->getVariable(varName);
                                if (isBinding) {
                                    info->constant
                                            = std::static_pointer_cast<Parser::BindingNode>(exprNode->left)->constant;
                                }
                                info->types[0] = rightTy;
                            }
                        } else if (!leftTy) {
                            Parser::IndexLValueNodePtr const lvalue = std::static_pointer_cast<Parser::IndexLValueNode>(
                                    exprNode->left);
                            if (lvalue->lvalue->type && lvalue->lvalue->type->getType() == ARRAY) {
                                std::static_pointer_cast<ArrayTy>(lvalue->lvalue->type)->baseType = rightTy;
                            } else if (lvalue->lvalue->type && lvalue->index->nodeType() == Parser::INT_EXPR) {
                                int64_t const index
                                        = std::static_pointer_cast<Parser::IntExprNode>(lvalue->index)->value;
                                TupleTyPtr const tupleTy = std::static_pointer_cast<TupleTy>(lvalue->lvalue->type);
                                tupleTy->contentTypes[index] = rightTy;
                            }
                        }
                        return (exprNode->type = rightTy);
                }
            }
            case Parser::CALL_EXPR: {
                Parser::CallExprNodePtr const exprNode = std::static_pointer_cast<Parser::CallExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                const bool isVarLValue
                        = exprNode->func->nodeType() == Parser::LVALUE_EXPR
                       && std::static_pointer_cast<Parser::LValueExprNode>(exprNode->func)->expr->nodeType()
                                  == Parser::VAR_EXPR;
                TyPtr type;
                std::vector<TyPtr> functionTypes;
                if (isVarLValue) {
                    std::string const varName
                            = std::static_pointer_cast<Parser::VarExprNode>(
                                      std::static_pointer_cast<Parser::LValueExprNode>(exprNode->func)->expr)
                                      ->variable;
                    if (std::find(_funcs.begin(), _funcs.end(), varName) != _funcs.end()) { return nullptr; }
                    assertVariableExists(varName, exprNode->func->position);
                    VarInfoPtr const functionInfo = _table->getVariable(varName);
                    type = functionInfo->types[0];
                    functionTypes = functionInfo->types;
                } else {
                    type = typeCheck(exprNode->func);
                    functionTypes.push_back(type);
                }
                assertType(type, FUNCTION_TY, exprNode->func->position);
                std::vector<TyPtr> argTypes;
                for (const Parser::ExprNodePtr& expr : exprNode->args->exprs) { argTypes.push_back(typeCheck(expr)); }
                FunctionTyPtr functionType = nullptr;
                bool unknownFunction = false;
                for (const TyPtr& typ : functionTypes) {
                    if (!typ) {
                        unknownFunction = true;
                        continue;
                    }
                    FunctionTyPtr const functionTy = std::static_pointer_cast<FunctionTy>(typ);
                    size_t const size = functionTy->parameters.size();
                    if (size != argTypes.size()) { continue; }
                    bool areEqual = true;
                    for (int i = 0; i < size; i++) {
                        if (!areTypesEqual(functionTy->parameters[i], argTypes[i])) {
                            areEqual = false;
                            break;
                        }
                    }
                    if (!areEqual) { continue; }
                    functionType = functionTy;
                    break;
                }
                if (!functionType && !unknownFunction) {
                    const TyPtr callType = std::make_shared<FunctionTy>(
                            argTypes, std::static_pointer_cast<FunctionTy>(type)->returnType);
                    if (isVarLValue) {
                        std::string const varName
                                = std::static_pointer_cast<Parser::VarExprNode>(
                                          std::static_pointer_cast<Parser::LValueExprNode>(exprNode->func)->expr)
                                          ->variable;
                        std::stringstream functionSignatures;
                        for (const TyPtr& typ : functionTypes) {
                            functionSignatures << "\n" << getErrorTypeString(typ);
                        }
                        throw Util::CommanderException(
                                "The parameters being used in call expression don't match any function signatures of '"
                                        + varName + "'. Calling signature:" + getErrorTypeString(callType)
                                        + "\nPossible function signatures:" + functionSignatures.str(),
                                exprNode->position);
                    }
                    throw Util::CommanderException("The parameters being used in call expression don't match the "
                                                   "function signature. Function signature:"
                                                           + getErrorTypeString(type)
                                                           + ", Calling signature:" + getErrorTypeString(callType),
                                                   exprNode->position);
                }
                if (isVarLValue) {
                    const Parser::LValueExprNodePtr lvalue = std::static_pointer_cast<Parser::LValueExprNode>(
                            exprNode->func);
                    const Parser::VarExprNodePtr varExpr = std::static_pointer_cast<Parser::VarExprNode>(lvalue->expr);
                    lvalue->type = functionType;
                    varExpr->type = functionType;
                }
                return (exprNode->type = functionType ? functionType->returnType : functionType);
            }
            case Parser::API_CALL_EXPR: {
                Parser::ApiCallExprNodePtr const exprNode = std::static_pointer_cast<Parser::ApiCallExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                exprNode->args->exprs.insert(exprNode->args->exprs.begin(), exprNode->expression);
                assertVariableExists(exprNode->func, exprNode->funcPosition);
                VarInfoPtr const functionInfo = _table->getVariable(exprNode->func);
                const TyPtr type = functionInfo->types[0];
                const std::vector<TyPtr> functionTypes = functionInfo->types;
                std::vector<TyPtr> argTypes;
                for (const Parser::ExprNodePtr& expr : exprNode->args->exprs) { argTypes.push_back(typeCheck(expr)); }
                FunctionTyPtr functionType = nullptr;
                bool unknownFunction = false;
                for (const TyPtr& typ : functionTypes) {
                    if (!typ) {
                        unknownFunction = true;
                        continue;
                    }
                    FunctionTyPtr const functionTy = std::static_pointer_cast<FunctionTy>(typ);
                    size_t const size = functionTy->parameters.size();
                    if (size != argTypes.size()) { continue; }
                    bool areEqual = true;
                    for (int i = 0; i < size; i++) {
                        if (!areTypesEqual(functionTy->parameters[i], argTypes[i])) {
                            areEqual = false;
                            break;
                        }
                    }
                    if (!areEqual) { continue; }
                    functionType = functionTy;
                    break;
                }
                if (!functionType && !unknownFunction) {
                    std::stringstream functionSignatures;
                    for (const TyPtr& typ : functionTypes) { functionSignatures << "\n" << getErrorTypeString(typ); }
                    const TyPtr callType = std::make_shared<FunctionTy>(
                            argTypes, std::static_pointer_cast<FunctionTy>(type)->returnType);
                    throw Util::CommanderException(
                            "The parameters being used in call expression don't match any function signatures of '"
                                    + exprNode->func + "'. Calling signature:" + getErrorTypeString(callType)
                                    + "\nPossible function signatures:" + functionSignatures.str(),
                            exprNode->position);
                }
                if (exprNode->func == "append" && argTypes[0]) {
                    ArrayTyPtr const arrayTy = std::static_pointer_cast<ArrayTy>(argTypes[0]);
                    if (!arrayTy->baseType) {
                        arrayTy->baseType = argTypes[1];
                    } else if (!areTypesEqual(arrayTy->baseType, argTypes[1])) {
                        throw Util::CommanderException("Cannot append type" + getErrorTypeString(argTypes[1])
                                                               + " to array of type" + getErrorTypeString(arrayTy),
                                                       exprNode->position);
                    }
                }
                return (exprNode->type = functionType ? functionType->returnType : functionType);
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
                TyPtr bodyReturnType = typeCheck(exprNode->body);
                if (!bodyReturnType) {
                    bodyReturnType = exprNode->body->nodeType() == Parser::RETURN_STMT
                                                  || (exprNode->body->nodeType() == Parser::SCOPE_STMT
                                                      && std::static_pointer_cast<Parser::ScopeStmtNode>(exprNode->body)
                                                                         ->stmts->stmts.back()
                                                                         ->nodeType()
                                                                 == Parser::RETURN_STMT)
                                           ? nullptr
                                           : VOID_TY;
                }
                if (!returnType) {
                    returnType = bodyReturnType;
                } else if (!areTypesEqual(returnType, bodyReturnType)) {
                    throw Util::CommanderException(
                            "Return types for the function declaration don't match. Expected return type"
                                    + getErrorTypeString(returnType) + " but got"
                                    + getErrorTypeString(typeCheck(exprNode->body)) + " instead",
                            exprNode->position);
                } else if (!bodyReturnType) {
                    if (exprNode->body->nodeType() == Parser::RETURN_STMT) {
                        std::static_pointer_cast<Parser::ReturnStmtNode>(exprNode->body)->retExpr->type = returnType;
                    } else {
                        std::static_pointer_cast<Parser::ReturnStmtNode>(
                                std::static_pointer_cast<Parser::ScopeStmtNode>(exprNode->body)->stmts->stmts.back())
                                ->retExpr->type
                                = returnType;
                    }
                }
                exprNode->table = popScope();
                return (exprNode->type = std::make_shared<FunctionTy>(bindings, returnType));
            }
            case Parser::CMD_EXPR: {
                Parser::CmdExprNodePtr const exprNode = std::static_pointer_cast<Parser::CmdExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                typeCheck(exprNode->cmd);
                return (exprNode->type = std::make_shared<TupleTy>(std::vector<TyPtr> {STRING_TY, STRING_TY, INT_TY}));
            }
            case Parser::SCAN_EXPR: {
                Parser::ScanExprNodePtr const exprNode = std::static_pointer_cast<Parser::ScanExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                assertType(typeCheck(exprNode->prompt), STRING_TY, exprNode->prompt->position);
                return (exprNode->type = STRING_TY);
            }
            case Parser::READ_EXPR: {
                Parser::ReadExprNodePtr const exprNode = std::static_pointer_cast<Parser::ReadExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                assertType(typeCheck(exprNode->filePath), STRING_TY, exprNode->filePath->position);
                return (exprNode->type = STRING_TY);
            }
            case Parser::EXPRS: {
                Parser::ExprsNodePtr const exprsPtr = std::static_pointer_cast<Parser::ExprsNode>(astNode);
                for (const Parser::ExprNodePtr& exprPtr : exprsPtr->exprs) { typeCheck(exprPtr); }
                return nullptr;
            }
            case Parser::PRGM: {
                Parser::PrgmNodePtr const program = std::static_pointer_cast<Parser::PrgmNode>(astNode);
                typeCheck(program->stmts);
                return nullptr;
            }
            case Parser::IF_STMT: {
                Parser::IfStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::IfStmtNode>(astNode);
                assertType(typeCheck(stmtNode->condition), BOOL_TY, stmtNode->condition->position);
                typeCheck(stmtNode->trueStmt);
                typeCheck(stmtNode->falseStmt);
                return nullptr;
            }
            case Parser::FOR_STMT: {
                Parser::ForStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::ForStmtNode>(astNode);
                pushScope();
                typeCheck(stmtNode->initial);
                assertType(typeCheck(stmtNode->condition), BOOL_TY, stmtNode->condition->position);
                typeCheck(stmtNode->update);
                typeCheck(stmtNode->body);
                popScope();
                return nullptr;
            }
            case Parser::WHILE_STMT: {
                Parser::WhileStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::WhileStmtNode>(astNode);
                assertType(typeCheck(stmtNode->condition), BOOL_TY, stmtNode->condition->position);
                typeCheck(stmtNode->body);
                return nullptr;
            }
            case Parser::DO_WHILE_STMT: {
                Parser::DoWhileStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::DoWhileStmtNode>(astNode);
                typeCheck(stmtNode->body);
                assertType(typeCheck(stmtNode->condition), BOOL_TY, stmtNode->condition->position);
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
                typeCheck(stmtNode->command);
                return nullptr;
            }
            case Parser::EXPR_STMT: {
                Parser::ExprStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::ExprStmtNode>(astNode);
                typeCheck(stmtNode->expression);
                return nullptr;
            }
            case Parser::ALIAS_STMT: {
                Parser::AliasStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::AliasStmtNode>(astNode);
                assertVariableNotExists(stmtNode->alias, stmtNode->position);
                typeCheck(stmtNode->command);
                _table->addVariable(stmtNode->alias, std::make_shared<AliasInfo>());
                return nullptr;
            }
            case Parser::IMPORT_STMT: {
                Parser::ImportStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::ImportStmtNode>(astNode);
                typeCheck(stmtNode->filePath);
                Parser::StringNodePtr currentNode = stmtNode->filePath;
                while (!currentNode->isLiteral()) {
                    if (currentNode->expressions->expressions.size() != 1) {
                        throw Util::CommanderException(
                                "Expected string literal for file path, but got an interpolated string",
                                stmtNode->filePath->position);
                    }
                    currentNode = std::static_pointer_cast<Parser::StringExprNode>(
                                          currentNode->expressions->expressions[0])
                                          ->stringNode;
                }
                std::string const filePath = currentNode->literal;
                Lexer::TokenList tokens;
                Lexer::tokenize(tokens, filePath);
                stmtNode->prgm = _parser->parse(tokens);
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
                assertType(typeCheck(stmtNode->fileData), STRING_TY, stmtNode->fileData->position);
                assertType(typeCheck(stmtNode->filePath), STRING_TY, stmtNode->filePath->position);
                return nullptr;
            }
            case Parser::TYPE_STMT: {
                Parser::TypeStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::TypeStmtNode>(astNode);
                assertVariableNotExists(stmtNode->alias, stmtNode->position);
                _table->addVariable(stmtNode->alias, std::make_shared<TypeInfo>(typeCheck(stmtNode->type)));
                return nullptr;
            }
            case Parser::FUNCTION_STMT: {
                Parser::FunctionStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::FunctionStmtNode>(
                        astNode);
                assertVariableNotExists(stmtNode->name, stmtNode->position);
                pushScope();
                _funcs.push_back(stmtNode->name);
                std::vector<TyPtr> bindings;
                for (const Parser::BindingNodePtr& binding : stmtNode->bindings->bindings) {
                    bindings.push_back(typeCheck(binding));
                }
                TyPtr returnType = typeCheck(stmtNode->returnType);
                TyPtr bodyReturnType = typeCheck(stmtNode->body);
                if (!bodyReturnType) {
                    bodyReturnType = stmtNode->body->nodeType() == Parser::RETURN_STMT
                                                  || (stmtNode->body->nodeType() == Parser::SCOPE_STMT
                                                      && std::static_pointer_cast<Parser::ScopeStmtNode>(stmtNode->body)
                                                                         ->stmts->stmts.back()
                                                                         ->nodeType()
                                                                 == Parser::RETURN_STMT)
                                           ? nullptr
                                           : VOID_TY;
                }
                if (!returnType) {
                    returnType = bodyReturnType;
                    stmtNode->returnType = std::make_shared<Parser::TupleTypeNode>(stmtNode->body->position);
                    stmtNode->returnType->type = returnType;
                } else if (!areTypesEqual(returnType, bodyReturnType)) {
                    throw Util::CommanderException(
                            "Return types for the function declaration don't match. Expected return type"
                                    + getErrorTypeString(returnType) + " but got"
                                    + getErrorTypeString(typeCheck(stmtNode->body)) + " instead",
                            stmtNode->position);
                } else if (!bodyReturnType) {
                    if (stmtNode->body->nodeType() == Parser::RETURN_STMT) {
                        std::static_pointer_cast<Parser::ReturnStmtNode>(stmtNode->body)->retExpr->type = returnType;
                    } else {
                        std::static_pointer_cast<Parser::ReturnStmtNode>(
                                std::static_pointer_cast<Parser::ScopeStmtNode>(stmtNode->body)->stmts->stmts.back())
                                ->retExpr->type
                                = returnType;
                    }
                }
                _funcs.pop_back();
                popScope();
                _table->addVariable(stmtNode->name, std::make_shared<FunctionInfo>(std::vector<TyPtr> {
                                                            std::make_shared<FunctionTy>(bindings, returnType)}));
                pushScope();
                typeCheck(stmtNode->bindings);
                typeCheck(stmtNode->body);
                stmtNode->table = popScope();
                return nullptr;
            }
            case Parser::TIMEOUT_STMT: {
                Parser::TimeoutStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::TimeoutStmtNode>(astNode);
                typeCheck(stmtNode->message);
                typeCheck(stmtNode->stmt);
                return nullptr;
            }
            case Parser::ASSERT_STMT: {
                Parser::AssertStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::AssertStmtNode>(astNode);
                typeCheck(stmtNode->message);
                assertType(typeCheck(stmtNode->expr), BOOL_TY, stmtNode->position);
                return nullptr;
            }
            case Parser::STMTS: {
                Parser::StmtsNodePtr const stmtsPtr = std::static_pointer_cast<Parser::StmtsNode>(astNode);
                TyPtr returnType = VOID_TY;
                int idx = 0;
                for (const Parser::StmtNodePtr& stmtPtr : stmtsPtr->stmts) {
                    TyPtr const type = typeCheck(stmtPtr);
                    if (stmtPtr->nodeType() == Parser::RETURN_STMT) {
                        returnType = type;
                        if (idx + 1 != stmtsPtr->stmts.size()) {
                            throw Util::CommanderException("Return statement is not the last statement in the scope",
                                                           stmtPtr->position);
                        }
                        break;
                    }
                    idx++;
                }
                return returnType;
            }
            case Parser::STRING: {
                Parser::StringNodePtr const stringPtr = std::static_pointer_cast<Parser::StringNode>(astNode);
                typeCheck(stringPtr->expressions);
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
                assertVariableExists(variableTypePtr->name, variableTypePtr->position);
                return (variableTypePtr->type = getVarType(variableTypePtr->name, TYPE_INFO,
                                                           variableTypePtr->position));
            }
            case Parser::TYPES: {
                Parser::TypesNodePtr const typesPtr = std::static_pointer_cast<Parser::TypesNode>(astNode);
                for (const Parser::TypeNodePtr& typePtr : typesPtr->types) { typeCheck(typePtr); }
                return nullptr;
            }
            default:
                throw Util::CommanderException("Unexpected error occurred with type checking (possible abstract AST "
                                               "node accidentally created)",
                                               astNode->position);
        }
    }

    void TypeChecker::assertVariableExists(const std::string& variable, const Lexer::FilePosition& position) {
        if (!_table->hasVariable(variable)) {
            throw Util::CommanderException("Variable '" + variable + "' has not been initialized yet.", position);
        }
    }

    void TypeChecker::assertVariableNotExists(const std::string& variable, const Lexer::FilePosition& position) {
        if (_table->hasVariable(variable)) {
            throw Util::CommanderException("Variable of name '" + variable + "' already exists in current scope.",
                                           position);
        }
    }

    void TypeChecker::assertType(const TyPtr& type, const TyPtr& expectedType, const Lexer::FilePosition& position) {
        if (!areTypesEqual(expectedType, type)) {
            throw Util::CommanderException("Expected" + getErrorTypeString(expectedType) + " type, but got"
                                                   + getErrorTypeString(type) + " instead.",
                                           position);
        }
    }

    void TypeChecker::pushScope() { _table = std::make_shared<VariableTable>(_table); }

    VariableTablePtr TypeChecker::popScope() {
        VariableTablePtr scope = _table;
        auto parent = _table->getParent();
        if (parent) { _table = parent; }
        return scope;
    }

    TyPtr TypeChecker::getVarType(const std::string& varName, const InfoType& infoType,
                                  const Lexer::FilePosition& variablePosition) {
        const VarInfoPtr variableInfo = _table->getVariable(varName);
        if (variableInfo->infoType() != infoType) {
            throw Util::CommanderException("Expected the variable named '" + varName + "' to be a "
                                                   + getVarTypeString(infoType) + " variable, but got a "
                                                   + getVarTypeString(variableInfo->infoType()) + " variable instead.",
                                           variablePosition);
        }
        // Guaranteed to have at least 1 type in the types list, if variableInfo exists that is
        return variableInfo ? variableInfo->types[0] : nullptr;
    }

    std::string TypeChecker::getVarTypeString(const InfoType& infoType) {
        switch (infoType) {
            case FUNCTION_INFO:
                return "function";
            case VARIABLE_INFO:
                return "normal";
            case TYPE_INFO:
                return "type";
            default:
                return "alias";
        }
    }

}  //  namespace TypeChecker
