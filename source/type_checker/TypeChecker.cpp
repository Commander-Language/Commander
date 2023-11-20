/**
 *
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

/**
 * setOrUpdateType() will attempt to assign a new variable to the current TypeChecker or update the current variable
 * @param variableID - The desired variableID to update or add
 * @param type - A string Type to associate the variable with
 */
void TypeChecker::setOrUpdateType(std::string variableID, std::string type) {
    assignedTypes[variableID] = type;
}

/**
 * getType() will attempt to return the variable type if present
 * @param variableID - The desired variableID to obtain
 * @return - A string type if the variable exists, otherwise nullptr is returned
 */
std::string TypeChecker::getType(std::string variableID) {
    if(assignedTypes.find(variableID) == assignedTypes.end()) {
        return nullptr;
    }
    return assignedTypes[variableID];
}

/**
 * verifyType() will return a boolean if the variableID type is compatable with the expected type. This occurs in the following situations:
 * - the expected type and variable type are equal
 * - the expected type and variable type can be converted to one another (e.g. Int to Float)
 * @param variableID - The variable one wishes to verify
 * @param expected - The expected outcome for a variable
 * @return - TRUE if a compatible type exists, otherwise FALSE is returned
 */
bool TypeChecker::verifyType(std::string variableID, std::string expected) {
    if(!hasVariable(variableID)) {
        return false; //return false if the variable doesnt exist
    }
    else if(assignedTypes[variableID] == expected) {
        return true;
    }
    return true; //TODO: determine how similar types should be handled
}

/**
 * hasVariable() returns a boolean value according to whether the specified variable is present in the TypeChecker
 * @param variableID - The desired variable to locate
 * @return TRUE if the variable exists, otherwise FALSE is returned
 */
bool TypeChecker::hasVariable(std::string variableID) {
    if(assignedTypes.find(variableID) == assignedTypes.end()) {
        return false;
    }
    return true;
}