/**
 * @file type_checker.cpp
 * @brief type_checker.cpp contains the implementation for the prototype methods in type_checker.hpp
 * @details For method prototypes, see type_checker.hpp
 */

#include "type_checker.hpp"

//Constructor
TypeChecker::TypeChecker() = default;

//Destructor
TypeChecker::~TypeChecker() = default;

//Copy-Constructor
TypeChecker::TypeChecker(TypeChecker *otherTypeChecker) {
    std::unordered_map<std::string, std::string> data(otherTypeChecker->assignedTypes);
    assignedTypes = data;
}

std::string TypeChecker::getType(std::string variableID) {
    return assignedTypes[variableID];
}

void TypeChecker::setOrUpdateType(std::string variableID, std::string type) {
    assignedTypes[variableID] = type;
}

bool TypeChecker::verifyType(std::string variableID, std::string expected) {
    if(!hasVariable(variableID)) return false; //return false if the variable doesnt exist

    if(assignedTypes[variableID] == expected) return true; //return true if the variable is equal to the expected value

    //if neither check was successful, check if expected is compatible with the variableID's type
    return isVariantOfExpected(assignedTypes[variableID], expected);
}


bool TypeChecker::hasVariable(std::string variableID) {
    return assignedTypes.find(variableID) != assignedTypes.end();
}

bool TypeChecker::isVariantOfExpected(std::string variableType, std::string expectedType) {
    //TODO: refactor and accound for additional types (i.e. STRING and CHAR, etc.)
    bool variableStatus = (variableType == "INTEGER" || variableType == "FLOAT" || variableType == "DOUBLE");
    bool expectedStatus = (expectedType == "INTEGER" || expectedType == "FLOAT" || expectedType == "DOUBLE");

    return variableStatus == expectedStatus;
}