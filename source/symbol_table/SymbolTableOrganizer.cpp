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

}

SymbolTableOrganizer::SymbolTableOrganizer(SymbolTableOrganizer &otherTableOrganizer) {
    //TODO
}

void SymbolTableOrganizer::pushSymbolTable() {
    if(symbolTables.size < 1) {
        Scope headScope = Scope();
        symbolTables.push_back(headScope);
    }
    else {
        Scope nextScope = Scope(symbolTables.back().getParentScopePointer());
        symbolTables.push_back(nextScope);
    }
}

void SymbolTableOrganizer::popSymbolTable() {
    symbolTables.pop_back();
}

