/**
* @file functions.hpp
* @brief Contains header content for functions.cpp, which contains all built in functions
*
*/

#ifndef COMMANDER_FUNCTIONS_HPP
#define COMMANDER_FUNCTIONS_HPP

#include <vector>
#include <string>
#include "source/type_checker/type.hpp"

/**
 * The functions for
 */
const std::vector<std::pair<std::string, std::shared_ptr<TypeChecker::FunctionTy>>> functionTypes(
    {"parseInt", std::make_shared<TypeChecker::FunctionTy>({std::make_shared<TypeChecker::IntTy>()},std::make_shared<TypeChecker::IntTy>())}
);

#endif  // COMMANDER_FUNCTIONS_HPP