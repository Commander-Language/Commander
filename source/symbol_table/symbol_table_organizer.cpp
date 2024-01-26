/**
 * @file symbol_table_organizer.cpp
 * @brief symbol_table_organizer.cpp contains the implementation for the prototype methods in symbol_table_organizer.hpp
 * @details For method prototypes, see symbol_table_organizer.hpp
 */

#include "symbol_table_organizer.hpp"

// Constructor
SymbolTableOrganizer::SymbolTableOrganizer() {}

// Destructor
SymbolTableOrganizer::~SymbolTableOrganizer() {
    for (int currentScopeObject = 0; currentScopeObject < _symbolTables.size(); currentScopeObject++) {
        delete _symbolTables.back();
        _symbolTables.pop_back();
    }
}

// Copy-Constructor
SymbolTableOrganizer::SymbolTableOrganizer(SymbolTableOrganizer& otherTableOrganizer) {
    for (int currentScope = 0; currentScope < otherTableOrganizer._symbolTables.size(); currentScope++) {
        // vector(otherVector) creates shallow copies of the Scope object, so we'll need to make our own deep copies
        // here
        _symbolTables.push_back(new Scope(*otherTableOrganizer._symbolTables[currentScope]));
    }
}

void SymbolTableOrganizer::pushSymbolTable() {
    if (_symbolTables.size() < 1) {
        Scope* headScope = new Scope();
        _symbolTables.push_back(headScope);
    } else {
        Scope* nextScope = new Scope(_symbolTables.back());
        _symbolTables.push_back(nextScope);
    }
}

void SymbolTableOrganizer::popSymbolTable() {
    if (_symbolTables.empty()) {
        return;  // return early if no scopes are present
    }
    delete _symbolTables.back();
    _symbolTables.pop_back();
}

void SymbolTableOrganizer::addOrUpdateVariable(std::string variableID, int data) {
    _symbolTables.back()->addOrUpdateVariable(variableID, data);
}

void SymbolTableOrganizer::addOrUpdateVariable(std::string variableID, int data, unsigned int occurrences) {
    _symbolTables.back()->setVariableOccurrences(variableID, occurrences);
    _symbolTables.back()->addOrUpdateVariable(variableID, data);
}

Scope* SymbolTableOrganizer::getScope() {
    if (_symbolTables.empty()) { return nullptr; }
    return _symbolTables.back();
}

bool SymbolTableOrganizer::varExistsInCurrentSymbolTable(std::string variableID) {
    return _symbolTables.back()->hasLocalVariable(variableID);
}

bool SymbolTableOrganizer::varExistsInScope(std::string variableID) {
    return _symbolTables.back()->hasGlobalVariable(variableID);
}

int* SymbolTableOrganizer::getVariable(std::string variableID) {
    return _symbolTables.back()->getVariable(variableID);
}

bool SymbolTableOrganizer::isScopeGlobal() {
    if (_symbolTables.size() == 0) {
        return true;  // Error case - Assume true if nothing exists
    }
    bool global = _symbolTables.size() <= 1;
    return global;
}

bool SymbolTableOrganizer::tryFreeVariableData(std::string variableID) {
    if(_symbolTables.back()->hasExpired(variableID)) {
        _symbolTables.back()->freeVariableData(variableID);
        return true;
    }
    return false;
}

void SymbolTableOrganizer::forceFreeVariableData(std::string variableID) {
    _symbolTables.back()->freeVariableData(variableID);
}

bool SymbolTableOrganizer::variableHasExpired(std::string variableID) {
    return _symbolTables.back()->hasExpired(variableID);
}
