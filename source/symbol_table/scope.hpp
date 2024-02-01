/**
 * @file scope.hpp
 * @brief scope.hpp contains prototype methods for the scope class
 * @details For an implementation, see scope.cpp
 */

#ifndef CPP_UTILITIES_SCOPE_HPP
#define CPP_UTILITIES_SCOPE_HPP
#include <map>
#include <memory>
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
    void addOrUpdateVariable(const std::string& variableID, int data);

    /**
     * updateVariable() is similar to addOrUpdateVariable. This method will only update variables if they exist anywhere
     * in the scope.
     * @param variableID - The variable one wishes to change
     * @param newData - The new data to associate with the variable
     * @return - TRUE if the variable was successfully updated, otherwise FALSE is returned
     */
    bool updateVariable(const std::string& variableID, int newData);

    /**
     * hasLocalVariable returns a boolean value according to whether the specified variable exists in this scope.
     * For scanning all scopes for a variable, use _getVariable()
     * @param variableID - A string ID which the variable will be referenced by
     * @return - TRUE if the variable exists in the current scope, otherwise FALSE is returned
     */
    bool hasLocalVariable(const std::string& variableID);

    /**
     * hasGlobalVariable returns a boolean value according to whether the entire scope collection contains the specified
     * variable
     * @param variableID - A string ID which the variable will be referenced by
     * @return - TRUE if the variable exists in any scope, otherwise FALSE is returned
     */
    bool hasGlobalVariable(const std::string& variableID);

    /**
     * _getVariable returns a pointer to the desired variable in the scope. If the variable does not exist in this
     * scope, the parent scope will be scanned for the variable
     * @param variableID - A string ID which the variable will be referenced by
     * @return - A pointer to the specified data if it exists, otherwise a nullptr will return
     */
    int* getVariable(const std::string& variableID);

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

    // Garbage Collection methods

    /**
     * setVariableOccurrences() adds a key, value pair for variable occurrences in the Commander script
     * @param occurrences - An unsigned int value which determines how many times a variable is used in a script
     */
    void setVariableOccurrences(const std::string& variableID, unsigned int occurrences);

    /**
     * freeVariableData() destructs the data used by a variable
     * @param variableID - A string variableID to free data from
     * @return - TRUE if the data was destructed (or already destructed), otherwise FALSE is returned
     */
    bool freeVariableData(const std::string& variableID);

    /**
     * decrementUses() subtracts the number of the variable's occurrences by 1. If the number of occurrences are 0, the
     * value will not update further
     * @param variableID - The variable one wishes to decrement
     */
    void decrementUses(const std::string& variableID);

    /**
     * hasExpired() returns a boolean value according to whether the number of occurrences of a variable is equal to 0
     * @param variableID - The variable to check the garbage collection status of
     * @return - TRUE if the variable has no more occurrences, otherwise FALSE is returned
     */
    bool hasExpired(const std::string& variableID);

private:
    std::map<std::string, std::shared_ptr<int>>
            _variableData {};                           // uses a Key variableName to find it's associated object
    std::map<std::string, unsigned int> _variableUses;  // uses a Key variableName to find usages left (unsigned int)
    Scope* _parentScope = nullptr;  // Pointer to the parent scope object (i.e. this scope exists within another scope)


    /**
     * _hasDataKey() returns a boolean value according to whether _variableData has an entry of the specified name
     * @param key - A string value representing a variable ID
     * @return - TRUE if the variable exists in _variableData, otherwise FALSE is returned
     */
    bool _hasDataKey(const std::string& key);

    /**
     * _hasUsesKey() returns a boolean value according to whether _variableUses has an entry of the specified name
     * @param key - A string value representing a variable ID
     * @return - TRUE if the variable exists in _variableUses, otherwise FALSE is returned
     */
    bool _hasUsesKey(const std::string& key);

    /**
     * _tryGetUses() is a helper method for getting the number of variable uses. If a value does not exist, a new one
     * will be created with a value of 0
     * @param variableID - The variable one wishes to find or initialize
     * @return - TRUE if the method created a new entry in _variableUses, otherwise FALSE is returned
     */
    bool _tryGetUses(const std::string& variableID);
};


#endif  // CPP_UTILITIES_SCOPE_HPP
