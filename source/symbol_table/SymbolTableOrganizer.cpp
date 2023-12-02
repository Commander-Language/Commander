/**
 * SymbolTableOrganizer is a class which holds and maintains Scope objects
 * These scopes can be accessed through the class
 */

#include "SymbolTableOrganizer.hpp"

//Constructor
SymbolTableOrganizer::SymbolTableOrganizer() {
}

//Destructor
SymbolTableOrganizer::~SymbolTableOrganizer() {
    //for(int currentScopeObject = 0; currentScopeObject < symbolTables.size(); currentScopeObject++) {
    //    delete symbolTables[currentScopeObject];
    //}
}

//Copy-Constructor
SymbolTableOrganizer::SymbolTableOrganizer(SymbolTableOrganizer &otherTableOrganizer) {
    for(int currentIndex = 0; currentIndex < otherTableOrganizer.symbolTables.size(); currentIndex++) {
        symbolTables[currentIndex] = (otherTableOrganizer.symbolTables[currentIndex]); //may not work as intended
    }
}

void SymbolTableOrganizer::pushSymbolTable() {
    if(symbolTables.size() < 1) {
        Scope headScope = new Scope();
        symbolTables.push_back(&headScope);
    }
    else {
        Scope nextScope = new Scope(symbolTables.back());
        symbolTables.push_back(&nextScope);
    }
}

//TODO: consider smart pointers
void SymbolTableOrganizer::popSymbolTable() {
    if(symbolTables.empty()) {
        return; //return early if no scopes are present
    }
    delete symbolTables.back();
    symbolTables.pop_back(); //despite the name, pop_back does not return the last item while removing it
}

void SymbolTableOrganizer::addVariable(std::string variableID, int data) {
    symbolTables.back()->addOrUpdateVariable(variableID, data);
}

void SymbolTableOrganizer::updateVariable(std::string variableID, int data) {
    for(int currentScope = 0; currentScope < symbolTables.size(); currentScope++) {
        if(symbolTables[currentScope]->hasVariable(variableID)) {
            symbolTables[currentScope]->addOrUpdateVariable(variableID, data);
            return; //Quit early if we've updated our requested variable
        }
    }
}

Scope* SymbolTableOrganizer::getScope() {
    if(symbolTables.empty()) {
        return nullptr;
    }
    return symbolTables.back();
}

bool SymbolTableOrganizer::varExistsInCurrentSymbolTable(std::string variableID) {
    if(symbolTables.back()->hasVariable(variableID)) {
        return true;
    }
    return false;
}

bool SymbolTableOrganizer::varExistsInScope(std::string variableID) {
    if(symbolTables[symbolTables.size() - 1]->getVariable(variableID) == nullptr) {
        return false;
    }
    return true;
}

bool SymbolTableOrganizer::isScopeGlobal() {
    if(symbolTables.size() == 0) {
        return true; //Error case - Assume true if nothing exists
    }
    bool global = symbolTables.size() <= 1;
    return global;
}