/**
 * @file type_checker.cpp
 * @brief type_checker.cpp contains the implementation for the prototype methods in type_checker.hpp
 * @details For method prototypes, see type_checker.hpp
 */

#include "type_checker.hpp"

namespace TypeChecker {
    // Constructor
    TypeChecker::TypeChecker() = default;

    // Destructor
    TypeChecker::~TypeChecker() = default;

    // Copy-Constructor
    TypeChecker::TypeChecker(TypeChecker* otherTypeChecker) {
        std::unordered_map<std::string, TyPtr> data(otherTypeChecker->_assignedTypes);
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
                Parser::BindingNodePtr binding = std::static_pointer_cast<Parser::BindingNode>(astNode);
                if (binding->type) {
                    setOrUpdateType(binding->variable,
                                    binding->type->type ? binding->type->type : typeCheck(binding->type));
                }
                return nullptr;
            }
            case Parser::BINDINGS: {
                Parser::BindingsNodePtr bindings = std::static_pointer_cast<Parser::BindingsNode>(astNode);
                for (const Parser::BindingNodePtr& currentBinding : bindings->bindings) { typeCheck(currentBinding); }
                return nullptr;
            }
            case Parser::CMD: {
                Parser::CmdCmdNodePtr command = std::static_pointer_cast<Parser::CmdCmdNode>(astNode);
                for (const Parser::ASTNodePtr& node : command->arguments) {
                    if (typeCheck(node)->getType() != Type::STRING) {
                        // TODO: Improve error
                        throw util::CommanderException("Command has an argument that is not a string.");
                    }
                }
                return nullptr;
            }
            case Parser::PIPE_CMD: {
                Parser::PipeCmdNodePtr pipeCommand = std::static_pointer_cast<Parser::PipeCmdNode>(astNode);
                typeCheck(pipeCommand->leftCmd);
                typeCheck(pipeCommand->rightCmd);
                return nullptr;
            }
            case Parser::ASYNC_CMD: {
                Parser::AsyncCmdNodePtr asyncCommand = std::static_pointer_cast<Parser::AsyncCmdNode>(astNode);
                typeCheck(asyncCommand);
                return nullptr;
            }
            case Parser::EXPR: {
                // MULTIPLE!!!
                //  TODO: Type check
                break;
            }
            case Parser::EXPRS: {
                Parser::ExprsNodePtr exprsPtr = std::static_pointer_cast<Parser::ExprsNode>(astNode);
                for (const Parser::ExprNodePtr& exprPtr : exprsPtr->exprs) { typeCheck(exprPtr); }
                return nullptr;
            }
            case Parser::PRGM: {
                Parser::PrgmNodePtr program = std::static_pointer_cast<Parser::PrgmNode>(astNode);
                for (const Parser::StmtNodePtr& currentStatement : program->stmts) { typeCheck(currentStatement); }
                return nullptr;
            }
            case Parser::STMT: {
                // MUTLIPLE!!!
                // TODO: need to switch for each derivative of switch statements
                // ForStmtNode, IfStmtNode, etc.
                break;
            }
            case Parser::STMTS: {
                Parser::StmtsNodePtr stmtsPtr = std::static_pointer_cast<Parser::StmtsNode>(astNode);
                for (const Parser::StmtNodePtr& stmtPtr : stmtsPtr->stmts) { typeCheck(stmtPtr); }
                return nullptr;
            }
            case Parser::STRING: {
                Parser::StringNodePtr stringPtr = std::static_pointer_cast<Parser::StringNode>(astNode);
                for (const Parser::ExprNodePtr& exprPtr : stringPtr->expressions) {
                    if (typeCheck(exprPtr)->getType() != Type::STRING) {
                        // TODO: Improve error
                        throw util::CommanderException("String has an expression that is not a string.");
                    }
                }
                // Note: No need to return string ty here, since string nodes are contained in string expressions, which do get typed
                return nullptr;
            }
            case Parser::INT_TYPE: {
                Parser::IntTypeNodePtr intPtr = std::static_pointer_cast<Parser::IntTypeNode>(astNode);
                if (intPtr->type) { return intPtr->type; }
                return (intPtr->type = std::make_shared<IntTy>());
            }
            case Parser::FLOAT_TYPE: {
                Parser::FloatTypeNodePtr floatPtr = std::static_pointer_cast<Parser::FloatTypeNode>(astNode);
                if (floatPtr->type) { return floatPtr->type; }
                return (floatPtr->type = std::make_shared<FloatTy>());
            }
            case Parser::BOOL_TYPE: {
                Parser::BoolTypeNodePtr boolPtr = std::static_pointer_cast<Parser::BoolTypeNode>(astNode);
                if (boolPtr->type) { return boolPtr->type; }
                return (boolPtr->type = std::make_shared<BoolTy>());
            }
            case Parser::STRING_TYPE: {
                Parser::StringTypeNodePtr stringTypePtr = std::static_pointer_cast<Parser::StringTypeNode>(astNode);
                if (stringTypePtr->type) { return stringTypePtr->type; }
                return (stringTypePtr->type = std::make_shared<StringTy>());
            }
            case Parser::ARRAY_TYPE: {
                Parser::ArrayTypeNodePtr arrayTypePtr = std::static_pointer_cast<Parser::ArrayTypeNode>(astNode);
                if (arrayTypePtr->type) { return arrayTypePtr->type; }
                return (arrayTypePtr->type = std::make_shared<ArrayTy>(typeCheck(arrayTypePtr->subtype)));
            }
            case Parser::TUPLE_TYPE: {
                Parser::TupleTypeNodePtr tupleTypePtr = std::static_pointer_cast<Parser::TupleTypeNode>(astNode);
                if (tupleTypePtr->type) { return tupleTypePtr->type; }
                std::vector<std::shared_ptr<Ty>> types;
                for (const Parser::TypeNodePtr& typeNodePtr : tupleTypePtr->subtypes) {
                    types.push_back(typeCheck(typeNodePtr));
                }
                return (tupleTypePtr->type = std::make_shared<TupleTy>(types));
            }
            case Parser::FUNCTION_TYPE: {
                Parser::FunctionTypeNodePtr functionTypePtr = std::static_pointer_cast<Parser::FunctionTypeNode>(astNode);
                if (functionTypePtr->type) { return functionTypePtr->type; }
                std::vector<std::shared_ptr<Ty>> types;
                for (const Parser::TypeNodePtr& typeNodePtr : functionTypePtr->params) {
                    types.push_back(typeCheck(typeNodePtr));
                }
                return (functionTypePtr->type = std::make_shared<TupleTy>(types));
            }
            case Parser::VARIABLE: {
                Parser::IdentVariableNodePtr variablePtr = std::static_pointer_cast<Parser::IdentVariableNode>(astNode);
                return getType(variablePtr->varName);
            }
        }
    }

    TyPtr& TypeChecker::getType(const std::string& variableID) { return _assignedTypes[variableID]; }

    void TypeChecker::setOrUpdateType(const std::string& variableID, const TyPtr& type) {
        _assignedTypes[variableID] = type;
    }

    bool TypeChecker::verifyType(const std::string& variableID, const TyPtr& expected) {
        if (!hasVariable(variableID)) return false;  // return false if the variable doesnt exist
        if (_assignedTypes[variableID] == expected)
            return true;  // return true if the variable is equal to the expected value

        // if neither check was successful, check if expected is compatible with the variableID's type
        return _isVariantOfExpected(_assignedTypes[variableID], expected);
    }


    bool TypeChecker::hasVariable(const std::string& variableID) {
        return getType(variableID) != nullptr;
    }

    bool TypeChecker::_isVariantOfExpected(const TyPtr& variableType, const TyPtr& expectedType) {
        // TODO: refactor and account for additional types (i.e. STRING and CHAR, etc.)
        bool variableStatus = (variableType->getType() == Type::INT || variableType->getType() == Type::FLOAT);
        bool expectedStatus = (expectedType->getType() == Type::INT || expectedType->getType() == Type::FLOAT);

        return variableStatus == expectedStatus;
    }
} //  namespace TypeChecker