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

    void pushSymbolTable(); //Add a new symbol table to the stack
    void popSymbolTable(); //remove a symbol table from the stack

    void addVariable(std::string variableID, int data); //add a variable to the current scope
    void updateVariable(std::string variableID, int data); //update a variable which may not be present in the current scope

    bool varExistsInCurrentSymbolTable(std::string variableID); //return TRUE if the variable exists at the top of the stack
    bool varExistsInScope(std::string variableID); //return TRUE if the variable can be found anywhere in the stack (recursive)
    bool isScopeGlobal(); //return TRUE if the current scope is a global scope

private:
    std::vector<Scope*> symbolTables; //A vector containing each symbol table; some methods will use this to find a value while others will rely on Scope's recursiveness
};


#endif //CPP_UTILITIES_SYMBOLTABLEORGANIZER_HPP
