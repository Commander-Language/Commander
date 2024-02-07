/**
 * @file scope.hpp
 * @brief scope.hpp contains prototype methods for the scope class
 * @details For an implementation, see scope.cpp
 */

#ifndef CPP_UTILITIES_SCOPE_HPP
#define CPP_UTILITIES_SCOPE_HPP
#include <map>
#include <string>
#include <memory>
#include <any>

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
    void addOrUpdateVariable(std::string variableID, std::any data);

    /**
     * updateVariable() is similar to addOrUpdateVariable. This method will only update variables if they exist anywhere in the scope.
     * @param variableID - The variable one wishes to change
     * @param newData - The new data to associate with the variable
     * @return - TRUE if the variable was successfully updated, otherwise FALSE is returned
     */
    bool updateVariable(std::string variableID, std::any newData);

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
     * @deprecated
     * getVariable returns a pointer to the desired variable in the scope. If the variable does not exist in this scope,
     * the parent scope will be scanned for the variable
     * @param variableID - A string ID which the variable will be referenced by
     * @return - A pointer to the specified data if it exists, otherwise a nullptr will return
     */
    int* getVariable(std::string variableID); //TODO: replace with below method

    /**
     * getVariableAsType() returns data from the Scope. If the variable does not exist in this scope, the parent will
     * be scanned for the variable
     * @tparam T - The type to cast the desired data as (e.g. int, std::string, float, etc.)
     * @param variableID - A string ID which the variable will be referenced by
     * @return - A type T pointer to the specified data if it exists, otherwise a nullptr will return
     * @warning - The desired type must be identical to the stored type. If one wishes to retrieve an int as a float, for instance, they must first call getVariableAsType<int>() and cast the result.
     */
     //TODO: if T == std::string...
    template <typename T>
    T* getVariableAsType(std::string variableID) {
        if(!hasDataKey(variableID)) {
            if(_parentScope != nullptr) { return _parentScope->getVariableAsType<T>(variableID); }
            return nullptr;
        }
        decrementUses(variableID);
        try{
            return std::any_cast<T>(_variableData[variableID].get()); //try to get the data as the requested type
        }
        catch(std::exception ex) {
            throw std::bad_any_cast(); //if we've failed, throw an exception
        }
    }

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
     * setVariableOccurrences() adds a key, value pair for variable occurrences in the Commander script
     * @param occurrences - An unsigned int value which determines how many times a variable is used in a script
     */
    void setVariableOccurrences(std::string variableID, unsigned int occurrences);

    /**
     * freeVariableData() destructs the data used by a variable
     * @param variableID - A string variableID to free data from
     * @return - TRUE if the data was destructed (or already destructed), otherwise FALSE is returned
     */
    bool freeVariableData(std::string variableID);

    /**
     * decrementUses() subtracts the number of the variable's occurrences by 1. If the number of occurrences are 0, the
     * value will not update further
     * @param variableID - The variable one wishes to decrement
     */
    void decrementUses(std::string variableID);

    /**
     * hasExpired() returns a boolean value according to whether the number of occurrences of a variable is equal to 0
     * @param variableID - The variable to check the garbage collection status of
     * @return - TRUE if the variable has no more occurrences, otherwise FALSE is returned
     */
    bool hasExpired(std::string variableID);

private:
    std::map<std::string, std::shared_ptr<std::any>> _variableData {};  // uses a Key variableName to find it's associated object
    std::map<std::string, unsigned int> _variableUses; //uses a Key variableName to find usages left (unsigned int)
    Scope* _parentScope = nullptr;  // Pointer to the parent scope object (i.e. this scope exists within another scope)


    /**
     * hasDataKey() returns a boolean value according to whether _variableData has an entry of the specified name
     * @param key - A string value representing a variable ID
     * @return - TRUE if the variable exists in _variableData, otherwise FALSE is returned
     */
    bool hasDataKey(std::string key);

    /**
     * hasUsesKey() returns a boolean value according to whether _variableUses has an entry of the specified name
     * @param key - A string value representing a variable ID
     * @return - TRUE if the variable exists in _variableUses, otherwise FALSE is returned
     */
    bool hasUsesKey(std::string key);

    /**
     * tryGetUses() is a helper method for getting the number of variable uses. If a value does not exist, a new one
     * will be created with a value of 0
     * @param variableID - The variable one wishes to find or initialize
     * @return - TRUE if the method created a new entry in _variableUses, otherwise FALSE is returned
     */
    bool tryGetUses(std::string variableID);

};


#endif  // CPP_UTILITIES_SCOPE_HPP
