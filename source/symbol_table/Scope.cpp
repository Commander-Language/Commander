/**
 * Scope.cpp serves as the implementation for the Scope class.
 * Scope contains a symbol table - a map which uses a string ID to return an object
 */

#include "Scope.hpp"

//Default Constructor
Scope::Scope() {
    parentScope = nullptr;
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

void Scope::addOrUpdateVariable(std::string variableID, int data) {
    variableData.insert_or_assign(variableID, data);
}

bool Scope::hasLocalVariable(std::string variableID) {
    if(hasKey(variableID)) {
        return true;
    }
    return false;
}

bool Scope::hasGlobalVariable(std::string variableID) {
    if(!hasKey(variableID)) {
        if(parentScope != nullptr) {
            return parentScope->hasGlobalVariable(variableID);
        }
        return false;
    }
    return true;
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
    return parentScope == NULL || parentScope == nullptr;
}

bool Scope::hasKey(std::string key) {
    if(variableData.find(key) == variableData.end()) { //.end() returns one index out-of-bounds from the data's contents (i.e. the equivalent of variableData.size())
        return false;
    }
    return true;
}