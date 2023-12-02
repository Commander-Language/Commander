/**
 * SymbolTableOrganizer.h holds the prototypes for the SymbolTableOrganizer class
 */

#ifndef CPP_UTILITIES_SYMBOLTABLEORGANIZER_HPP
#define CPP_UTILITIES_SYMBOLTABLEORGANIZER_HPP
#include "source/symbol_table/Scope.hpp"
#include <string>
#include <vector>

//TODO: replace int data with a generic object

class SymbolTableOrganizer {
public:
    SymbolTableOrganizer(); //Constructor
    ~SymbolTableOrganizer(); //Destructor
    SymbolTableOrganizer(SymbolTableOrganizer &otherTableOrganizer); //Copy-Constructor

    /**
     * pushSymbolTable() adds a new Scope object to the SymbolTables vector and updates its parent scope pointer
     */
    void pushSymbolTable(); //Add a new symbol table to the stack

    /**
     * popSymbolTable() removes the Scope object at the back of the SymbolTables vector
     */
    void popSymbolTable(); //remove a symbol table from the stack

    /**
     * addVariable() will add a provided variable to the top of the stack (i.e. current scope)
     * For updating a previously-established variable, use updateVariable().
     * @param variableID - A string representing the variable's ID (e.g. cat)
     * @param data - An object which the variable should be associated with
     */
    void addVariable(std::string variableID, int data); //add a variable to the current scope

    /**
     * updateVariable() will attempt to update a variable from anywhere in the scope. If the variable does not exist, nothing
     * will be updated.
     * @note - This method assumes no duplicate variables exist in the scope!
     * @param variableID - A string representing the variable's ID
     * @param data - A new object which the variable should be associated with
     */
    void updateVariable(std::string variableID, int data); //update a variable which may not be present in the current scope

    /**
     * getScope() returns a pointer to the last scope in the SymbolTableOrganizer
     * @return - A pointer to the last scope object present or nullptr
     */
    Scope* getScope();

    /**
     * varExistsInCurrentSymbolTable() returns a boolean according to whether a variable of the specified string exists in the current scope (top of stack)
     * @param variableID - A string representing the ID of a variable (e.g. "cat")
     * @return - TRUE if the variable exists in the top of the stack, otherwise FALSE is returned
     */
    bool varExistsInCurrentSymbolTable(std::string variableID); //return TRUE if the variable exists at the top of the stack

    /**
     * varExistsInScope() returns a boolean according to whether a variable of the specified string ID exists anywhere in the current scope
     * This will search all variables in the SymbolTables vector
     * @param variableID - A string representing the ID of a variable
     * @return - TRUE if the variable exists anywhere in the vector, otherwise FALSE is returned
     */
    bool varExistsInScope(std::string variableID); //return TRUE if the variable can be found anywhere in the stack (recursive)

    /**
     * isScopeGlobal() returns a boolean according to whether a scope is the head.
     * @return TRUE if the scope can be seen everywhere, otherwise FALSE is returned.
     */
    bool isScopeGlobal(); //return TRUE if the current scope is a global scope

private:
    std::vector<Scope*> symbolTables{}; //A vector containing each symbol table; some methods will use this to find a value while others will rely on Scope's recursiveness
};


#endif //CPP_UTILITIES_SYMBOLTABLEORGANIZER_HPP
