/**
 * @file type_checker.hpp
 * @brief type_checker.hpp contains the prototypes for methods in the type_checker class
 * @details For an implementation, see type_checker
 */

#ifndef CPP_UTILITIES_TYPE_CHECKER_HPP
#define CPP_UTILITIES_TYPE_CHECKER_HPP
#include "source/parser/parser.hpp"
#include "source/symbol_table/symbol_table_organizer.hpp"
#include "source/type_checker/type.hpp"
#include "source/util/commander_exception.hpp"
#include "type.hpp"
#include <string>
#include <unordered_map>

namespace TypeChecker {
    /**
     * @brief Class for type checking
     *
     */
    class TypeChecker {
    public:
        TypeChecker() = default;   // Default Constructor
        ~TypeChecker() = default;  // Default Destructor

        /**
         * TypeChecker(otherTypeChecker) is a copy-constructor for the TypeChecker class.
         * @param otherTypeChecker - The TypeChecker object to copy from
         */
        TypeChecker(TypeChecker* otherTypeChecker);

        /**
         * @brief Type checks (recursively) a list of AST nodes
         * @param astNodeList The list of AST nodes
         */
        void typeCheck(const Parser::ASTNodeList& astNodeList);

        /**
         * @brief Type checks (recursively) a single AST node
         * @param astNode The ast node pointer
         * @return The type of the ast node if it evaluates to a type, otherwise returns nullptr
         */
        TyPtr typeCheck(const Parser::ASTNodePtr& astNode);

        /**
         * getType() will attempt to return the variable type if present
         * @param variableID - The desired variableID to obtain
         * @return - A string type if the variable exists, otherwise nullptr is returned
         */
        TyPtr& getType(const std::string& variableID);  // Returns the type of a variable

        /**
         * setOrUpdateType() will attempt to assign a new variable to the current type_checker or update the current
         * variable
         * @param variableID - The desired variableID to update or add
         * @param type - A string Type to associate the variable with
         */
        void setOrUpdateType(const std::string& variableID,
                             const TyPtr& type);  // Sets or updates the type of a variable

        /**
         * verifyType() will return a boolean if the variableID type is compatable with the expected type. This occurs
         * in the following situations:
         * - the expected type and variable type are equal
         * - the expected type and variable type can be converted to one another (e.g. Int to Float)
         * @param variableID - The variable one wishes to verify
         * @param expected - The expected outcome for a variable
         * @return - TRUE if a compatible type exists, otherwise FALSE is returned
         */
        bool verifyType(const std::string& variableID,
                        const TyPtr& expected);  // returns TRUE if the expected is equal to the
                                                 // actual type or can be used as the expected type

        /**
         * hasVariable() returns a boolean value according to whether the specified variable is present in the
         * type_checker
         * @param variableID - The desired variable to locate
         * @return TRUE if the variable exists, otherwise FALSE is returned
         */
        bool hasVariable(
                const std::string& variableID);  // returns a boolean according to whether a specified variable exists

    private:
        std::unordered_map<std::string, TyPtr>
                _assignedTypes {};  // A dictionary which stores a string id and returns a string type

        /**
         * isVariantOfExpected is a helper method which determines if the provided type is compatible with the expected
         * type
         * @param variableType - The type we would like to convert (e.g. INTEGER)
         * @param expectedType - The type we would like to check compatability with (e.g. FLOAT)
         * @return - A boolean value according to whether variableType is compatible with expectedType
         */
        static bool _isVariantOfExpected(const TyPtr& variableType, const TyPtr& expectedType);
    };
}  //  namespace TypeChecker

#endif  // CPP_UTILITIES_TYPE_CHECKER_HPP
