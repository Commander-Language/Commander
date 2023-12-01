/**
 * TypeChecker.cpp contains the implementations of the TypeChecker class
 */

#include "TypeChecker.hpp"

//Constructor
TypeChecker::TypeChecker() {

}

//Destructor
TypeChecker::~TypeChecker() {

}

//Copy-Constructor
TypeChecker::TypeChecker(TypeChecker *otherTypeChecker) {
    std::map<std::string, std::string> data(otherTypeChecker->assignedTypes);
    assignedTypes = data;
}

std::string TypeChecker::getType(std::string variableID) {
    if(assignedTypes.find(variableID) == assignedTypes.end()) {
        return nullptr;
    }
    return assignedTypes[variableID];
}

void TypeChecker::setOrUpdateType(std::string variableID, std::string type) {
    assignedTypes[variableID] = type;
}

bool TypeChecker::verifyType(std::string variableID, std::string expected) {
    if(!hasVariable(variableID)) {
        return false; //return false if the variable doesnt exist
    }
    else if(assignedTypes[variableID] == expected) {
        return true;
    }
    return isVariantOfExpected(assignedTypes[variableID], expected);
}


bool TypeChecker::hasVariable(std::string variableID) {
    if(assignedTypes.find(variableID) == assignedTypes.end()) {
        return false;
    }
    return true;
}

bool TypeChecker::isVariantOfExpected(std::string variableType, std::string expectedType) {
    //TODO: refactor and accound for additional types (i.e. STRING and CHAR, etc.)
    bool variableStatus = (variableType == "INTEGER" || variableType == "FLOAT" || variableType == "DOUBLE");
    bool expectedStatus = (expectedType == "INTEGER" || expectedType == "FLOAT" || expectedType == "DOUBLE");

    if(variableStatus == expectedStatus) {
        return true;
    }
    return false;
}