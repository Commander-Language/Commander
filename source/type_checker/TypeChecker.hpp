/**
 * TypeChecker.hpp contains the prototypes for methods in the TypeChecker class.
 * For an implementation, see TypeChecker.cpp
 */

#ifndef CPP_UTILITIES_TYPECHECKER_HPP
#define CPP_UTILITIES_TYPECHECKER_HPP
#include <string>
#include <map>


class TypeChecker {
public:
    TypeChecker(); //Default Constructor
    ~TypeChecker(); //Destructor
    TypeChecker(TypeChecker *otherTypeChecker); //Copy-Constructor

    std::string getType(std::string variableID); //Returns the type of a variable
    void setOrUpdateType(std::string variableID, std::string type); //Sets or updates the type of a variable
    bool verifyType(std::string variableID, std::string expected); //returns TRUE if the expected is equal to the actual type or can be used as the expected type

private:
    std::map<std::string, std::string> assignedTypes{}; //A dictionary which stores a string id and returns a string type

};


#endif //CPP_UTILITIES_TYPECHECKER_HPP
