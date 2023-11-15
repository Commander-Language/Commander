/**
 * SymbolTableOrganizer.h holds the prototypes for the SymbolTableOrganizer class
 */

#ifndef CPP_UTILITIES_SYMBOLTABLEORGANIZER_HPP
#define CPP_UTILITIES_SYMBOLTABLEORGANIZER_HPP
#include "Scope.hpp"
#include <string>


class SymbolTableOrganizer {
public:
    SymbolTableOrganizer(); //Constructor
    ~SymbolTableOrganizer(); //Destructor
    SymbolTableOrganizer(SymbolTableOrganizer &otherTableOrganizer); //Copy-Constructor

    void pushSymbolTable(); //Add a new symbol table to the stack
    void popSymbolTable(); //remove a symbol table from the stack

    bool varExistsInCurrentSymbolTable(std::string variableID); //return TRUE if the variable exists at the top of the stack
    bool varExistsInScope(std::string variableID); //return TRUE if the variable can be found anywhere in the stack (recursive)

private:
    std::vector<Scope> symbolTables; //A vector containing each symbol table; some methods will use this to find a value while others will rely on Scope's recursiveness
};


#endif //CPP_UTILITIES_SYMBOLTABLEORGANIZER_HPP
