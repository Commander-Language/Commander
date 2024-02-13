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
const std::vector<std::pair<std::string, std::shared_ptr<TypeChecker::FunctionTy>>> functionTypes { //TODO: "parseInt/Float/Bool" boolean values
        {"parseInt", std::make_shared<TypeChecker::FunctionTy>(
                             std::vector<std::shared_ptr<TypeChecker::Ty>> {std::make_shared<TypeChecker::IntTy>()},
                             std::make_shared<TypeChecker::IntTy>())},

        {"parseInt", std::make_shared<TypeChecker::FunctionTy>(
                             std::vector<std::shared_ptr<TypeChecker::Ty>> {std::make_shared<TypeChecker::FloatTy>()},
                             std::make_shared<TypeChecker::IntTy>())},

        {"parseInt", std::make_shared<TypeChecker::FunctionTy>(
                             std::vector<std::shared_ptr<TypeChecker::Ty>> {std::make_shared<TypeChecker::StringTy>()},
                             std::make_shared<TypeChecker::IntTy>())},

        {"parseInt", std::make_shared<TypeChecker::FunctionTy>(
                             std::vector<std::shared_ptr<TypeChecker::Ty>> {std::make_shared<TypeChecker::BoolTy>()},
                             std::make_shared<TypeChecker::IntTy>())},

        {"parseFloat", std::make_shared<TypeChecker::FunctionTy>(
                             std::vector<std::shared_ptr<TypeChecker::Ty>> {std::make_shared<TypeChecker::IntTy>()},
                             std::make_shared<TypeChecker::FloatTy>())},

        {"parseFloat", std::make_shared<TypeChecker::FunctionTy>(
                             std::vector<std::shared_ptr<TypeChecker::Ty>> {std::make_shared<TypeChecker::FloatTy>()},
                             std::make_shared<TypeChecker::FloatTy>())},

        {"parseFloat", std::make_shared<TypeChecker::FunctionTy>(
                             std::vector<std::shared_ptr<TypeChecker::Ty>> {std::make_shared<TypeChecker::StringTy>()},
                             std::make_shared<TypeChecker::FloatTy>())},

        {"parseFloat", std::make_shared<TypeChecker::FunctionTy>(
                               std::vector<std::shared_ptr<TypeChecker::Ty>> {std::make_shared<TypeChecker::BoolTy>()},
                              std::make_shared<TypeChecker::FloatTy>())},

        {"parseBool", std::make_shared<TypeChecker::FunctionTy>(
                               std::vector<std::shared_ptr<TypeChecker::Ty>> {std::make_shared<TypeChecker::IntTy>()},
                               std::make_shared<TypeChecker::BoolTy>())},

        {"parseBool", std::make_shared<TypeChecker::FunctionTy>(
                               std::vector<std::shared_ptr<TypeChecker::Ty>> {std::make_shared<TypeChecker::FloatTy>()},
                               std::make_shared<TypeChecker::BoolTy>())},

        {"parseBool", std::make_shared<TypeChecker::FunctionTy>(
                               std::vector<std::shared_ptr<TypeChecker::Ty>> {std::make_shared<TypeChecker::StringTy>()},
                               std::make_shared<TypeChecker::BoolTy>())},

        {"parseBool", std::make_shared<TypeChecker::FunctionTy>(
                             std::vector<std::shared_ptr<TypeChecker::Ty>> {std::make_shared<TypeChecker::BoolTy>()},
                             std::make_shared<TypeChecker::BoolTy>())}
};

int64_t parseIntAsInt(int64_t number);

int64_t parseFloatAsInt(double number);

int64_t parseStringAsInt(std::string string);

int64_t parseBoolAsInt(bool value);

double parseIntAsFloat(int64_t number);

double parseFloatAsFloat(double number);

double parseStringAsFloat(std::string string);

double parseBoolAsFloat(bool value);

bool parseIntAsBool(int64_t number);

bool parseFloatAsBool(double number);

bool parseStringAsBool(std::string string);

bool parseBoolAsBool(bool value);

#endif  // COMMANDER_FUNCTIONS_HPP