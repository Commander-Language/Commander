/**
 * @file scope.hpp
 * @brief scope.hpp contains prototype methods for the scope class
 * @details For an implementation, see scope.cpp
 */

#ifndef CPP_UTILITIES_SCOPE_HPP
#define CPP_UTILITIES_SCOPE_HPP
#include <map>
#include <string>

// TODO: replace data with a generic object class (template class?)

class Scope {
public:
    /**
     * scope() serves as a default constructor for the scope object. Its parent will default to nullptr
     */
    Scope();

    /**
     * scope(scope* parent) is a constructor which specifies the parent scope object.
     * @param parent - A pointer to the parent of the scope
     */
    Scope(Scope* parent);

    /**
     * scope() serves as a destructor for the scope object
     */
    ~Scope();

    /**
     * Not to be confused with scope(scope* parent), this method serves as a copy constructor
     * @param otherScope - The scope one wishes to copy
     */
    Scope(Scope& otherScope);

    /**
     * addOrUpdateVariable will attempt to add the relevant data to the scope.
     * @param variableID - A String ID which the variable will be referenced by (e.g. "cat")
     * @param data - An object which will be stored as data (e.g. 14, "dog", std::vector<int>, etc.)
     * @param occurences - The number of times the variable is used in the script (used for garbage collection)
     */
    void addOrUpdateVariable(std::string variableID, int data);

    /**
     * hasLocalVariable returns a boolean value according to whether the specified variable exists in this scope.
     * For scanning all scopes for a variable, use getVariable()
     * @param variableID - A string ID which the variable will be referenced by
     * @return - TRUE if the variable exists in the current scope, otherwise FALSE is returned
     */
    bool hasLocalVariable(std::string variableID);

    /**
     * hasGlobalVariable returns a boolean value according to whether the entire scope collection contains the specified
     * variable
     * @param variableID - A string ID which the variable will be referenced by
     * @return - TRUE if the variable exists in any scope, otherwise FALSE is returned
     */
    bool hasGlobalVariable(std::string variableID);

    /**
     * getVariable returns a pointer to the desired variable in the scope. If the variable does not exist in this scope,
     * the parent scope will be scanned for the variable
     * @param variableID - A string ID which the variable will be referenced by
     * @return - A pointer to the specified data if it exists, otherwise a nullptr will return
     */
    int* getVariable(std::string variableID);

    /**
     * getParenScopePointer() returns a pointer to the parent of this scope
     * @return - A pointer to the parent of this scope
     */
    Scope* getParentScopePointer();

    /**
     * isGlobal() will return a value according to whether the scope has a parent
     * @return - FALSE if the scope has a parent, TRUE if the scope returns a nullptr
     */
    bool isGlobal();

    //Garbage Collection methods

    /**
     * setVariableOccurences() adds a key, value pair for variable occurances in the Commander script
     * @param occurences - An unsigned int value which determines how many times a variable is used in a script
     */
    void setVariableOccurences(std::string variableID, unsigned int occurences);

    /**
     * freeVariableData() destructs the data used by a variable
     * @param variableID - A string variableID to free data from
     */
    void freeVariableData(std::string variableID);

    /**
     * TODO
     * @param variableID
     */
    void decrementUses(std::string variableID);

    /**
     * TODO
     * @param variableID
     * @return
     */
    bool hasExpired(std::string variableID);

private:
    std::map<std::string, int> _variableData {};  // uses a Key variableName to find it's associated object
    std::map<std::string, unsigned int> _variableUses; //uses a Key variableName to find usages left (unsigned int)
    Scope* parentScope = nullptr;  // Pointer to the parent scope object (i.e. this scope exists within another scope)


    /**
     * hasKey() returns a boolean value according to whether variableData has an entry of the specified name
     * @param key - A string value representing a variable ID
     * @return - TRUE if the variable exists in variableData, otherwise FALSE is returned
     */
    bool hasKey(std::string key);

};


#endif  // CPP_UTILITIES_SCOPE_HPP
