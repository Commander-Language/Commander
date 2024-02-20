/**
 * @file shared.hpp
 * @details contains common types, constants, etc. used in builtins
 */
#ifndef SHARED_BUILTIN_HPP
#define SHARED_BUILTIN_HPP

#include <string>
#include <tuple>
#include <vector>

/**
 * @brief A list of arguments for builtins
 */
using List = std::vector<std::string>;
/**
 * @brief Structure of return information for builtins
 */
using ReturnInfo = std::tuple<std::string, std::string, int>;
/**
 * @brief Return code for successful call
 */
const int SUCCESS = 0;
/**
 * @brief Return code for call with general errors
 */
const int FAIL = 1;
/**
 * @brief Return code for extreme errors
 */
const int ERROR = 2;

#endif