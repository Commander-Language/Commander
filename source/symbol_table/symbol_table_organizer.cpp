/**
 * @file symbol_table_organizer.cpp
 * @brief symbol_table_organizer.cpp contains the implementation for the prototype methods in symbol_table_organizer.hpp
 * @details For method prototypes, see symbol_table_organizer.hpp
 */

#include "symbol_table_organizer.hpp"

// Constructor
SymbolTableOrganizer::SymbolTableOrganizer() = default;

// Destructor
SymbolTableOrganizer::~SymbolTableOrganizer() {
    for (int currentScopeObject = 0; currentScopeObject < _symbolTables.size(); currentScopeObject++) {
        delete _symbolTables.back();
        _symbolTables.pop_back();
    }
}

// Copy-Constructor
SymbolTableOrganizer::SymbolTableOrganizer(SymbolTableOrganizer& otherTableOrganizer) {
    for (Scope* currentScope : otherTableOrganizer._symbolTables) {
        // vector(otherVector) creates shallow copies of the Scope object, so we'll need to make our own deep copies
        // here
        _symbolTables.push_back(new Scope(*currentScope));
    }
    for (int currentScope = 0; currentScope < otherTableOrganizer._symbolTables.size(); currentScope++) {}
}

void SymbolTableOrganizer::pushSymbolTable() {
    _symbolTables.push_back(_symbolTables.empty() ? new Scope() : new Scope(_symbolTables.back()));
}

void SymbolTableOrganizer::popSymbolTable() {
    if (_symbolTables.empty()) {
        return;  // return early if no scopes are present
    }
    delete _symbolTables.back();
    _symbolTables.pop_back();
}

Scope* SymbolTableOrganizer::getScope() {
    if (_symbolTables.empty()) { return nullptr; }
    return _symbolTables.back();
}

bool SymbolTableOrganizer::varExistsInCurrentSymbolTable(const std::string& variableID) {
    return _symbolTables.back()->hasLocalVariable(variableID);
}

bool SymbolTableOrganizer::varExistsInScope(const std::string& variableID) {
    return _symbolTables.back()->hasGlobalVariable(variableID);
}

bool SymbolTableOrganizer::isScopeGlobal() { return _symbolTables.size() <= 1; }

bool SymbolTableOrganizer::tryFreeVariableData(const std::string& variableID) {
    if (_symbolTables.back()->hasExpired(variableID)) {
        _symbolTables.back()->freeVariableData(variableID);
        return true;
    }
    return false;
}

void SymbolTableOrganizer::forceFreeVariableData(const std::string& variableID) {
    _symbolTables.back()->freeVariableData(variableID);
}

bool SymbolTableOrganizer::variableHasExpired(const std::string& variableID) {
    return _symbolTables.back()->hasExpired(variableID);
}
