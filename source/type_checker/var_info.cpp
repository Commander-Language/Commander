/**
 * @file var_info.cpp
 * @brief var_info.cpp contains implementation of var_info.hpp
 */

#include "var_info.hpp"

#include <utility>

namespace TypeChecker {

    InfoType VariableInfo::infoType() const { return InfoType::VARIABLE_INFO; }

    InfoType FunctionInfo::infoType() const { return InfoType::FUNCTION_INFO; }

    InfoType TypeInfo::infoType() const { return InfoType::TYPE_INFO; }

    InfoType AliasInfo::infoType() const { return InfoType::ALIAS_INFO; }

    VarInfo::VarInfo(bool constant, const std::vector<TyPtr>& types) : constant(constant), types(types) {}

    VariableInfo::VariableInfo(bool constant, TyPtr type) : VarInfo(constant, std::vector<TyPtr> {std::move(type)}) {}

    FunctionInfo::FunctionInfo(const std::vector<TyPtr>& types) : VarInfo(true, types) {}

    TypeInfo::TypeInfo(TyPtr type) : VarInfo(true, std::vector<TyPtr> {std::move(type)}) {}

    AliasInfo::AliasInfo() : VarInfo(true, std::vector<TyPtr> {COMMAND_TY}) {}

}  //  namespace TypeChecker