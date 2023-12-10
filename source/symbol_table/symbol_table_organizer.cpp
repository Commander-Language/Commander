/**
 * @file symbol_table_organizer.cpp
 * @brief symbol_table_organizer.cpp contains the implementation for the prototype methods in symbol_table_organizer.hpp
 * @details For method prototypes, see symbol_table_organizer.hpp
 */

#include "symbol_table_organizer.hpp"

//Constructor
SymbolTableOrganizer::SymbolTableOrganizer() {
}

//Destructor
SymbolTableOrganizer::~SymbolTableOrganizer() {
    for(int currentScopeObject = 0; currentScopeObject < _symbolTables.size(); currentScopeObject++) {
        delete _symbolTables.top();
        _symbolTables.pop();
    }
}

//Copy-Constructor
SymbolTableOrganizer::SymbolTableOrganizer(SymbolTableOrganizer &otherTableOrganizer) {
    _symbolTables = std::stack(otherTableOrganizer._symbolTables);
}

void SymbolTableOrganizer::pushSymbolTable() {
    if(_symbolTables.size() < 1) {
        Scope* headScope = new Scope();
        _symbolTables.push(headScope);
    }
    else {
        Scope* nextScope = new Scope(_symbolTables.top());
        _symbolTables.push(nextScope);
    }
}

void SymbolTableOrganizer::popSymbolTable() {
    if(_symbolTables.empty()) {
        return; //return early if no scopes are present
    }
    delete _symbolTables.top();
    _symbolTables.pop();
}

void SymbolTableOrganizer::addOrUpdateVariable(std::string variableID, int data) {
    _symbolTables.top()->addOrUpdateVariable(variableID, data);
}

Scope* SymbolTableOrganizer::getScope() {
    if(_symbolTables.empty()) {
        return nullptr;
    }
    return _symbolTables.top();
}

bool SymbolTableOrganizer::varExistsInCurrentSymbolTable(std::string variableID) {
    return _symbolTables.top()->hasLocalVariable(variableID);
}

bool SymbolTableOrganizer::varExistsInScope(std::string variableID) {
    return _symbolTables.top()->hasGlobalVariable(variableID);
}

int* SymbolTableOrganizer::getVariable(std::string variableID) {
    return _symbolTables.top()->getVariable(variableID);
}

bool SymbolTableOrganizer::isScopeGlobal() {
    if(_symbolTables.size() == 0) {
        return true; //Error case - Assume true if nothing exists
    }
    bool global = _symbolTables.size() <= 1;
    return global;
}