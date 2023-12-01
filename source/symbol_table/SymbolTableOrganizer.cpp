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
    for(int currentIndex = 0; currentIndex < otherTableOrganizer.symbolTables.size(); currentIndex++) {
        symbolTables[currentIndex] = (otherTableOrganizer.symbolTables[currentIndex]); //may not work as intended
    }
}

/**
 * pushSymbolTable() adds a new Scope object to the SymbolTables vector and updates its parent scope pointer
 */
void SymbolTableOrganizer::pushSymbolTable() {
    if(symbolTables.size() < 1) {
        Scope headScope = Scope();
        symbolTables.push_back(&headScope);
    }
    else {
        Scope nextScope = Scope(symbolTables.back());
        symbolTables.push_back(&nextScope);
    }
}

/**
 * popSymbolTable() removes the Scope object at the back of the SymbolTables vector
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
    if(symbolTables.back()->hasVariable(variableID)) {
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
    if(symbolTables[symbolTables.size() - 1]->getVariable(variableID) == nullptr) {
        return false;
    }
    return true;
}

/**
 * isScopeGlobal() returns a boolean according to whether a scope is the head.
 * @return TRUE if the scope can be seen everywhere, otherwise FALSE is returned.
 */
bool SymbolTableOrganizer::isScopeGlobal() {
    if(symbolTables.size() == 0) {
        return true; //Error case - Assume true if nothing exists
    }
    //bool global = symbolTables[symbolTables.size() - 1]->isGlobal(); //removing this causes tests to fail
    bool global = symbolTables.size() <= 1;
    return global;
}

/**
 * addVariable() will add a provided variable to the top of the stack (i.e. current scope)
 * For updating a previously-established variable, use updateVariable().
 * @param variableID - A string representing the variable's ID (e.g. cat)
 * @param data - An object which the variable should be associated with
 */
void SymbolTableOrganizer::addVariable(std::string variableID, int data) {
    symbolTables.back()->addOrUpdateVariable(variableID, data);
}

/**
 * updateVariable() will attempt to update a variable from anywhere in the scope. If the variable does not exist, nothing
 * will be updated.
 * @note - This method assumes no duplicate variables exist in the scope!
 * @param variableID - A string representing the variable's ID
 * @param data - A new object which the variable should be associated with
 */
void SymbolTableOrganizer::updateVariable(std::string variableID, int data) {
    for(int currentScope = 0; currentScope < symbolTables.size(); currentScope++) {
        if(symbolTables[currentScope]->hasVariable(variableID)) {
            symbolTables[currentScope]->addOrUpdateVariable(variableID, data);
            return; //Quit early if we've updated our requested variable
        }
    }
}