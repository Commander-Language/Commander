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
   for(int currentScopeObject = 0; currentScopeObject < symbolTables.size(); currentScopeObject++) {
       delete symbolTables[currentScopeObject];
   }
}

//Copy-Constructor
SymbolTableOrganizer::SymbolTableOrganizer(SymbolTableOrganizer &otherTableOrganizer) {
   for(int currentIndex = 0; currentIndex < otherTableOrganizer.symbolTables.size(); currentIndex++) {
       symbolTables[currentIndex] = (otherTableOrganizer.symbolTables[currentIndex]); //may not work as intended
   }
}

void SymbolTableOrganizer::pushSymbolTable() {
   if(symbolTables.size() < 1) {
       Scope* headScope = new Scope();
       symbolTables.push_back(headScope);
   }
   else {
       Scope* nextScope = new Scope(symbolTables.back());
       symbolTables.push_back(nextScope);
   }
}

void SymbolTableOrganizer::popSymbolTable() {
   if(symbolTables.empty()) {
       return; //return early if no scopes are present
   }
   delete symbolTables.back();
   symbolTables.pop_back(); //despite the name, pop_back does not return the last item while removing it
}

void SymbolTableOrganizer::addOrUpdateVariable(std::string variableID, int data) {
   symbolTables.back()->addOrUpdateVariable(variableID, data);
}

Scope* SymbolTableOrganizer::getScope() {
   if(symbolTables.empty()) {
       return nullptr;
   }
   return symbolTables.back();
}

bool SymbolTableOrganizer::varExistsInCurrentSymbolTable(std::string variableID) {
   return symbolTables.back()->hasLocalVariable(variableID);
}

bool SymbolTableOrganizer::varExistsInScope(std::string variableID) {
   return symbolTables.back()->hasGlobalVariable(variableID);
}

int* SymbolTableOrganizer::getVariable(std::string variableID) {
   return symbolTables.back()->getVariable(variableID);
}

bool SymbolTableOrganizer::isScopeGlobal() {
   if(symbolTables.size() == 0) {
       return true; //Error case - Assume true if nothing exists
   }
   bool global = symbolTables.size() <= 1;
   return global;
}