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
            case Parser::BINDING:
                Parser::BindingNodePtr binding = std::static_pointer_cast<Parser::BindingNode>(astNode);
                TyPtr type = nullptr;
                if (binding->type) {
                    type = typeCheck(binding->type);
                    setOrUpdateType(binding->variable, type);
                }
                return type;
                break;
            case Parser::BINDINGS:
                Parser::BindingsNodePtr bindings = std::static_pointer_cast<Parser::BindingsNode>(astNode);
                //std::vector<TyPtr> bindingTypes;
                for(const Parser::BindingNodePtr &currentBinding : bindings->bindings) {
                    typeCheck(currentBinding);
                }
                return nullptr; //TODO: modify?
                break;
            case Parser::CMD:
                Parser::CmdCmdNodePtr command = std::static_pointer_cast<Parser::CmdCmdNode>(astNode);
                break;
            case Parser::PIPE_CMD:
                Parser::PipeCmdNodePtr pipeCommand = std::static_pointer_cast<Parser::PipeCmdNode>(astNode);
                break;
            case Parser::ASYNC_CMD:
                Parser::AsyncCmdNodePtr asyncCommand = std::static_pointer_cast<Parser::AsyncCmdNode>(astNode);
                break;
            case Parser::EXPR:
                // TODO: Type check
                break;
            case Parser::EXPRS:
                // TODO: Type check
                break;
            case Parser::PRGM:
                Parser::PrgmNodePtr program = std::static_pointer_cast<Parser::PrgmNode>(astNode);
                for(const Parser::StmtNodePtr &currentStatement : program->stmts) {
                    typeCheck(currentStatement);
                }
                return nullptr;
                break;
            case Parser::STMT:
                //TODO: need to switch for each derivative of switch statements
                    //ForStmtNode, IfStmtNode, etc.
                break;
            case Parser::STMTS:
                // TODO: Type check
                break;
            case Parser::STRING:
                // TODO: Type check
                break;
            case Parser::TYPE:
                // TODO: Add to symbol table
                break;
            case Parser::VARIABLE:
                // TODO: Add to symbol table (after type checking)
                break;
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
        return _assignedTypes.find(variableID) != _assignedTypes.end();
    }

    bool TypeChecker::_isVariantOfExpected(const TyPtr& variableType, const TyPtr& expectedType) {
        // TODO: refactor and account for additional types (i.e. STRING and CHAR, etc.)
        bool variableStatus = (variableType->getType() == Type::INT || variableType->getType() == Type::FLOAT);
        bool expectedStatus = (expectedType->getType() == Type::INT || expectedType->getType() == Type::FLOAT);

        return variableStatus == expectedStatus;
    }
} //  namespace TypeChecker