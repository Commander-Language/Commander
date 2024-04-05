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
        _parser = parser;
    }

    void TypeChecker::typeCheck(const Parser::ASTNodeList& astNodeList) {
        for (const Parser::ASTNodePtr& node : astNodeList) { typeCheck(node); }
    }

    TyPtr TypeChecker::typeCheck(const Parser::ASTNodePtr& astNode) {
        if (!astNode) { return nullptr; }
        switch (astNode->nodeType()) {
            case Parser::BINDING: {
                Parser::BindingNodePtr const binding = std::static_pointer_cast<Parser::BindingNode>(astNode);
                assertNotExists(binding->variable, binding->position);
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
                    TyPtr const nodeType = typeCheck(node);
                    if (nodeType && nodeType->getType() != Type::STRING) {
                        throw Util::CommanderException("Command has an argument that is of type"
                                                               + getErrorTypeString(nodeType)
                                                               + " instead of type STRING",
                                                       node->position);
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
                return (lvalue->type = _table->hasVariable(lvalue->variable)
                                             ? getVarType(lvalue->variable, VARIABLE_INFO, lvalue->position)
                                             : nullptr);
            }
            case Parser::INDEX_LVALUE: {
                Parser::IndexLValueNodePtr const lvalue = std::static_pointer_cast<Parser::IndexLValueNode>(astNode);
                if (lvalue->type) { return lvalue->type; }
                // TODO: The exact same as index expr, figure out a way to use the same code
                TyPtr const indexType = typeCheck(lvalue->index);
                if (indexType && indexType->getType() != Type::INT) {
                    throw Util::CommanderException("Tried to index an array or tuple with type"
                                                           + getErrorTypeString(indexType) + " instead of type INT",
                                                   lvalue->index->position);
                }
                TyPtr const exprType = typeCheck(lvalue->lvalue);
                if (!exprType) { return (lvalue->type = nullptr); }
                switch (exprType->getType()) {
                    case ARRAY: {
                        ArrayTyPtr const arrayTy = std::static_pointer_cast<ArrayTy>(exprType);
                        if (!arrayTy->baseType) {
                            throw Util::CommanderException("Unable to index empty array", lvalue->lvalue->position);
                        }
                        return (lvalue->type = arrayTy->baseType);
                    }
                    case TUPLE: {
                        TupleTyPtr const tupleTy = std::static_pointer_cast<TupleTy>(exprType);
                        if (tupleTy->contentTypes.size() == 0) {
                            throw Util::CommanderException("Unable to index empty tuple", lvalue->lvalue->position);
                        }
                        if (lvalue->index->nodeType() == Parser::INT_EXPR) {
                            int64_t const index = std::static_pointer_cast<Parser::IntExprNode>(lvalue->index)->value;
                            if (tupleTy->contentTypes.size() <= index) {
                                throw Util::CommanderException("Index out of bounds on tuple. Tuple size is "
                                                                       + std::to_string(tupleTy->contentTypes.size())
                                                                       + " while index is " + std::to_string(index),
                                                               lvalue->index->position);
                            }
                            return (lvalue->type = tupleTy->contentTypes[index]);
                        }
                        return (lvalue->type = nullptr);
                    }
                    default:
                        throw Util::CommanderException("Tried to index a type" + getErrorTypeString(exprType)
                                                               + " instead of an ARRAY or TUPLE",
                                                       lvalue->lvalue->position);
                }
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
                if (!_table->hasVariable(exprNode->variable)) {
                    throw Util::CommanderException(
                            "Variable '" + exprNode->variable + "' has not been initialized yet.", exprNode->position);
                }
                return (exprNode->type = getVarType(exprNode->variable, VARIABLE_INFO, exprNode->position));
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
                    for (const Parser::ExprNodePtr& exprNodePtr : exprNode->expressions->exprs) {
                        TyPtr const type2 = typeCheck(exprNodePtr);
                        if (!areTypesEqual(type, type2)) {
                            throw Util::CommanderException("Array contains multiple different types, including"
                                                                   + getErrorTypeString(type) + " and"
                                                                   + getErrorTypeString(type2),
                                                           exprNode->position);
                        }
                    }
                }
                return (exprNode->type = std::make_shared<ArrayTy>(type));
            }
            case Parser::INDEX_EXPR: {
                Parser::IndexExprNodePtr const exprNode = std::static_pointer_cast<Parser::IndexExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                TyPtr const indexType = typeCheck(exprNode->index);
                if (indexType && indexType->getType() != Type::INT) {
                    throw Util::CommanderException("Tried to index an array or tuple with type"
                                                           + getErrorTypeString(indexType) + " instead of type INT",
                                                   exprNode->index->position);
                }
                TyPtr const exprType = typeCheck(exprNode->expr);
                if (!exprType) { return (exprNode->type = nullptr); }
                switch (exprType->getType()) {
                    case ARRAY: {
                        ArrayTyPtr const arrayTy = std::static_pointer_cast<ArrayTy>(exprType);
                        if (!arrayTy->baseType) {
                            throw Util::CommanderException("Unable to index empty array", exprNode->expr->position);
                        }
                        return (exprNode->type = arrayTy->baseType);
                    }
                    case TUPLE: {
                        TupleTyPtr const tupleTy = std::static_pointer_cast<TupleTy>(exprType);
                        if (tupleTy->contentTypes.size() == 0) {
                            throw Util::CommanderException("Unable to index empty tuple", exprNode->expr->position);
                        }
                        if (exprNode->index->nodeType() == Parser::INT_EXPR) {
                            int64_t const index = std::static_pointer_cast<Parser::IntExprNode>(exprNode->index)->value;
                            if (tupleTy->contentTypes.size() <= index) {
                                throw Util::CommanderException("Index out of bounds on tuple. Tuple size is "
                                                                       + std::to_string(tupleTy->contentTypes.size())
                                                                       + " while index is " + std::to_string(index),
                                                               exprNode->index->position);
                            }
                            return (exprNode->type = tupleTy->contentTypes[index]);
                        }
                        return (exprNode->type = nullptr);
                    }
                    default:
                        throw Util::CommanderException("Tried to index a type" + getErrorTypeString(exprType)
                                                               + " instead of an ARRAY or TUPLE",
                                                       exprNode->expr->position);
                }
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
                if (conditionType && conditionType->getType() != Type::BOOL) {
                    throw Util::CommanderException("Ternary condition evaluates to the type"
                                                           + getErrorTypeString(conditionType) + " instead of BOOL",
                                                   exprNode->condition->position);
                }
                TyPtr const trueType = typeCheck(exprNode->trueExpr);
                TyPtr const falseType = typeCheck(exprNode->falseExpr);
                if (!areTypesEqual(trueType, falseType)) {
                    throw Util::CommanderException(
                            "True and false expressions do not match the same type. True expression has type"
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
                bool const isTuple
                        = exprNode->node->nodeType() == Parser::INDEX_LVALUE
                       && std::static_pointer_cast<Parser::IndexLValueNode>(exprNode->node)->lvalue->type
                       && std::static_pointer_cast<Parser::IndexLValueNode>(exprNode->node)->lvalue->type->getType()
                                  == TUPLE;
                switch (exprNode->opType) {
                    case Parser::NOT:
                        if (expressionType && expressionType->getType() != Type::BOOL) {
                            throw Util::CommanderException("Got type" + getErrorTypeString(expressionType)
                                                                   + " instead of BOOL inside of ! operation",
                                                           exprNode->node->position);
                        }
                        break;
                    case Parser::NEGATE:
                    case Parser::PRE_INCREMENT:
                    case Parser::POST_INCREMENT:
                    case Parser::PRE_DECREMENT:
                    case Parser::POST_DECREMENT:
                        if (!isTuple && expressionType && expressionType->getType() != Type::INT
                            && expressionType->getType() != Type::FLOAT) {
                            throw Util::CommanderException("Got type" + getErrorTypeString(expressionType)
                                                                   + " instead of INT or FLOAT inside of "
                                                                   + Parser::unOpToString(exprNode->opType)
                                                                   + " operation",
                                                           exprNode->node->position);
                        }
                        if (exprNode->opType != Parser::NEGATE) {
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
                                if (!_table->hasVariable(varName)) {
                                    throw Util::CommanderException("Variable named '" + varName
                                                                           + "' is not initialized.",
                                                                   exprNode->node->position);
                                }
                                VarInfoPtr const varInfo = _table->getVariable(varName);
                                if (varInfo->constant) {
                                    throw Util::CommanderException(
                                            "Unable to increment or decrement constant variable '" + varName + "'",
                                            exprNode->node->position);
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
                               && !_table->hasVariable(
                                       std::static_pointer_cast<Parser::BindingNode>(exprNode->left)->variable)
                       || exprNode->left->nodeType() == Parser::VAR_LVALUE
                                  && !_table->hasVariable(
                                          std::static_pointer_cast<Parser::VarLValueNode>(exprNode->left)->variable);
                TyPtr const leftTy
                        = !isBinding || isFirst
                                ? typeCheck(exprNode->left)
                                : getVarType(std::static_pointer_cast<Parser::BindingNode>(exprNode->left)->variable,
                                             VARIABLE_INFO, exprNode->left->position);
                TyPtr const rightTy = typeCheck(exprNode->right);
                // Right type must be known in all operations
                if (!rightTy) {
                    throw Util::CommanderException("Right expression evaluates to unknown type in binary operation",
                                                   exprNode->right->position);
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
                            throw Util::CommanderException(
                                    "Invalid set expression. Must have variable or index lvalue on left side of '=', "
                                    "but found "
                                            + Parser::nodeTypeToString(exprNode->left->nodeType()) + " instead",
                                    exprNode->left->position);
                        }
                        if (isBinding) {
                            const Parser::BindingNodePtr bindingNode = std::static_pointer_cast<Parser::BindingNode>(
                                    exprNode->left);
                            if (bindingNode->type && !areTypesEqual(typeCheck(bindingNode->type), rightTy)) {
                                throw Util::CommanderException(
                                        "Expected the type"
                                                + getErrorTypeString(typeCheck(
                                                        std::static_pointer_cast<Parser::BindingNode>(exprNode->left)
                                                                ->type))
                                                + " for the expression right of '=' to match left, but got"
                                                + getErrorTypeString(rightTy) + " instead",
                                        exprNode->right->position);
                            }
                        }
                        if (isBinding || exprNode->left->nodeType() == Parser::VAR_LVALUE) {
                            std::string const varName
                                    = isBinding
                                            ? std::static_pointer_cast<Parser::BindingNode>(exprNode->left)->variable
                                            : std::static_pointer_cast<Parser::VarLValueNode>(exprNode->left)->variable;
                            bool const isConstant = !isFirst && _table->getVariable(varName)->constant;
                            if (isConstant) {
                                throw Util::CommanderException("Unable to update constant variable '" + varName + "'",
                                                               exprNode->left->position);
                            }
                            _table->addVariable(
                                    varName,
                                    std::make_shared<VariableInfo>(
                                            isBinding
                                                    && std::static_pointer_cast<Parser::BindingNode>(exprNode->left)
                                                               ->constant,
                                            rightTy));
                        } else if (leftTy) {
                            Parser::IndexLValueNodePtr const lvalue = std::static_pointer_cast<Parser::IndexLValueNode>(
                                    exprNode->left);
                            if (!areEqual && lvalue->lvalue->type->getType() == ARRAY) {
                                throw Util::CommanderException(
                                        "Array of type" + getErrorTypeString(lvalue->lvalue->type)
                                                + " cannot be set due to expression to the right of '=' being of type "
                                                + getErrorTypeString(rightTy),
                                        exprNode->right->position);
                            }
                            if (lvalue->lvalue->type->getType() == TUPLE) {
                                int64_t const index
                                        = std::static_pointer_cast<Parser::IntExprNode>(lvalue->index)->value;
                                TupleTyPtr const tupleTy = std::static_pointer_cast<TupleTy>(lvalue->lvalue->type);
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
                        throw Util::CommanderException(
                                "Inequality operation '" + Parser::binOpToString(exprNode->opType)
                                        + "' has incompatible types. Left and right types should be INT, FLOAT, or "
                                          "STRING. Left: "
                                        + getErrorTypeString(leftTy) + ", Right: " + getErrorTypeString(rightTy),
                                exprNode->position);
                    case Parser::AND:
                    case Parser::OR:
                        // bool/bool --> bool
                        if (areEqual && isBool) { return (exprNode->type = BOOL_TY); }
                        throw Util::CommanderException(
                                "Boolean operation '" + Parser::binOpToString(exprNode->opType)
                                        + "' has incompatible types. Both left and right types should be BOOL. Left: "
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
                    case Parser::ADD_SET:
                    case Parser::SUBTRACT_SET:
                    case Parser::MULTIPLY_SET:
                    case Parser::DIVIDE_SET:
                    case Parser::MODULO_SET:
                    case Parser::EXPONENTIATE_SET:
                        if (!isLvalue) {
                            throw Util::CommanderException(
                                    "Invalid set expression. Must have variable or index lvalue on left side of '"
                                            + Parser::binOpToString(exprNode->opType) + "', but found "
                                            + Parser::nodeTypeToString(exprNode->left->nodeType()) + " instead",
                                    exprNode->left->position);
                        }
                        if (exprNode->left->nodeType() == Parser::VAR_LVALUE) {
                            std::string const varName
                                    = std::static_pointer_cast<Parser::VarLValueNode>(exprNode->left)->variable;
                            if (!leftTy) {
                                throw Util::CommanderException("Variable '" + varName
                                                                       + "' has not been initialized yet",
                                                               exprNode->left->position);
                            }
                            if (_table->getVariable(varName)->constant) {
                                throw Util::CommanderException("Unable to update constant variable '" + varName + "'",
                                                               exprNode->left->position);
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
                        throw Util::CommanderException(
                                "Binary operation '" + Parser::binOpToString(exprNode->opType)
                                        + "' has incompatible types. Left and right types should be INT, FLOAT, or "
                                          "STRING (if doing ADD for string concatenation). Left: "
                                        + getErrorTypeString(leftTy) + ", Right: " + getErrorTypeString(rightTy),
                                exprNode->position);
                }
                // All cases are dealt with in switch statement, so it shouldn't ever get here
                throw Util::CommanderException("Unexpected error occurred with type checking binary expression",
                                               exprNode->position);
            }
            case Parser::CALL_EXPR: {
                Parser::CallExprNodePtr const exprNode = std::static_pointer_cast<Parser::CallExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                TyPtr const type = typeCheck(exprNode->func);
                if (type && type->getType() != Type::FUNCTION) {
                    throw Util::CommanderException("Tried to call something that wasn't a function, but rather of type"
                                                           + getErrorTypeString(type),
                                                   exprNode->func->position);
                }
                std::vector<TyPtr> functionTypes;
                const bool isVarLValue
                        = exprNode->func->nodeType() == Parser::LVALUE_EXPR
                       && std::static_pointer_cast<Parser::LValueExprNode>(exprNode->func)->expr->nodeType()
                                  == Parser::VAR_EXPR;
                const bool isTupleLValue
                        = exprNode->func->nodeType() == Parser::LVALUE_EXPR
                       && std::static_pointer_cast<Parser::LValueExprNode>(exprNode->func)->expr->nodeType()
                                  == Parser::INDEX_EXPR
                       && std::static_pointer_cast<Parser::IndexExprNode>(
                                  std::static_pointer_cast<Parser::LValueExprNode>(exprNode->func)->expr)
                                          ->expr->type->getType()
                                  == TUPLE;
                if (isVarLValue) {
                    std::string const varName
                            = std::static_pointer_cast<Parser::VarExprNode>(
                                      std::static_pointer_cast<Parser::LValueExprNode>(exprNode->func)->expr)
                                      ->variable;
                    VarInfoPtr const functionInfo = _table->getVariable(varName);
                    if (functionInfo->infoType() != FUNCTION_INFO) {
                        throw Util::CommanderException(
                                "The variable '" + varName
                                        + "' does not refer to a function, but instead something of type"
                                        + getErrorTypeString(functionInfo->types[0]),
                                exprNode->func->position);
                    }
                    functionTypes = functionInfo->types;
                } else if (type) {
                    functionTypes.push_back(type);
                }
                std::vector<TyPtr> argTypes;
                for (const Parser::ExprNodePtr& expr : exprNode->args->exprs) { argTypes.push_back(typeCheck(expr)); }
                FunctionTyPtr functionType = nullptr;
                for (const TyPtr& typ : functionTypes) {
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
                if (!functionType && !isTupleLValue) {
                    if (isVarLValue) {
                        std::string const varName
                                = std::static_pointer_cast<Parser::VarExprNode>(
                                          std::static_pointer_cast<Parser::LValueExprNode>(exprNode->func)->expr)
                                          ->variable;
                        std::stringstream functionSignatures;
                        for (const TyPtr& typ : _table->getVariable(varName)->types) {
                            functionSignatures << "\n" << getErrorTypeString(typ);
                        }
                        const TyPtr callType = std::make_shared<FunctionTy>(
                                argTypes, std::static_pointer_cast<FunctionTy>(type)->returnType);
                        throw Util::CommanderException(
                                "The parameters being used in call expression don't match any function signatures of '"
                                        + varName + "'. Calling signature:" + getErrorTypeString(callType)
                                        + "\nPossible function signatures:" + functionSignatures.str(),
                                exprNode->position);
                    }
                    const TyPtr callType = std::make_shared<FunctionTy>(
                            argTypes, std::static_pointer_cast<FunctionTy>(type)->returnType);
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
                return (exprNode->type = functionType->returnType);
            }
            case Parser::API_CALL_EXPR: {
                Parser::ApiCallExprNodePtr const exprNode = std::static_pointer_cast<Parser::ApiCallExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                exprNode->args->exprs.insert(exprNode->args->exprs.begin(), exprNode->expression);
                if (!_table->hasVariable(exprNode->func)) {
                    throw Util::CommanderException("Function variable named '" + exprNode->func
                                                           + "' is not initialized.",
                                                   exprNode->funcPosition);
                }
                TyPtr const type = getVarType(exprNode->func, FUNCTION_INFO, exprNode->funcPosition);
                if (type && type->getType() != Type::FUNCTION) {
                    throw Util::CommanderException("Tried to call '" + exprNode->func
                                                           + "' which isn't a function type. The type is instead"
                                                           + getErrorTypeString(type),
                                                   exprNode->funcPosition);
                }
                std::vector<TyPtr> functionTypes;
                VarInfoPtr const functionInfo = _table->getVariable(exprNode->func);
                if (functionInfo->infoType() != FUNCTION_INFO) {
                    throw Util::CommanderException(
                            "The variable '" + exprNode->func
                                    + "' does not refer to a function, but instead something of type"
                                    + getErrorTypeString(functionInfo->types[0]),
                            exprNode->funcPosition);
                }
                functionTypes = functionInfo->types;
                std::vector<TyPtr> argTypes;
                for (const Parser::ExprNodePtr& expr : exprNode->args->exprs) { argTypes.push_back(typeCheck(expr)); }
                FunctionTyPtr functionType = nullptr;
                for (const TyPtr& typ : functionTypes) {
                    FunctionTyPtr const functionTy = std::static_pointer_cast<FunctionTy>(typ);
                    size_t const size = functionTy->parameters.size();
                    if (size != argTypes.size()) { continue; }
                    bool areEqual = true;
                    for (int i = 0; i < size; i++) {
                        if (!areTypesEqual(functionTy->parameters[i], argTypes[i])) {
                            areEqual = false;
                            continue;
                        }
                    }
                    if (!areEqual) { continue; }
                    functionType = functionTy;
                    break;
                }
                if (!functionType) {
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
                if (exprNode->func == "append") {
                    ArrayTyPtr const arrayTy = std::static_pointer_cast<ArrayTy>(argTypes[0]);
                    if (!arrayTy->baseType) {
                        arrayTy->baseType = argTypes[1];
                    } else if (!areTypesEqual(arrayTy->baseType, argTypes[1])) {
                        throw Util::CommanderException("Cannot append type" + getErrorTypeString(argTypes[1])
                                                               + " to array of type" + getErrorTypeString(arrayTy),
                                                       exprNode->position);
                    }
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
                    throw Util::CommanderException(
                            "Return types for the lambda function don't match. Expected return type"
                                    + getErrorTypeString(returnType) + " but got"
                                    + getErrorTypeString(typeCheck(exprNode->body)) + " instead",
                            exprNode->position);
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
                if (promptType && promptType->getType() != Type::STRING) {
                    throw Util::CommanderException("Expected a STRING prompt for scan expression, but found type"
                                                           + getErrorTypeString(promptType) + " instead",
                                                   exprNode->prompt->position);
                }
                return (exprNode->type = STRING_TY);
            }
            case Parser::READ_EXPR: {
                Parser::ReadExprNodePtr const exprNode = std::static_pointer_cast<Parser::ReadExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                TyPtr const pathType = typeCheck(exprNode->filePath);
                if (pathType && pathType->getType() != Type::STRING) {
                    throw Util::CommanderException("Expected a STRING file path for read expression, but found type"
                                                           + getErrorTypeString(pathType) + " instead",
                                                   exprNode->filePath->position);
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
                if (conditionType && conditionType->getType() != Type::BOOL) {
                    throw Util::CommanderException(
                            "If statement condition does not evaluate to a BOOL type, but instead to type"
                                    + getErrorTypeString(conditionType),
                            stmtNode->condition->position);
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
                if (conditionType && conditionType->getType() != Type::BOOL) {
                    throw Util::CommanderException(
                            "For statement condition does not evaluate to a BOOL type, but instead to type"
                                    + getErrorTypeString(conditionType),
                            stmtNode->condition->position);
                }
                typeCheck(stmtNode->update);
                typeCheck(stmtNode->body);
                popScope();
                return nullptr;
            }
            case Parser::WHILE_STMT: {
                Parser::WhileStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::WhileStmtNode>(astNode);
                TyPtr const conditionType = typeCheck(stmtNode->condition);
                if (conditionType && conditionType->getType() != Type::BOOL) {
                    throw Util::CommanderException(
                            "While statement condition does not evaluate to a BOOL type, but instead to type"
                                    + getErrorTypeString(conditionType),
                            stmtNode->condition->position);
                }
                typeCheck(stmtNode->body);
                return nullptr;
            }
            case Parser::DO_WHILE_STMT: {
                Parser::DoWhileStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::DoWhileStmtNode>(astNode);
                typeCheck(stmtNode->body);
                TyPtr const conditionType = typeCheck(stmtNode->condition);
                if (conditionType && conditionType->getType() != Type::BOOL) {
                    throw Util::CommanderException(
                            "Do-while statement condition does not evaluate to a BOOL type, but instead to type"
                                    + getErrorTypeString(conditionType),
                            stmtNode->condition->position);
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
                assertNotExists(stmtNode->alias, stmtNode->position);
                _table->addVariable(stmtNode->alias, std::make_shared<AliasInfo>());
                return nullptr;
            }
            case Parser::IMPORT_STMT: {
                Parser::ImportStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::ImportStmtNode>(astNode);
                TyPtr const pathType = typeCheck(stmtNode->filePath);
                if (pathType && pathType->getType() != Type::STRING) {
                    throw Util::CommanderException("Expected a STRING file path for import statement, but found type"
                                                           + getErrorTypeString(pathType) + " instead",
                                                   stmtNode->filePath->position);
                }
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
                stmtNode->prgm = _parser.parse(tokens);
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
                if (dataType && dataType->getType() != Type::STRING) {
                    throw Util::CommanderException("Expected a STRING for file data in write statement, but found type"
                                                           + getErrorTypeString(dataType) + " instead",
                                                   stmtNode->fileData->position);
                }
                TyPtr const pathType = typeCheck(stmtNode->filePath);
                if (pathType && pathType->getType() != Type::STRING) {
                    throw Util::CommanderException("Expected a STRING file path for write statement, but found type"
                                                           + getErrorTypeString(pathType) + " instead",
                                                   stmtNode->filePath->position);
                }
                return nullptr;
            }
            case Parser::TYPE_STMT: {
                Parser::TypeStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::TypeStmtNode>(astNode);
                assertNotExists(stmtNode->alias, stmtNode->position);
                _table->addVariable(stmtNode->alias, std::make_shared<TypeInfo>(typeCheck(stmtNode->type)));
                return nullptr;
            }
            case Parser::FUNCTION_STMT: {
                Parser::FunctionStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::FunctionStmtNode>(
                        astNode);
                assertNotExists(stmtNode->name, stmtNode->position);
                pushScope();
                std::vector<TyPtr> bindings;
                for (const Parser::BindingNodePtr& binding : stmtNode->bindings->bindings) {
                    bindings.push_back(typeCheck(binding));
                }
                TyPtr returnType = typeCheck(stmtNode->returnType);
                if (!returnType) {
                    returnType = typeCheck(stmtNode->body);
                    if (!returnType) { returnType = VOID_TY; }
                    stmtNode->returnType = std::make_shared<Parser::FunctionTypeNode>(stmtNode->body->position,
                                                                                      nullptr);
                    stmtNode->returnType->type = returnType;
                } else if (!areTypesEqual(returnType, typeCheck(stmtNode->body))) {
                    throw Util::CommanderException(
                            "Return types for the function declaration don't match. Expected return type"
                                    + getErrorTypeString(returnType) + " but got"
                                    + getErrorTypeString(typeCheck(stmtNode->body)) + " instead",
                            stmtNode->position);
                }
                popScope();
                _table->addVariable(stmtNode->name, std::make_shared<FunctionInfo>(std::vector<TyPtr> {
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
                if (type && type->getType() != BOOL) {
                    throw Util::CommanderException(
                            "Assert statement condition does not evaluate to a BOOL type, but instead to type"
                                    + getErrorTypeString(type),
                            stmtNode->expr->position);
                }
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
                VarInfoPtr typeInfo = _table->getVariable(variableTypePtr->name);
                if (!typeInfo) {
                    throw Util::CommanderException("The variable '" + variableTypePtr->name
                                                           + "' has not been initialized yet",
                                                   variableTypePtr->position);
                }
                if (typeInfo->infoType() != TYPE_INFO) {
                    throw Util::CommanderException("The variable '" + variableTypePtr->name
                                                           + "' does not refer to a type, but instead something of type"
                                                           + getErrorTypeString(typeInfo->types[0]),
                                                   variableTypePtr->position);
                }
                return (variableTypePtr->type = typeInfo->types[0]);
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

    void TypeChecker::assertNotExists(const std::string& variable, const Lexer::FilePosition& position) {
        if (_table->hasVariable(variable)) {
            throw Util::CommanderException("Variable of name '" + variable + "' already exists in current scope.",
                                           position);
        }
    }

    void TypeChecker::pushScope() { _table = std::make_shared<VariableTable>(_table); }

    VariableTablePtr TypeChecker::popScope() {
        VariableTablePtr scope = std::make_shared<VariableTable>(_table);
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