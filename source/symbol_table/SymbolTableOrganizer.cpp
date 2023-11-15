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

//Copy-Constructor
SymbolTableOrganizer::SymbolTableOrganizer(SymbolTableOrganizer &otherTableOrganizer) {
    //TODO
}

/**
 * pushSymbolTable() adds a new Scope object to the SymbolTables vector and updates its parent scope pointer
 */
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

/**
 * popSymbolTable() removes the Scope object at the back of the SymbolTables vecor
 */
void SymbolTableOrganizer::popSymbolTable() {
    symbolTables.pop_back();
}

/**
 * varExistsInCurrentSymbolTable() returns a boolean according to whether a variable of the specified string exists in the current scope (top of stack)
 * @param variableID - A string representing the ID of a variable (e.g. "cat")
 * @return - TRUE if the variable exists in the top of the stack, otherwise FALSE is returned
 */
bool SymbolTableOrganizer::varExistsInCurrentSymbolTable(std::string variableID) {
    if(symbolTables.back().hasVariable(variableID)) {
        return true;
    }
    return false;
}

/**
 * varExistsInScope() returns a boolean according to whether a variable of the specified string ID exists anywhere in the current scope
 * This will search all variables in the SymbolTables vector
 * @param variableID - A string representing the ID of a variable
 * @return - TRUE if the variable exists anywhere in the vector, otherwise FALSE is returned
 */
bool SymbolTableOrganizer::varExistsInScope(std::string variableID) {
    //TODO: may want to rewrite this method
    int currentScope = symbolTables.size();
    while(currentScope != -1) {
        if(symbolTables[currentScope].hasVariable(variableID)) {
            return true;
        }
        currentScope--;
    }
    return false;
}

