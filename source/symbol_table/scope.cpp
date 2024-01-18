/**
 * @file scope.cpp
 * @brief scope.cpp contains implementations for the scope.hpp class
 * @details For method prototypes, see scope.hpp
 */

#include "scope.hpp"

// Default Constructor
Scope::Scope() { parentScope = nullptr; }

// Constructor for specifying a parent
Scope::Scope(Scope* parent) { parentScope = parent; }

// Destructor
Scope::~Scope() = default;

// Copy Constructor
Scope::Scope(Scope& otherScope) {
    parentScope = otherScope.getParentScopePointer();
    std::map<std::string, int> copyData(otherScope._variableData);
    _variableData = copyData;
}

//Deprecated
void Scope::addOrUpdateVariable(std::string variableID, int data) {
    _variableData.insert_or_assign(variableID, data);
}

bool Scope::hasLocalVariable(std::string variableID) { return hasKey(variableID); }

bool Scope::hasGlobalVariable(std::string variableID) {
    return hasKey(variableID) || (parentScope != nullptr && parentScope->hasGlobalVariable(variableID));
}

int* Scope::getVariable(std::string variableID) {
    if (!hasKey(variableID)) {
        if (parentScope != nullptr) { return parentScope->getVariable(variableID); }
        return nullptr;
    }
    return &_variableData[variableID];
}


Scope* Scope::getParentScopePointer() { return parentScope; }

bool Scope::isGlobal() { return parentScope == nullptr; }

bool Scope::hasKey(std::string key) { return _variableData.count(key) > 0; }

//Garbage Collection methods

void Scope::setVariableOccurrences(std::string variableID, unsigned int occurrences) {
    _variableUses.insert_or_assign(variableID, occurrences);
}

void Scope::freeVariableData(std::string variableID) {
    int* dataPointer = &_variableData[variableID];
    delete dataPointer;
}

void Scope::decrementUses(std::string variableID) {
    if(_variableUses[variableID] != 0) {
        _variableUses[variableID] = _variableUses[variableID] - 1;
    }
}

bool Scope::hasExpired(std::string variableID) {
    bool status = (_variableUses.find(variableID) != _variableUses.end()) && (_variableUses[variableID] == 0); //Oddly enough, not recording this variable cause false to yield in true conditions
    return status;
}