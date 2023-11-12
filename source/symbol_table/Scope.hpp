/**
 * Scope.hpp contains the prototypes for the Scope class. These prototypes contain basic data and methods, but no implementations
 */

#ifndef CPP_UTILITIES_SCOPE_HPP
#define CPP_UTILITIES_SCOPE_HPP
#include <string>


class Scope {
public:
    Scope(); //Constructor - will set parent to NULLPTR
    Scope(Scope *parent); //Constructor
    ~Scope(); //Destructor
    Scope(Scope &otherScope); //Copy Constructor
    void addOrUpdateVariable(std::string variableID, Object data); //Set or update the current variable
    bool hasVariable(std::string variableID); //returns TRUE if the specified variable exists in this scope
    *Object getVariable(std::string variableID); //returns a pointer to the specified variable. If not in this scope, we'll traverse up the scope stack
    *Scope getParentScopePointer(); //returns a pointer to the parent scope or NULLPTR
    bool isGlobal(); //A boolean which determines whether this is the head of all scopes

private:
    std::map<std::string, object> variableData{}; //uses a Key variableName to find it's associated object
    *Scope parentScope{}; //Pointer to the parent scope object (i.e. this scope exists within another scope)
};


#endif //CPP_UTILITIES_SCOPE_HPP
