/**
 * @file type_checker.hpp
 * @brief type_checker.hpp contains the prototypes for methods in the type_checker class
 * @details For an implementation, see type_checker
 */

#ifndef CPP_UTILITIES_TYPE_CHECKER_HPP
#define CPP_UTILITIES_TYPE_CHECKER_HPP
#include "source/parser/parser.hpp"
#include "source/util/commander_exception.hpp"
#include "type.hpp"
#include "variable_table.hpp"
#include <string>

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
         * Asserts that a variable does not exist in the current scope
         * @param variable The variable to assert does not exist
         */
        void assertNotExists(const std::string& variable);

        /**
         * @brief Pushes new scope on top of the stack
         */
        void pushScope();

        /**
         * @brief Pops top scope of variables off the stack
         */
        VariableTablePtr popScope();

    private:
        VariableTable _table;
    };
}  //  namespace TypeChecker

#endif  // CPP_UTILITIES_TYPE_CHECKER_HPP
