/**
 * @file type_checker.cpp
 * @brief type_checker.cpp contains the implementation for the prototype methods in type_checker.hpp
 * @details For method prototypes, see type_checker.hpp
 */

#include "type_checker.hpp"
#include "source/symbol_table/symbol_table_organizer.hpp"

// Constructor
TypeChecker::TypeChecker() = default;

// Destructor
TypeChecker::~TypeChecker() = default;

// Copy-Constructor
TypeChecker::TypeChecker(TypeChecker* otherTypeChecker) {
    std::unordered_map<std::string, std::string> data(otherTypeChecker->assignedTypes);
    _assignedTypes = data;
}

Parser::ASTNodeList TypeChecker::typeCheck(const Parser::ASTNodeList& astNodeList) {
    SymbolTableOrganizer symbolTable;
    symbolTable.pushSymbolTable();
    for (const std::shared_ptr<Parser::ASTNode>& node : astNodeList) {
        switch (node->nodeType()) {
            case Parser::BINDING:

                break;
            case Parser::BINDINGS:

                break;
            case Parser::CMD:

                break;
            case Parser::EXPR:

                break;
            case Parser::EXPRS:

                break;
            case Parser::PRGM:

                break;
            case Parser::STMT:
                //TODO: determine what kind of statement
                symbolTable.pushSymbolTable();
                break;
            case Parser::STMTS:

                break;
            case Parser::STRING:

                break;
            case Parser::TYPE:

                break;
            case Parser::VARIABLE:

                break;
        }
    }
}

std::string TypeChecker::getType(std::string variableID) { return _assignedTypes[variableID]; }

void TypeChecker::setOrUpdateType(std::string variableID, std::string type) { _assignedTypes[variableID] = type; }

bool TypeChecker::verifyType(std::string variableID, std::string expected) {
    if (!hasVariable(variableID)) return false;  // return false if the variable doesnt exist

    if (_assignedTypes[variableID] == expected)
        return true;  // return true if the variable is equal to the expected value

    // if neither check was successful, check if expected is compatible with the variableID's type
    return isVariantOfExpected(_assignedTypes[variableID], expected);
}


bool TypeChecker::hasVariable(std::string variableID) { return _assignedTypes.find(variableID) != _assignedTypes.end(); }

bool TypeChecker::isVariantOfExpected(std::string variableType, std::string expectedType) {
    // TODO: refactor and account for additional types (i.e. STRING and CHAR, etc.)
    bool variableStatus = (variableType == "INTEGER" || variableType == "FLOAT" || variableType == "DOUBLE");
    bool expectedStatus = (expectedType == "INTEGER" || expectedType == "FLOAT" || expectedType == "DOUBLE");

    return variableStatus == expectedStatus;
}