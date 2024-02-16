/**
 * @file type_checker.cpp
 * @brief type_checker.cpp contains the implementation for the prototype methods in type_checker.hpp
 * @details For method prototypes, see type_checker.hpp
 */

#include "type_checker.hpp"

namespace TypeChecker {

    // Default-Constructor
    TypeChecker::TypeChecker() {
        _assignedTypes.insert(Function::functionTypes.begin(), Function::functionTypes.end());
    }

    // Copy-Constructor
    TypeChecker::TypeChecker(TypeChecker* otherTypeChecker) {
        std::unordered_map<std::string, TyPtr> const data(otherTypeChecker->_assignedTypes);
        _assignedTypes = data;
    }

    void TypeChecker::typeCheck(const Parser::ASTNodeList& astNodeList) {
        // Symbol table:
        // 4 different types of stuff stored in it
        // Variables: name --> Ty
        // Functions: name --> FunctionTy (Ty)
        // Aliases: ?
        // Custom Types: name --> Ty
        for (const Parser::ASTNodePtr& node : astNodeList) { typeCheck(node); }
    }

    TyPtr TypeChecker::typeCheck(const Parser::ASTNodePtr& astNode) {
        switch (astNode->nodeType()) {
            case Parser::BINDING: {
                Parser::BindingNodePtr const binding = std::static_pointer_cast<Parser::BindingNode>(astNode);
                if (binding->type) {
                    if (hasVariable(binding->variable)) {
                        // TODO: Improve error
                        throw Util::CommanderException("The variable '" + binding->variable
                                                       + "' already exists in scope.");
                    }
                    TyPtr bindingType = binding->type->type ? binding->type->type : typeCheck(binding->type);
                    setOrUpdateType(binding->variable, bindingType);
                    return bindingType;
                }
                return nullptr;
            }
            case Parser::BINDINGS: {
                Parser::BindingsNodePtr const bindings = std::static_pointer_cast<Parser::BindingsNode>(astNode);
                for (const Parser::BindingNodePtr& currentBinding : bindings->bindings) { typeCheck(currentBinding); }
                return nullptr;
            }
            case Parser::CMD_CMD: {
                Parser::CmdCmdNodePtr const command = std::static_pointer_cast<Parser::CmdCmdNode>(astNode);
                for (const Parser::ASTNodePtr& node : command->arguments) {
                    TyPtr const nodeType = typeCheck(node);
                    if (!nodeType || nodeType->getType() != Type::STRING) {
                        // TODO: Improve error
                        throw Util::CommanderException("Command has an argument that is not a string.");
                    }
                }
                std::vector<TyPtr> contentTypes;
                contentTypes.push_back(std::make_shared<StringTy>());
                contentTypes.push_back(std::make_shared<StringTy>());
                contentTypes.push_back(std::make_shared<IntTy>());
                return std::make_shared<TupleTy>(contentTypes);
            }
            case Parser::PIPE_CMD: {
                Parser::PipeCmdNodePtr const pipeCommand = std::static_pointer_cast<Parser::PipeCmdNode>(astNode);
                typeCheck(pipeCommand->leftCmd);
                typeCheck(pipeCommand->rightCmd);
                std::vector<TyPtr> contentTypes;
                contentTypes.push_back(std::make_shared<StringTy>());
                contentTypes.push_back(std::make_shared<StringTy>());
                contentTypes.push_back(std::make_shared<IntTy>());
                return std::make_shared<TupleTy>(contentTypes);
            }
            case Parser::ASYNC_CMD: {
                Parser::AsyncCmdNodePtr const asyncCommand = std::static_pointer_cast<Parser::AsyncCmdNode>(astNode);
                return typeCheck(asyncCommand);
            }
            case Parser::INT_EXPR: {
                Parser::IntExprNodePtr const exprNode = std::static_pointer_cast<Parser::IntExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                return (exprNode->type = std::make_shared<IntTy>());
            }
            case Parser::FLOAT_EXPR: {
                Parser::FloatExprNodePtr const exprNode = std::static_pointer_cast<Parser::FloatExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                return (exprNode->type = std::make_shared<FloatTy>());
            }
            case Parser::STRING_EXPR: {
                Parser::StringExprNodePtr const exprNode = std::static_pointer_cast<Parser::StringExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                typeCheck(exprNode->stringNode);
                return (exprNode->type = std::make_shared<StringTy>());
            }
            case Parser::BOOL_EXPR: {
                Parser::BoolExprNodePtr const exprNode = std::static_pointer_cast<Parser::BoolExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                return (exprNode->type = std::make_shared<BoolTy>());
            }
            case Parser::VAR_EXPR: {
                Parser::VarExprNodePtr const exprNode = std::static_pointer_cast<Parser::VarExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                TyPtr const varType = typeCheck(exprNode->variable);
                if (!varType) {
                    Parser::IdentVariableNodePtr const variablePtr
                            = std::static_pointer_cast<Parser::IdentVariableNode>(exprNode->variable);
                    // TODO: Improve error
                    throw Util::CommanderException("No variable by the name '" + variablePtr->varName
                                                   + "' has not been initialized yet (in the current scope).");
                }
                return (exprNode->type = varType);
            }
            case Parser::ARRAY_EXPR: {
                Parser::ArrayExprNodePtr const exprNode = std::static_pointer_cast<Parser::ArrayExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                TyPtr const type = exprNode->expressions.empty() ? nullptr
                                                                               : typeCheck(exprNode->expressions[0]);
                if (!exprNode->expressions.empty()) {
                    if (!type) {
                        // TODO: Improve error
                        throw Util::CommanderException("Array has an unknown type.");
                    }
                    for (const Parser::ExprNodePtr& exprNodePtr : exprNode->expressions) {
                        if (!areTypesEqual(type, typeCheck(exprNodePtr))) {
                            // TODO: Improve error
                            throw Util::CommanderException("Array contains multiple different types.");
                        }
                    }
                }
                return (exprNode->type = std::make_shared<ArrayTy>(type));
            }
            case Parser::ARRAY_INDEX_EXPR: {
                Parser::ArrayIndexExprNodePtr const exprNode = std::static_pointer_cast<Parser::ArrayIndexExprNode>(
                        astNode);
                if (exprNode->type) { return exprNode->type; }
                TyPtr const arrayType = typeCheck(exprNode->array);
                if (!arrayType || arrayType->getType() != Type::ARRAY) {
                    // TODO: Improve error
                    throw Util::CommanderException("Tried to index a type that isn't an array");
                }
                for (const Parser::ExprNodePtr& exprNodePtr : exprNode->indexExprs) {
                    TyPtr const indexType = typeCheck(exprNodePtr);
                    if (!indexType || indexType->getType() != Type::INT) {
                        // TODO: Improve error (not just position, but what type; same for the other errors)
                        throw Util::CommanderException("Tried to index an array with a different type than an int");
                    }
                }
                ArrayTyPtr const arrayTy = std::static_pointer_cast<ArrayTy>(arrayType);
                return (exprNode->type = arrayTy->baseType);
            }
            case Parser::TUPLE_EXPR: {
                Parser::TupleExprNodePtr const exprNode = std::static_pointer_cast<Parser::TupleExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                std::vector<TyPtr> expressionTypes;
                for (const Parser::ExprNodePtr& exprNodePtr : exprNode->expressions) {
                    TyPtr const exprType = typeCheck(exprNodePtr);
                    if (!exprType) {
                        // TODO: Improve error
                        throw Util::CommanderException("Unknown type for expression in tuple");
                    }
                    expressionTypes.push_back(exprType);
                }
                return (exprNode->type = std::make_shared<TupleTy>(expressionTypes));
            }
            case Parser::TUPLE_INDEX_EXPR: {
                Parser::TupleIndexExprNodePtr const exprNode = std::static_pointer_cast<Parser::TupleIndexExprNode>(
                        astNode);
                if (exprNode->type) { return exprNode->type; }
                TyPtr const tupleType = typeCheck(exprNode->tuple);
                if (!tupleType || tupleType->getType() != Type::TUPLE) {
                    // TODO: Improve error
                    throw Util::CommanderException("Tried to index a type that isn't an tuple");
                }
                TyPtr const indexType = typeCheck(exprNode->index);
                if (!indexType || indexType->getType() != Type::INT) {
                    // TODO: Improve error (not just position, but what type; same for the other errors)
                    throw Util::CommanderException("Tried to index a tuple with a different type than an int");
                }
                // Impossible to know the type of this, especially if the index expression is a variable, so just return
                // nullptr.
                return nullptr;
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
                TyPtr const expressionType = typeCheck(exprNode->expr);
                if (!expressionType) {
                    // TODO: Improve error
                    throw Util::CommanderException("Unknown type in unop expression.");
                }
                Type const type = expressionType->getType();
                if (exprNode->opType == Parser::NOT && type != Type::BOOL) {
                    // TODO: Improve error
                    throw Util::CommanderException("Expected boolean in not expression.");
                }
                if (exprNode->opType != Parser::NOT && type != Type::INT && type != Type::FLOAT) {
                    // TODO: Improve error
                    throw Util::CommanderException("Expected int or float in unop expression.");
                }
                return (exprNode->type = expressionType);
            }
            case Parser::BINOP_EXPR: {
                Parser::BinOpExprNodePtr const exprNode = std::static_pointer_cast<Parser::BinOpExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                bool const isVariable = exprNode->leftVariable != nullptr;
                TyPtr const leftTy = isVariable ? typeCheck(exprNode->leftVariable) : typeCheck(exprNode->leftExpr);
                TyPtr const rightTy = typeCheck(exprNode->rightExpr);
                // Right type must be known in all operations
                if (!rightTy) {
                    // TODO: Improve error
                    throw Util::CommanderException("Expression evaluates to unknown type");
                }
                Type const rightType = rightTy->getType();
                bool const isInt = rightType == Type::INT;
                bool const isFloat = rightType == Type::FLOAT;
                bool const isBool = rightType == Type::BOOL;
                bool const isString = rightType == Type::STRING;
                bool const areEqual = areTypesEqual(leftTy, rightTy);
                bool const areIntFloat = leftTy
                                      && ((leftTy->getType() == Type::INT && isFloat)
                                          || (leftTy->getType() == Type::FLOAT && isInt));
                switch (exprNode->opType) {
                    case Parser::SET: {
                        if (!isVariable) {
                            // TODO: Improve error
                            throw Util::CommanderException(
                                    "Invalid set expression; must have variable on left side of '='");
                        }
                        // TODO: Make variables a binding
                        Parser::IdentVariableNodePtr const varPtr = std::static_pointer_cast<Parser::IdentVariableNode>(
                                exprNode->leftVariable);
                        setOrUpdateType(varPtr->varName, rightTy);
                        return (exprNode->type = rightTy);
                    }
                    case Parser::LESSER:
                    case Parser::GREATER:
                    case Parser::LESSER_EQUAL:
                    case Parser::GREATER_EQUAL:
                        // Int/float | float/float | Int/int | string/string --> bool
                        if (areIntFloat || (areEqual && (isInt || isFloat || isString))) {
                            return (exprNode->type = std::make_shared<BoolTy>());
                        }
                        // TODO: Improve error
                        throw Util::CommanderException("Inequality operation has incompatible types.");
                    case Parser::AND:
                    case Parser::OR:
                        // bool/bool --> bool
                        if (areEqual && isBool) { return (exprNode->type = std::make_shared<BoolTy>()); }
                        // TODO: Improve error
                        throw Util::CommanderException("Boolean operation has incompatible types.");
                    case Parser::EQUAL:
                    case Parser::NOT_EQUAL:
                        // any/any --> bool
                        if (areIntFloat || areEqual) { return (exprNode->type = std::make_shared<BoolTy>()); }
                        // TODO: Improve error
                        throw Util::CommanderException("Equality operation has incompatible types.");
                    case Parser::ADD:
                    case Parser::ADD_SET:
                    case Parser::MODULO:
                    case Parser::DIVIDE:
                    case Parser::MULTIPLY:
                    case Parser::SUBTRACT:
                    case Parser::SUBTRACT_SET:
                    case Parser::MULTIPLY_SET:
                    case Parser::DIVIDE_SET:
                    case Parser::MODULO_SET:
                    case Parser::EXPONENTIATE_SET:
                    case Parser::EXPONENTIATE:
                        // Int/float | float/float --> float
                        // Int/int --> int
                        // string/string --> string (in the case of ADD and ADD_EQUAL)
                        if (areIntFloat || (areEqual && isFloat)) {
                            return (exprNode->type = std::make_shared<FloatTy>());
                        }
                        if (areEqual && isInt) { return (exprNode->type = std::make_shared<IntTy>()); }
                        if (areEqual && isString
                            && (exprNode->opType == Parser::ADD || exprNode->opType == Parser::ADD_SET)) {
                            return (exprNode->type = std::make_shared<StringTy>());
                        }
                        // TODO: Improve error
                        throw Util::CommanderException("Binary operation has incompatible types.");
                }
                return nullptr;
            }
            case Parser::CALL_EXPR: {
                Parser::CallExprNodePtr const exprNode = std::static_pointer_cast<Parser::CallExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                TyPtr const functionType = typeCheck(exprNode->func);
                if (!functionType || functionType->getType() != Type::FUNCTION) {
                    // TODO: Improve Error
                    throw Util::CommanderException("Tried to call something that wasn't a function.");
                }
                FunctionTyPtr const functionTy = std::static_pointer_cast<FunctionTy>(functionType);
                size_t const size = functionTy->parameters.size();
                if (size != exprNode->args.size()) {
                    // TODO: Improve Error
                    throw Util::CommanderException(
                            "Function being called doesn't match number of parameters in call expression.");
                }
                for (int i = 0; i < size; i++) {
                    if (!areTypesEqual(functionTy->parameters[i], typeCheck(exprNode->args[i]))) {
                        // TODO: Improve Error
                        throw Util::CommanderException(
                                "Parameter expression types in call expression don't match function signature.");
                    }
                }
                return (exprNode->type = functionTy->returnType);
            }
            case Parser::LAMBDA_EXPR: {
                Parser::LambdaExprNodePtr const exprNode = std::static_pointer_cast<Parser::LambdaExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                std::vector<TyPtr> bindings;
                for (const Parser::BindingNodePtr& binding : exprNode->bindings) {
                    bindings.push_back(typeCheck(binding));
                }
                // TODO: See if possible to check return type and return stmt match
                TyPtr const returnType = typeCheck(exprNode->body);
                return (exprNode->type = std::make_shared<FunctionTy>(bindings, returnType));
            }
            case Parser::CMD_EXPR: {
                Parser::CmdExprNodePtr const exprNode = std::static_pointer_cast<Parser::CmdExprNode>(astNode);
                if (exprNode->type) { return exprNode->type; }
                typeCheck(exprNode->cmd);
                std::vector<TyPtr> contentTypes;
                contentTypes.push_back(std::make_shared<StringTy>());
                contentTypes.push_back(std::make_shared<StringTy>());
                contentTypes.push_back(std::make_shared<IntTy>());
                return (exprNode->type = std::make_shared<TupleTy>(contentTypes));
            }
            case Parser::EXPRS: {
                Parser::ExprsNodePtr const exprsPtr = std::static_pointer_cast<Parser::ExprsNode>(astNode);
                for (const Parser::ExprNodePtr& exprPtr : exprsPtr->exprs) { typeCheck(exprPtr); }
                return nullptr;
            }
            case Parser::PRGM: {
                Parser::PrgmNodePtr const program = std::static_pointer_cast<Parser::PrgmNode>(astNode);
                for (const Parser::StmtNodePtr& currentStatement : program->stmts) { typeCheck(currentStatement); }
                return nullptr;
            }
            case Parser::IF_STMT: {
                Parser::IfStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::IfStmtNode>(astNode);
                for (const Parser::ExprNodePtr& condition : stmtNode->conditions) {
                    // TODO: Put the following lines of code into a helper method since it is repeated
                    TyPtr const conditionType = typeCheck(condition);
                    if (!conditionType || conditionType->getType() != Type::BOOL) {
                        // TODO: Improve error
                        throw Util::CommanderException("The condition does not evaluate to a bool type.");
                    }
                }
                for (const Parser::StmtNodePtr& currentStatement : stmtNode->trueStmts) { typeCheck(currentStatement); }
                typeCheck(stmtNode->falseStmt);
                return nullptr;
            }
            case Parser::FOR_STMT: {
                Parser::ForStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::ForStmtNode>(astNode);
                typeCheck(stmtNode->initial);
                TyPtr const conditionType = typeCheck(stmtNode->condition);
                if (!conditionType || conditionType->getType() != Type::BOOL) {
                    // TODO: Improve error
                    throw Util::CommanderException("The condition does not evaluate to a bool type.");
                }
                typeCheck(stmtNode->update);
                typeCheck(stmtNode->body);
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
            case Parser::FUNCTION_STMT: {
                Parser::FunctionStmtNodePtr const functionNode = std::static_pointer_cast<Parser::FunctionStmtNode>(astNode);
                std::vector<TyPtr> bindings;
                for (const Parser::BindingNodePtr& binding : functionNode->bindings) {
                    bindings.push_back(typeCheck(binding));
                }
                // TODO: See if possible to check return type and return stmt match
                TyPtr const returnType = typeCheck(functionNode->body);
                return std::make_shared<FunctionTy>(bindings, returnType);
            }
            case Parser::RETURN_STMT: {
                // TODO: Typecheck that it matches the function's return type that it is in? (not sure how to or if
                // possible)
                Parser::ReturnStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::ReturnStmtNode>(astNode);
                return typeCheck(stmtNode->retExpr);
            }
            case Parser::SCOPE_STMT: {
                Parser::ScopeStmtNodePtr const stmtNode = std::static_pointer_cast<Parser::ScopeStmtNode>(astNode);
                // TODO: Figure out scopes (not only for this, but for functions, loops, etc.; types for variables only
                // exist within scopes)
                for (const Parser::StmtNodePtr& stmtPtr : stmtNode->stmts) { typeCheck(stmtPtr); }
                return nullptr;
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
                // TODO: Implement
                return nullptr;
            }
            case Parser::STMTS: {
                Parser::StmtsNodePtr const stmtsPtr = std::static_pointer_cast<Parser::StmtsNode>(astNode);
                for (const Parser::StmtNodePtr& stmtPtr : stmtsPtr->stmts) { typeCheck(stmtPtr); }
                return nullptr;
            }
            case Parser::STRING: {
                Parser::StringNodePtr const stringPtr = std::static_pointer_cast<Parser::StringNode>(astNode);
                for (const Parser::ExprNodePtr& exprPtr : stringPtr->expressions) {
                    TyPtr const exprType = typeCheck(exprPtr);
                    if (!exprPtr || exprType->getType() != Type::STRING) {
                        // TODO: Improve error
                        throw Util::CommanderException("String has an expression that is not a string.");
                    }
                }
                return std::make_shared<StringTy>();
            }
            case Parser::INT_TYPE: {
                Parser::IntTypeNodePtr const intPtr = std::static_pointer_cast<Parser::IntTypeNode>(astNode);
                if (intPtr->type) { return intPtr->type; }
                return (intPtr->type = std::make_shared<IntTy>());
            }
            case Parser::FLOAT_TYPE: {
                Parser::FloatTypeNodePtr const floatPtr = std::static_pointer_cast<Parser::FloatTypeNode>(astNode);
                if (floatPtr->type) { return floatPtr->type; }
                return (floatPtr->type = std::make_shared<FloatTy>());
            }
            case Parser::BOOL_TYPE: {
                Parser::BoolTypeNodePtr const boolPtr = std::static_pointer_cast<Parser::BoolTypeNode>(astNode);
                if (boolPtr->type) { return boolPtr->type; }
                return (boolPtr->type = std::make_shared<BoolTy>());
            }
            case Parser::STRING_TYPE: {
                Parser::StringTypeNodePtr const stringTypePtr = std::static_pointer_cast<Parser::StringTypeNode>(
                        astNode);
                if (stringTypePtr->type) { return stringTypePtr->type; }
                return (stringTypePtr->type = std::make_shared<StringTy>());
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
                for (const Parser::TypeNodePtr& typeNodePtr : tupleTypePtr->subtypes) {
                    types.push_back(typeCheck(typeNodePtr));
                }
                return (tupleTypePtr->type = std::make_shared<TupleTy>(types));
            }
            case Parser::FUNCTION_TYPE: {
                Parser::FunctionTypeNodePtr const functionTypePtr = std::static_pointer_cast<Parser::FunctionTypeNode>(
                        astNode);
                if (functionTypePtr->type) { return functionTypePtr->type; }
                std::vector<TyPtr> types;
                for (const Parser::TypeNodePtr& typeNodePtr : functionTypePtr->params) {
                    types.push_back(typeCheck(typeNodePtr));
                }
                return (functionTypePtr->type = std::make_shared<TupleTy>(types));
            }
            case Parser::VARIABLE: {
                Parser::IdentVariableNodePtr const variablePtr = std::static_pointer_cast<Parser::IdentVariableNode>(
                        astNode);
                return getType(variablePtr->varName);
            }
            default:
                throw Util::CommanderException(
                        "This should not have happened. Abstract ExprNode, TypeNode, or StmtNode was created...");
        }
    }

    TyPtr& TypeChecker::getType(const std::string& variableID) { return _assignedTypes[variableID]; }

    void TypeChecker::setOrUpdateType(const std::string& variableID, const TyPtr& type) {
        _assignedTypes[variableID] = type;
    }

    bool TypeChecker::verifyType(const std::string& variableID, const TyPtr& expected) {
        if (!hasVariable(variableID)) return false;  // return false if the variable doesnt exist
        if (areTypesEqual(_assignedTypes[variableID], expected))
            return true;  // return true if the variable is equal to the expected value

        // if neither check was successful, check if expected is compatible with the variableID's type
        return _isVariantOfExpected(_assignedTypes[variableID], expected);
    }


    bool TypeChecker::hasVariable(const std::string& variableID) { return getType(variableID) != nullptr; }

    bool TypeChecker::_isVariantOfExpected(const TyPtr& variableType, const TyPtr& expectedType) {
        // TODO: refactor and account for additional types (i.e. STRING and CHAR, etc.)
        bool const variableStatus = (variableType->getType() == Type::INT || variableType->getType() == Type::FLOAT);
        bool const expectedStatus = (expectedType->getType() == Type::INT || expectedType->getType() == Type::FLOAT);

        return variableStatus == expectedStatus;
    }
}  //  namespace TypeChecker