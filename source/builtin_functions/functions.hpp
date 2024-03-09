/**
 * @file functions.hpp
 * @brief Contains header content for functions.cpp, which contains all built in functions
 */

#ifndef COMMANDER_FUNCTIONS_HPP
#define COMMANDER_FUNCTIONS_HPP

#include "source/flow_controller/operations.hpp"
#include "source/flow_controller/types.hpp"
#include "source/type_checker/type.hpp"
#include <chrono>
#include <cmath>
#include <functional>
#include <string>
#include <thread>
#include <vector>

namespace Function {

    /**
     * Value to return for functions that have void return types
     */
    FlowController::CommanderTuplePtr VOID = std::make_shared<FlowController::CommanderTuple>(
            std::vector<FlowController::CommanderTypePtr> {});

    /* ========== Type Maker ========== */

    /**
     * Returns a function type with zero param types and the return type as the argument type
     * @param arg The argument type that is used for the return type
     * @return The function type
     */
    TypeChecker::FunctionTyPtr getFunctionTy(TypeChecker::TyPtr arg);

    /**
     * Returns a function type with the first n - 1 arguments as param types and the return type as the nth argument
     * type
     * @tparam Args The type of the param arguments (should be TyPtr); required for variadic functions
     * @param first The first argument
     * @param args The rest of the arguments
     * @return The function type
     */
    template<typename... Args>
    TypeChecker::FunctionTyPtr getFunctionTy(TypeChecker::TyPtr first, Args... args) {
        std::vector<TypeChecker::TyPtr> params;
        params.push_back(first);
        auto pushArgs = [&](auto&&... arg) { (params.push_back(std::forward<decltype(arg)>(arg)), ...); };
        (pushArgs(args), ...);
        const TypeChecker::TyPtr returnTy = params.back();
        params.pop_back();
        return std::make_shared<TypeChecker::FunctionTy>(params, returnTy);
    }

