/**
 * @file scope.cpp
 * @brief scope.cpp contains implementations for the scope.hpp class
 * @details For method prototypes, see scope.hpp
 */

#include "scope.hpp"

//Default Constructor
Scope::Scope() {
    parentScope = nullptr;
}

//Constructor for specifying a parent
Scope::Scope(Scope* parent) {
    parentScope = parent;
}

//Destructor
Scope::~Scope() = default;

//Copy Constructor
Scope::Scope(Scope &otherScope) {
    parentScope = otherScope.getParentScopePointer();
    std::map<std::string, int> copyData(otherScope.variableData);
    variableData = copyData;
}

void Scope::addOrUpdateVariable(std::string variableID, int data) {
    variableData.insert_or_assign(variableID, data);
}

bool Scope::hasLocalVariable(std::string variableID) {
    return hasKey(variableID);
}

bool Scope::hasGlobalVariable(std::string variableID) {
    return hasKey(variableID) || (parentScope != nullptr && parentScope->hasGlobalVariable(variableID));
}

int* Scope::getVariable(std::string variableID) {
    if(!hasKey(variableID)) {
        if(parentScope != nullptr) {
            return parentScope->getVariable(variableID);
        }
        return nullptr;
    }
    return &variableData[variableID];
}


Scope* Scope::getParentScopePointer() {
    return parentScope;
}

bool Scope::isGlobal() {
    return parentScope == nullptr;
}

bool Scope::hasKey(std::string key) {
    return variableData.count(key) > 0;
}