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

    /**
     * getType() will attempt to return the variable type if present
     * @param variableID - The desired variableID to obtain
     * @return - A string type if the variable exists, otherwise nullptr is returned
     */
    std::string getType(std::string variableID); //Returns the type of a variable

    /**
     * setOrUpdateType() will attempt to assign a new variable to the current TypeChecker or update the current variable
     * @param variableID - The desired variableID to update or add
     * @param type - A string Type to associate the variable with
     */
    void setOrUpdateType(std::string variableID, std::string type); //Sets or updates the type of a variable

    /**
     * verifyType() will return a boolean if the variableID type is compatable with the expected type. This occurs in the following situations:
     * - the expected type and variable type are equal
     * - the expected type and variable type can be converted to one another (e.g. Int to Float)
     * @param variableID - The variable one wishes to verify
     * @param expected - The expected outcome for a variable
     * @return - TRUE if a compatible type exists, otherwise FALSE is returned
     */
    bool verifyType(std::string variableID, std::string expected); //returns TRUE if the expected is equal to the actual type or can be used as the expected type

    /**
     * hasVariable() returns a boolean value according to whether the specified variable is present in the TypeChecker
     * @param variableID - The desired variable to locate
     * @return TRUE if the variable exists, otherwise FALSE is returned
     */
    bool hasVariable(std::string variableID); //returns a boolean according to whether a specified variable exists

private:
    std::map<std::string, std::string> assignedTypes{}; //A dictionary which stores a string id and returns a string type

    /**
     * isVariantOfExpected is a helper method which determines if the provided type is compatible with the expected type
     * @param variableType - The type we would like to convert (e.g. INTEGER)
     * @param expectedType - The type we would like to check compatability with (e.g. FLOAT)
     * @return - A boolean value according to whether variableType is compatible with expectedType
     */
    bool isVariantOfExpected(std::string variableType, std::string expectedType);
};


#endif //CPP_UTILITIES_TYPECHECKER_HPP