    /**
     * The built in function types
     */
    const std::unordered_map<std::string, std::vector<TypeChecker::FunctionTyPtr>> functionTypes {
            {"parseInt",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::INT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::INT_TY),
                                                      getFunctionTy(TypeChecker::BOOL_TY, TypeChecker::INT_TY),
                                                      getFunctionTy(TypeChecker::STRING_TY, TypeChecker::INT_TY)}},
            {"parseFloat",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::BOOL_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::STRING_TY, TypeChecker::FLOAT_TY)}},
            {"parseBool",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::BOOL_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::BOOL_TY),
                                                      getFunctionTy(TypeChecker::BOOL_TY, TypeChecker::BOOL_TY),
                                                      getFunctionTy(TypeChecker::STRING_TY, TypeChecker::BOOL_TY)}},
            {"toString",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::ANY_TY, TypeChecker::STRING_TY)}},
            {"sqrt",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"ln",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"log",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"abs",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"floor",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"ceil",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"round",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"sin",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"cos",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"tan",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"csc",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"sec",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"cot",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"sinh",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"cosh",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"tanh",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"csch",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"sech",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"coth",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"arcsin",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"arccos",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"arctan",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"arccsc",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"arcsec",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"arccot",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"arcsinh",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"arccosh",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"arctanh",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"arccsch",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"arcsech",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"arccoth",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::FLOAT_TY),
                                                      getFunctionTy(TypeChecker::FLOAT_TY, TypeChecker::FLOAT_TY)}},
            {"random", std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::FLOAT_TY)}},
            {"time", std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY)}},
            {"date", std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(std::make_shared<TypeChecker::TupleTy>(
                             std::vector<TypeChecker::TyPtr> {TypeChecker::INT_TY, TypeChecker::INT_TY,
                                                              TypeChecker::INT_TY}))}},
            {"sleep",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::INT_TY, TypeChecker::VOID_TY)}},
            {"charAt", std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(
                               TypeChecker::STRING_TY, TypeChecker::INT_TY, TypeChecker::STRING_TY)}},
            {"startsWith", std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(
                                   TypeChecker::STRING_TY, TypeChecker::STRING_TY, TypeChecker::BOOL_TY)}},
            {"endsWith", std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(
                                 TypeChecker::STRING_TY, TypeChecker::STRING_TY, TypeChecker::BOOL_TY)}},
            {"includes",
             std::vector<TypeChecker::FunctionTyPtr> {
                     getFunctionTy(TypeChecker::STRING_TY, TypeChecker::STRING_TY, TypeChecker::BOOL_TY),
                     getFunctionTy(TypeChecker::TUPLE_TY, TypeChecker::ANY_TY, TypeChecker::BOOL_TY),
                     getFunctionTy(TypeChecker::ARRAY_TY, TypeChecker::ANY_TY, TypeChecker::BOOL_TY)}},
            {"indexOf",
             std::vector<TypeChecker::FunctionTyPtr> {
                     getFunctionTy(TypeChecker::STRING_TY, TypeChecker::STRING_TY, TypeChecker::INT_TY),
                     getFunctionTy(TypeChecker::TUPLE_TY, TypeChecker::ANY_TY, TypeChecker::INT_TY),
                     getFunctionTy(TypeChecker::ARRAY_TY, TypeChecker::ANY_TY, TypeChecker::INT_TY)}},
            {"length",
             std::vector<TypeChecker::FunctionTyPtr> {
                     getFunctionTy(TypeChecker::STRING_TY, TypeChecker::INT_TY),
                     getFunctionTy(TypeChecker::TUPLE_TY, TypeChecker::INT_TY),
                     getFunctionTy(TypeChecker::ARRAY_TY, TypeChecker::INT_TY),
             }},
            {"replace",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::STRING_TY, TypeChecker::STRING_TY,
                                                                    TypeChecker::STRING_TY, TypeChecker::STRING_TY)}},
            {"replaceAll",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::STRING_TY, TypeChecker::STRING_TY,
                                                                    TypeChecker::STRING_TY, TypeChecker::STRING_TY)}},
            {"substring",
             std::vector<TypeChecker::FunctionTyPtr> {
                     getFunctionTy(TypeChecker::STRING_TY, TypeChecker::INT_TY, TypeChecker::STRING_TY),
                     getFunctionTy(TypeChecker::STRING_TY, TypeChecker::INT_TY, TypeChecker::INT_TY,
                                   TypeChecker::STRING_TY)}},
            {"trim",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::STRING_TY, TypeChecker::STRING_TY)}},
            {"lower",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::STRING_TY, TypeChecker::STRING_TY)}},
            {"upper",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::STRING_TY, TypeChecker::STRING_TY)}},
            {"split", std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(
                              TypeChecker::STRING_TY, TypeChecker::STRING_TY,
                              std::make_shared<TypeChecker::ArrayTy>(TypeChecker::STRING_TY))}},
            {"sort", std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(
                             TypeChecker::ARRAY_TY, TypeChecker::FUNCTION_TY, TypeChecker::ARRAY_TY)}},
            {"filter", std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(
                               TypeChecker::ARRAY_TY, TypeChecker::FUNCTION_TY, TypeChecker::ARRAY_TY)}},
            {"map", std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(
                            TypeChecker::ARRAY_TY, TypeChecker::FUNCTION_TY, TypeChecker::ARRAY_TY)}},
            {"foreach", std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(
                                TypeChecker::ARRAY_TY, TypeChecker::FUNCTION_TY, TypeChecker::VOID_TY)}},
            {"append", std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(
                               TypeChecker::ARRAY_TY, TypeChecker::ANY_TY, TypeChecker::VOID_TY)}},
            {"remove", std::vector<TypeChecker::FunctionTyPtr> {
                               getFunctionTy(TypeChecker::ARRAY_TY, TypeChecker::ANY_TY, TypeChecker::VOID_TY)}}};

    /* ========== Implementations ========== */

    FlowController::CommanderIntPtr parseInt(FlowController::CommanderTypePtr intValue);

    FlowController::CommanderFloatPtr parseFloat(FlowController::CommanderTypePtr floatValue);

    FlowController::CommanderBoolPtr parseBool(FlowController::CommanderTypePtr boolValue);

    FlowController::CommanderStringPtr toString(FlowController::CommanderTypePtr value);

    FlowController::CommanderFloatPtr sqrt(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr ln(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr log(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr abs(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderIntPtr floor(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderIntPtr ceil(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderIntPtr round(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr sin(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr cos(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr tan(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr csc(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr sec(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr cot(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr sinh(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr cosh(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr tanh(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr csch(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr sech(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr coth(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr arcsin(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr arccos(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr arctan(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr arccsc(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr arcsec(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr arccot(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr arcsinh(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr arccosh(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr arctanh(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr arccsch(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr arcsech(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr arccoth(FlowController::CommanderTypePtr numberValue);

    FlowController::CommanderFloatPtr randomFloat();

    FlowController::CommanderIntPtr time();

    FlowController::CommanderTuplePtr date();

    FlowController::CommanderTuplePtr sleep(FlowController::CommanderIntPtr timeToSleep);

    FlowController::CommanderStringPtr charAt(FlowController::CommanderStringPtr sourceString,
                                              FlowController::CommanderIntPtr index);

    FlowController::CommanderBoolPtr startsWith(FlowController::CommanderStringPtr sourceString,
                                                FlowController::CommanderStringPtr expected);

    FlowController::CommanderBoolPtr endsWith(FlowController::CommanderStringPtr sourceString,
                                              FlowController::CommanderStringPtr expected);

    FlowController::CommanderIntPtr length(FlowController::CommanderTypePtr source);

    FlowController::CommanderStringPtr replace(FlowController::CommanderStringPtr sourceString,
                                               FlowController::CommanderStringPtr oldPhrase,
                                               FlowController::CommanderStringPtr newPhrase);

    FlowController::CommanderStringPtr replaceAll(FlowController::CommanderStringPtr sourceString,
                                                  FlowController::CommanderStringPtr oldPhrase,
                                                  FlowController::CommanderStringPtr newPhrase);

    FlowController::CommanderStringPtr substring(FlowController::CommanderStringPtr sourceString,
                                                 FlowController::CommanderIntPtr startingIndex);

    FlowController::CommanderStringPtr substring(FlowController::CommanderStringPtr sourceString,
                                                 FlowController::CommanderIntPtr startingIndex,
                                                 FlowController::CommanderIntPtr endingIndex);

    FlowController::CommanderStringPtr trim(FlowController::CommanderStringPtr sourceString);

    FlowController::CommanderStringPtr lower(FlowController::CommanderStringPtr sourceString);

    FlowController::CommanderStringPtr upper(FlowController::CommanderStringPtr sourceString);

    FlowController::CommanderArrayPtr split(FlowController::CommanderStringPtr sourceString,
                                            FlowController::CommanderStringPtr splitToken);

    FlowController::CommanderIntPtr indexOf(FlowController::CommanderTypePtr obj,
                                            FlowController::CommanderTypePtr data);

    FlowController::CommanderBoolPtr includes(FlowController::CommanderTypePtr obj,
                                              FlowController::CommanderTypePtr data);

    FlowController::CommanderArrayPtr sort(FlowController::CommanderArrayPtr array,
                                           FlowController::CommanderLambdaPtr function);

    FlowController::CommanderArrayPtr filter(FlowController::CommanderArrayPtr array,
                                             FlowController::CommanderLambdaPtr function);

    FlowController::CommanderArrayPtr map(FlowController::CommanderArrayPtr array,
                                          FlowController::CommanderLambdaPtr function);

    FlowController::CommanderTuplePtr foreach(FlowController::CommanderArrayPtr array,
                                              FlowController::CommanderLambdaPtr function);

    FlowController::CommanderTuplePtr append(FlowController::CommanderArrayPtr array,
                                             FlowController::CommanderTypePtr data);

    FlowController::CommanderTuplePtr remove(FlowController::CommanderArrayPtr array,
                                             FlowController::CommanderTypePtr data);

    /**
     * parseAsType() allows the user to parse the specified data as a specified type.
     * @tparam T - The type of the original data
     * @tparam U - The type of the data to parse as
     * @param originalVal - The unmodified data to parse as type U
     * @return - The original data parsed as type U (e.g. parseAsType<int, bool>(36) will convert 36 from an int to a
     * boolean value)
     */
    template<typename T, typename U>
    U parseAsType(T originalVal) {
        return (U)originalVal;  // TODO: exceptions when string is passed in (or redirect to parseAsString /
                                // parseStringAsType)
    }

    template<typename T>
    std::string parseAsString(T originalVal) {
        if (typeid(T) == typeid(bool)) {
            if (((FlowController::CommanderInt)originalVal).value > 0) { return "true"; }
            return "false";
        }
        return std::to_string(originalVal);
    }
}  // namespace Function

#endif  // COMMANDER_FUNCTIONS_HPP