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

/* ========== Type Maker ========== */

/**
 * Returns a function type with zero param types and the return type as the argument type
 * @param arg The argument type that is used for the return type
 * @return The function type
 */
TypeChecker::FunctionTyPtr getFunctionTy(TypeChecker::TyPtr arg) {
    std::vector<TypeChecker::TyPtr> params;
    return std::make_shared<TypeChecker::FunctionTy>(params, arg);
}

/**
 * Returns a function type with the first n - 1 arguments as param types and the return type as the nth argument type
 * @tparam Args The type of the param arguments (should be TyPtr); required for variadic functions
 * @param first The first argument
 * @param args The rest of the arguments
 * @return The function type
 */
template<typename... Args>
TypeChecker::FunctionTyPtr getFunctionTy(TypeChecker::TyPtr first, Args... args) {
    std::vector<TypeChecker::TyPtr> params;
    params.push_back(first);
    auto push_args = [&](auto&&... xs) {
        (params.push_back(std::forward<decltype(xs)>(xs)), ...);
    };
    (push_args(args), ...);
    TypeChecker::TyPtr returnTy = params.back();
    params.pop_back();
    return std::make_shared<TypeChecker::FunctionTy>(params, returnTy);
}

/* ========== Types ========== */
const TypeChecker::TupleTyPtr VOID = std::make_shared<TypeChecker::TupleTy>(std::vector<TypeChecker::TyPtr>{});
const TypeChecker::IntTyPtr INT = std::make_shared<TypeChecker::IntTy>();
const TypeChecker::FloatTyPtr FLOAT = std::make_shared<TypeChecker::FloatTy>();
const TypeChecker::BoolTyPtr BOOL = std::make_shared<TypeChecker::BoolTy>();
const TypeChecker::StringTyPtr STRING = std::make_shared<TypeChecker::StringTy>();

/**
 * The built in function types
 */
const std::vector<std::pair<std::string, std::shared_ptr<TypeChecker::FunctionTy>>> functionTypes {
        {"parseInt", getFunctionTy(INT, INT)},
        {"parseInt", getFunctionTy(FLOAT, INT)},
        {"parseInt", getFunctionTy(BOOL, INT)},
        {"parseInt", getFunctionTy(STRING, INT)},
        {"parseFloat", getFunctionTy(INT, FLOAT)},
        {"parseFloat", getFunctionTy(FLOAT, FLOAT)},
        {"parseFloat", getFunctionTy(BOOL, FLOAT)},
        {"parseFloat", getFunctionTy(STRING, FLOAT)},
        {"parseBool", getFunctionTy(INT, BOOL)},
        {"parseBool", getFunctionTy(FLOAT, BOOL)},
        {"parseBool", getFunctionTy(BOOL, BOOL)},
        {"parseBool", getFunctionTy(STRING, BOOL)}
};

/* ========== Implementations ========== */
int64_t parseIntAsInt(int64_t number);

int64_t parseFloatAsInt(double number);

int64_t parseBoolAsInt(bool value);

int64_t parseStringAsInt(std::string string);

double parseIntAsFloat(int64_t number);

double parseFloatAsFloat(double number);

double parseBoolAsFloat(bool value);

double parseStringAsFloat(std::string string);

bool parseIntAsBool(int64_t number);

bool parseFloatAsBool(double number);

bool parseBoolAsBool(bool value);

bool parseStringAsBool(std::string string);

#endif  // COMMANDER_FUNCTIONS_HPP