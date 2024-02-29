/**
 * @file variable_table.cpp
 * @brief variable_table.cpp contains implementation for variable_table.hpp
 */

#include "variable_table.hpp"

#include <utility>

namespace TypeChecker {

    VariableTable::VariableTable(std::shared_ptr<VariableTable> parent) : _parent(std::move(parent)) {}

    bool VariableTable::hasVariable(const std::string& variableID) {
        return _variables.find(variableID) != _variables.end() || _parent && _parent->hasVariable(variableID);
    }

    void VariableTable::addVariable(const std::string& variableID, VarInfoPtr variable) {
        _variables[variableID] = std::move(variable);
    }

    VarInfoPtr VariableTable::getVariable(const std::string& variableID) {
        return _variables.find(variableID) != _variables.end() ? _variables[variableID]
                                                               : (_parent ? _parent->getVariable(variableID) : nullptr);
    }

    std::shared_ptr<VariableTable> VariableTable::getParent() { return _parent; }

}  //  namespace TypeChecker