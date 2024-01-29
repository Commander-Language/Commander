/**
 * @file scope.cpp
 * @brief scope.cpp contains implementations for the scope.hpp class
 * @details For method prototypes, see scope.hpp
 */

#include "scope.hpp"

// Default Constructor
Scope::Scope() { _parentScope = nullptr; }

// Constructor for specifying a parent
Scope::Scope(Scope* parent) { _parentScope = parent; }

// Destructor
Scope::~Scope() = default;

// Copy Constructor
Scope::Scope(Scope& otherScope) {
    _parentScope = otherScope.getParentScopePointer();
    std::map<std::string, std::shared_ptr<int>> copyData(otherScope._variableData);
    std::map<std::string, unsigned int> copyUses(otherScope._variableUses);
    _variableData = copyData;
    _variableUses = copyUses;
}

void Scope::addOrUpdateVariable(std::string variableID, int data) {
    if(!updateVariable(variableID, data)) {
        _variableData.insert_or_assign(variableID, std::make_shared<int>(data));
    }
}

bool Scope::updateVariable(std::string variableID, int newData) {
    if(!hasDataKey(variableID)) {
        if(_parentScope != nullptr) {
            return _parentScope->updateVariable(variableID, newData);
        }
        else {
            return false;
        }
    }
    else {
        _variableData.insert_or_assign(variableID, std::make_shared<int>(newData));
        if(!hasUsesKey(variableID)) {
            _variableUses.insert_or_assign(variableID, 0); //If the variable somehow doesn't have uses assigned, default to 0
        }
        else {
            decrementUses(variableID);
        }
        return true;
    }
}

bool Scope::hasLocalVariable(std::string variableID) { return hasDataKey(variableID); }

bool Scope::hasGlobalVariable(std::string variableID) {
    return hasDataKey(variableID) || (_parentScope != nullptr && _parentScope->hasGlobalVariable(variableID));
}

int* Scope::getVariable(std::string variableID) {
    if (!hasDataKey(variableID)) {
        if (_parentScope != nullptr) { return _parentScope->getVariable(variableID); }
        return nullptr;
    }
    decrementUses(variableID);
    return _variableData[variableID].get();
}


Scope* Scope::getParentScopePointer() { return _parentScope; }

bool Scope::isGlobal() { return _parentScope == nullptr; }

bool Scope::hasDataKey(std::string key) { return _variableData.count(key) > 0; }

bool Scope::hasUsesKey(std::string key) { return _variableUses.count(key) > 0; }

//Garbage Collection methods

void Scope::setVariableOccurrences(std::string variableID, unsigned int occurrences) {
    //We won't deep search for the variable here - This method is intended for initialization rather than updating!
    _variableUses.insert_or_assign(variableID, occurrences);
}

bool Scope::freeVariableData(std::string variableID) {
    //if a shared pointer is unique, resetting (allegedly) destructs the object
    if(!hasDataKey(variableID)) {
        if(_parentScope != nullptr) {
            return _parentScope->freeVariableData(variableID);
        }
        else {
            return false; //return false if we've reached the end of the Scope chain and the variable doesn't exist
        }
    }
    else {
        _variableData[variableID].reset();
        return true; //return true if we've freed space or have already freed space
    }
}

void Scope::decrementUses(std::string variableID) {
    if(!tryGetUses(variableID)) {
        if(_parentScope != nullptr) {
            return _parentScope->decrementUses(variableID);
        }
    }
    else{
        if(_variableUses[variableID] != 0) {
            _variableUses[variableID] = _variableUses[variableID] - 1;
        }
    }
}

bool Scope::hasExpired(std::string variableID) {
    if(!hasUsesKey(variableID)) {
        if(_parentScope != nullptr) {
            return _parentScope->hasExpired(variableID);
        }
        else {
            return false; //return false if nothing exists
        }
    }
    else {
        return (_variableUses[variableID] == 0);
    }
}

bool Scope::tryGetUses(std::string variableID) {
    if(!hasUsesKey(variableID)) {
        return false;
    }
    return true;
}