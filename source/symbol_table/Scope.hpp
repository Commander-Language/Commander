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
    Scope(); //Constructor - will set parent to NULLPTR
    Scope(Scope* parent); //Constructor
    ~Scope(); //Destructor
    Scope(Scope &otherScope); //Copy Constructor
    void addOrUpdateVariable(std::string variableID, int data); //Set or update the current variable
    bool hasVariable(std::string variableID); //returns TRUE if the specified variable exists in this scope
    int* getVariable(std::string variableID); //returns a pointer to the specified variable. If not in this scope, we'll traverse up the scope stack
    Scope* getParentScopePointer(); //returns a pointer to the parent scope or NULLPTR
    bool isGlobal(); //A boolean which determines whether this is the head of all scopes

private:
    std::map<std::string, int> variableData{}; //uses a Key variableName to find it's associated object
    Scope* parentScope = nullptr; //Pointer to the parent scope object (i.e. this scope exists within another scope)

    bool hasKey(std::string key); //Helper method to find if variableData has a given key (map.contains available in C++20+)
};


#endif //CPP_UTILITIES_SCOPE_HPP
