/**
 * Scope.hpp contains the prototypes for the Scope class. These prototypes contain basic data and methods, but no implementations
 */

#ifndef CPP_UTILITIES_SCOPE_HPP
#define CPP_UTILITIES_SCOPE_HPP
#include <string>
#include <map>

//TODO: replace data with a generic object class (template class?)

class Scope {
public:
    /**
     * Scope() serves as a default constructor for the scope object. Its parent will default to nullptr
     */
    Scope();

    /**
     * Scope(Scope* parent) is a constructor which specifies the parent scope object.
     * @param parent - A pointer to the parent of the scope
     */
    Scope(Scope* parent);

    /**
     * ~Scope() serves as a destructor for the Scope object
     */
    ~Scope();

    /**
     * Not to be confused with Scope(Scope* parent), this method serves as a copy constructor
     * @param otherScope - The scope one wishes to copy
     */
    Scope(Scope &otherScope);

    /**
     * addOrUpdateVariable will attempt to add the relevant data to the scope.
     * @param variableID - A String ID which the variable will be referenced by (e.g. "cat")
     * @param data - An object which will be stored as data (e.g. 14, "dog", std::vector<int>, etc.)
     */
    void addOrUpdateVariable(std::string variableID, int data);

    /**
     * hasVariable returns a boolean value according to whether the specified variable exists in this scope.
     * For scanning all scopes for a variable, use getVariable()
     * @param variableID - A string ID which the variable will be referenced by
     * @return - TRUE if the variable exists in the current scope, otherwise FALSE is returned
     */
    bool hasVariable(std::string variableID);

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
     * isGlobal() will return a value according to whether the Scope has a parent
     * @return - FALSE if the scope has a parent, TRUE if the scope returns a nullptr
     */
    bool isGlobal();

private:
    std::map<std::string, int> variableData{}; //uses a Key variableName to find it's associated object
    Scope* parentScope = nullptr; //Pointer to the parent scope object (i.e. this scope exists within another scope)


    /**
     * hasKey() returns a boolean value according to whether variableData has an entry of the specified name
     * @param key - A string value representing a variable ID
     * @return - TRUE if the variable exists in variableData, otherwise FALSE is returned
     */
    bool hasKey(std::string key);
};


#endif //CPP_UTILITIES_SCOPE_HPP
