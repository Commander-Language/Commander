/**
 * Scope.cpp serves as the implementation for the Scope class.
 * Scope contains a symbol table - a map which uses a string ID to return an object
 */

#include "Scope.hpp"

//Default Constructor
Scope::Scope() {
    //parentScope = nullptr;
}

//Constructor for specifying a parent
Scope::Scope(Scope* parent) {
    parentScope = parent;
}

//Destructor
Scope::~Scope() {

}

//Copy Constructor
Scope::Scope(Scope &otherScope) {
    parentScope = otherScope.getParentScopePointer();
    std::map<std::string, int> copyData(otherScope.variableData);
    variableData = copyData;
}

/**
 * addOrUpdateVariable will attempt to add the relevant data to the scope.
 * @param variableID - A String ID which the variable will be referenced by (e.g. "cat")
 * @param data - An object which will be stored as data (e.g. 14, "dog", std::vector<int>, etc.)
 */
void Scope::addOrUpdateVariable(std::string variableID, int data) {
    variableData.insert_or_assign(variableID, data);
}

/**
 * hasVariable returns a boolean value according to whether the specified variable exists in this scope.
 * For scanning all scopes for a variable, use getVariable()
 * @param variableID - A string ID which the variable will be referenced by
 * @return - TRUE if the variable exists in the current scope, otherwise FALSE is returned
 */
bool Scope::hasVariable(std::string variableID) {
    if(hasKey(variableID)) {
        return true;
    }
    return false;
}

/**
 * getVariable returns a pointer to the desired variable in the scope. If the variable does not exist in this scope,
 * the parent scope will be scanned for the variable
 * @param variableID - A string ID which the variable will be referenced by
 * @return - A pointer to the specified data if it exists, otherwise a nullptr will return
 */
int* Scope::getVariable(std::string variableID) {
    if(!hasKey(variableID)) {
        if(parentScope != nullptr) {
            return parentScope->getVariable(variableID);
        }
        return nullptr;
    }
    return &variableData[variableID];
}

/**
 * getParenScopePointer() returns a pointer to the parent of this scope
 * @return - A pointer to the parent of this scope
 */
Scope* Scope::getParentScopePointer() {
    return parentScope;
}

/**
 * isGlobal() will return a value according to whether the Scope has a parent
 * @return - FALSE if the scope has a parent, TRUE if the scope returns a nullptr
 */
bool Scope::isGlobal() {
    return parentScope == NULL;
}

/**
 * hasKey() returns a boolean value according to whether variableData has an entry of the specified name
 * @param key - A string value representing a variable ID
 * @return - TRUE if the variable exists in variableData, otherwise FALSE is returned
 */
bool Scope::hasKey(std::string key) {
    if(variableData.find(key) == variableData.end()) { //.end() returns one index out-of-bounds from the data's contents (i.e. the equivalent of variableData.size())
        return false;
    }
    return true;
}