/**
 * @file variable_table.hpp
 * @brief variable_table.hpp contains implementation for a class that stores variable info in scopes
 */

#ifndef VARIABLE_TABLE_HPP
#define VARIABLE_TABLE_HPP

#include "var_info.hpp"
#include <unordered_map>

namespace TypeChecker {

    /**
     * @brief Class for storing variable info in scopes
     *
     */
    class VariableTable {
    private:
        /**
         * @brief The parent scope table
         */
        std::shared_ptr<VariableTable> _parent;

        /**
         * @brief A dictionary which stores a string id and returns the variable info
         */
        std::unordered_map<std::string, VarInfoPtr> _variables;

    public:
        /**
         * @brief Default constructor
         */
        VariableTable() = default;

        /**
         * @brief Constructor for creating new scope
         */
        VariableTable(std::shared_ptr<VariableTable> parent);

        /**
         * @brief Checks if variable exists
         */
        bool hasVariable(const std::string& variableID);

        /**
         * @brief Add variable info to the table
         */
        void addVariable(const std::string& variableID, VarInfoPtr variable);

        /**
         * @brief Gets the variable info in the table
         * @param variableID The variable
         * @return The variable info, or nullptr if it doesn't exist
         */
        VarInfoPtr getVariable(const std::string& variableID);

        /**
         * @brief Gets the parent scope
         */
        std::shared_ptr<VariableTable> getParent();
    };
    using VariableTablePtr = std::shared_ptr<VariableTable>;
}  //  namespace TypeChecker

#endif  // VARIABLE_TABLE_HPP