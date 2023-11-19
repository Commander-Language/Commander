/**
 * TypeChecker.hpp contains the prototypes for methods in the TypeChecker class.
 * For an implementation, see TypeChecker.cpp
 */

#ifndef CPP_UTILITIES_TYPECHECKER_HPP
#define CPP_UTILITIES_TYPECHECKER_HPP
#include <string>


class TypeChecker {
public:
    TypeChecker(); //Default Constructor
    ~TypeChecker(); //Destructor
    TypeChecker(TypeChecker *otherTypeChecker); //Copy-Constructor

    std::string getType(std::string variableID); //Returns the type of a variable
    bool verifyType(std::string variableID, std::string expected); //returns TRUE if the expected is equal to the actual type or can be used as the expected type

};


#endif //CPP_UTILITIES_TYPECHECKER_HPP
