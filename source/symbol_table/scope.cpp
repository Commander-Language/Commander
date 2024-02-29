/**
 * @file scope.cpp
 * @brief scope.cpp contains implementations for the scope.hpp class
 * @details For method prototypes, see scope.hpp
 */

#include "scope.hpp"

// Default Constructor
Scope::Scope() = default;

// Constructor for specifying a parent
Scope::Scope(Scope* parent) : _parentScope(parent) {}

// Destructor
Scope::~Scope() = default;

// Copy Constructor
Scope::Scope(Scope& otherScope) {
    if (_parentScope != nullptr) {
        // If a Scope has a parent, recursively use the copy constructor, then set its address as this object's parent
        // Scope
        Scope newParent = Scope(*_parentScope);
        _parentScope = &newParent;
    } else {
        // If we've reached the end of the line, set the parentScope to nullptr
        _parentScope = nullptr;
    }
    std::map<std::string, std::shared_ptr<std::any>> const copyData(otherScope._variableData);
    std::map<std::string, unsigned int> const copyUses(otherScope._variableUses);
    _variableData = copyData;
    _variableUses = copyUses;
}

void Scope::addOrUpdateVariable(const std::string& variableID, const std::any& data) {
    if (!updateVariable(variableID, data)) {
        _variableData.insert_or_assign(variableID, std::make_shared<std::any>(data));
    }
}

bool Scope::updateVariable(const std::string& variableID, const std::any& newData) {
    if (!_hasDataKey(variableID)) {
        if (_parentScope != nullptr) { return _parentScope->updateVariable(variableID, newData); }
        return false;
    }
    _variableData.insert_or_assign(variableID, std::make_shared<std::any>(newData));
    if (!_hasUsesKey(variableID)) {
        _variableUses.insert_or_assign(variableID,
                                       0);  // If the variable somehow doesn't have uses assigned, default to 0
    } else {
        decrementUses(variableID);
    }
    return true;
}

bool Scope::hasLocalVariable(const std::string& variableID) { return _hasDataKey(variableID); }

bool Scope::hasGlobalVariable(const std::string& variableID) {
    return _hasDataKey(variableID) || (_parentScope != nullptr && _parentScope->hasGlobalVariable(variableID));
}

Scope* Scope::getParentScopePointer() { return _parentScope; }

bool Scope::isGlobal() { return _parentScope == nullptr; }

bool Scope::_hasDataKey(const std::string& key) { return _variableData.count(key) > 0; }

bool Scope::_hasUsesKey(const std::string& key) { return _variableUses.count(key) > 0; }

// Garbage Collection methods

void Scope::setVariableOccurrences(const std::string& variableID, unsigned int occurrences) {
    // We won't deep search for the variable here - This method is intended for initialization rather than updating!
    _variableUses.insert_or_assign(variableID, occurrences);
}

bool Scope::freeVariableData(const std::string& variableID) {
    // if a shared pointer is unique, resetting (allegedly) destructs the object
    if (!_hasDataKey(variableID)) {
        if (_parentScope != nullptr) { return _parentScope->freeVariableData(variableID); }
        return false;  // return false if we've reached the end of the Scope chain and the variable doesn't exist
    }
    _variableData[variableID].reset();
    return true;  // return true if we've freed space or have already freed space
}

void Scope::decrementUses(const std::string& variableID) {
    if (!_tryGetUses(variableID)) {
        if (_parentScope != nullptr) { return _parentScope->decrementUses(variableID); }
    } else {
        if (_variableUses[variableID] != 0) { _variableUses[variableID] = _variableUses[variableID] - 1; }
    }
}

bool Scope::hasExpired(const std::string& variableID) {
    if (!_hasUsesKey(variableID)) {
        if (_parentScope != nullptr) { return _parentScope->hasExpired(variableID); }
        return false;  // return false if nothing exists
    }
    return (_variableUses[variableID] == 0);
}

bool Scope::_tryGetUses(const std::string& variableID) { return _hasUsesKey(variableID); }