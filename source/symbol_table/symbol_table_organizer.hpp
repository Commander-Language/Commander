/**
 * @file symbol_table_organizer.hpp
 * @brief symbol_table_organizer.hpp contains the prototypes for the SymbolTableOrganizer class
 * @details For an implementation, see symbol_table_organizer.cpp
 */

#ifndef CPP_UTILITIES_SYMBOL_TABLE_ORGANIZER_HPP
#define CPP_UTILITIES_SYMBOL_TABLE_ORGANIZER_HPP
#include "source/symbol_table/scope.hpp"
#include <string>
#include <vector>

class SymbolTableOrganizer {
public:
    /**
     * SymbolTableOrganizer() serves as a default constructor for the class
     */
    SymbolTableOrganizer();  // Constructor

    /**
     * ~SymbolTableOrganizer() destructs all Scope objects being held before the organizer is removed
     */
    ~SymbolTableOrganizer();  // Destructor

    /**
     * SymbolTableOrganizer(otherTableOrganizer) serves as a copy constructor for the SymbolTableOrganizer class
     * @param otherTableOrganizer - The SymbolTableOrganizer to copy
     */
    SymbolTableOrganizer(SymbolTableOrganizer& otherTableOrganizer);  // Copy-Constructor

    /**
     * pushSymbolTable() adds a new scope object to the SymbolTables vector and updates its parent scope pointer
     */
    void pushSymbolTable();  // Add a new symbol table to the stack

    /**
     * popSymbolTable() removes the scope object at the back of the SymbolTables vector
     */
    void popSymbolTable();  // remove a symbol table from the stack

    /**
     * addOrUpdateVariable() will add a provided variable to the top of the stack (i.e. current scope)
     * @param variableID - A string representing the variable's ID (e.g. cat)
     * @param data - An object which the variable should be associated with
     */
    template <typename T>
    void addOrUpdateVariable(const std::string& variableID, T data) {
        _symbolTables.back()->addOrUpdateVariable(variableID, data);
    }

    /**
     * addOrUpdateVariable() will add the provided variable to the top of the stack (i.e. current scope)
     * This is an overloaded version of a two-parameter addOrUpdate variable. This method is reccommended for
     * initializing variables for garbage collection.
     * If the user is unsure of the number of occurrences, they should use NULL
     * @param variableID - A string representing the variable's ID
     * @param data - An object which the variable should associate with
     * @param occurrences - The number of times the variable is found in the Commander script - will not update if the
     * parameter passed in is null
     */
    template <typename T>
    void addOrUpdateVariable(const std::string& variableID, T data, unsigned int occurrences) {
        _symbolTables.back()->addOrUpdateVariable(variableID, data);
        _symbolTables.back()->setVariableOccurrences(variableID, occurrences);
    }

    /**
     * freeVariableData() will attempt to remove the data associated with a string variableID.
     * If the variable does not exist, nothing will be removed
     * @param variableID - The variable to remove
     * @return - TRUE if the variable was removed, otherwise FALSE is returned
     */
    bool tryFreeVariableData(const std::string& variableID);

    /**
     * forceFreeVariableData() will garbage collect the variable, regardless of whether it has expired.
     * If the variable does not exist, nothing will be removed
     * @param variableID - The variable to remove
     */
    void forceFreeVariableData(const std::string& variableID);

    /**
     * variableHasExpired() will check if the variable's occurrences are equal to zero
     * @param variableID - The variable to check
     * @return - TRUE if the variable has no more occurrences, otherwise FALSE is returned
     */
    bool variableHasExpired(const std::string& variableID);

    /**
     * getScope() returns a pointer to the last scope in the symbol_table_organizer
     * @return - A pointer to the last scope object present or nullptr
     */
    Scope* getScope();

    /**
     * varExistsInCurrentSymbolTable() returns a boolean according to whether a variable of the specified string exists
     * in the current scope (top of stack)
     * @param variableID - A string representing the ID of a variable (e.g. "cat")
     * @return - TRUE if the variable exists in the top of the stack, otherwise FALSE is returned
     */
    bool varExistsInCurrentSymbolTable(const std::string& variableID);

    /**
     * varExistsInScope() returns a boolean according to whether a variable of the specified string ID exists anywhere
     * in the current scope This will search all variables in the SymbolTables vector
     * @param variableID - A string representing the ID of a variable
     * @return - TRUE if the variable exists anywhere in the vector, otherwise FALSE is returned
     */
    bool varExistsInScope(const std::string& variableID);

    /**
     * isScopeGlobal() returns a boolean according to whether a scope is the head.
     * @return TRUE if the scope can be seen everywhere, otherwise FALSE is returned.
     */
    bool isScopeGlobal();  // return TRUE if the current scope is a global scope

    /**
     * getVariable() returns a Type pointer to the desired variable
     * @tparam T - The expected type of the variable
     * @param variableID - A string ID which the variable is referenced by
     * @return - A Type pointer to the variable's data if it exists, otherwise a nullptr is returned
     * @warning - The desired type must be identical to the stored type. If one wishes to retrieve an int as a float, for instance, they must first call getVariableAsType<int>() and cast the result.
     */
    template <typename T>
    T* getVariable(std::string variableID) {
        return _symbolTables.back()->getVariable<T>(variableID);
    }

private:
    std::vector<Scope*> _symbolTables {};  // A vector containing each symbol table; some methods will use this to find
                                           // a value while others will rely on scope's recursiveness
};


#endif  // CPP_UTILITIES_SYMBOL_TABLE_ORGANIZER_HPP
