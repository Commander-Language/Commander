/**
 * @file functions.hpp
 * @brief Contains header content for functions.cpp, which contains all built in functions
 */

#ifndef COMMANDER_FUNCTIONS_HPP
#define COMMANDER_FUNCTIONS_HPP

#include "source/flow_controller/types.hpp"
#include "source/type_checker/type.hpp"
#include <chrono>
#include <cmath>
#include <functional>
#include <string>
#include <thread>
#include <vector>
#include "source/flow_controller/operations.hpp"

namespace Function {
    //TODO: refactor TypeChecker::SomeType to use FlowController::SomeType

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
            {"print",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::ANY_TY, TypeChecker::VOID_TY)}},
            {"toString",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::ANY_TY, TypeChecker::STRING_TY)}},
            {"println",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::ANY_TY, TypeChecker::VOID_TY)}},
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
            {"includes", std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(
                                 TypeChecker::STRING_TY, TypeChecker::STRING_TY, TypeChecker::BOOL_TY),
                                getFunctionTy(TypeChecker::TUPLE_TY, TypeChecker::ANY_TY, TypeChecker::BOOL_TY),
                                getFunctionTy(TypeChecker::ARRAY_TY, TypeChecker::ANY_TY, TypeChecker::BOOL_TY)}},
            {"indexOf", std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(
                                TypeChecker::STRING_TY, TypeChecker::STRING_TY, TypeChecker::INT_TY),
                                getFunctionTy(TypeChecker::TUPLE_TY, TypeChecker::ANY_TY, TypeChecker::INT_TY),
                                getFunctionTy(TypeChecker::ARRAY_TY, TypeChecker::ANY_TY, TypeChecker::INT_TY)}},
            {"length",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::STRING_TY, TypeChecker::INT_TY),
                                                        getFunctionTy(TypeChecker::TUPLE_TY, TypeChecker::INT_TY),
                                                        getFunctionTy(TypeChecker::ARRAY_TY, TypeChecker::INT_TY),}},
            {"replace",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::STRING_TY, TypeChecker::STRING_TY,
                                                                    TypeChecker::STRING_TY, TypeChecker::STRING_TY)}},
            {"replaceAll",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::STRING_TY, TypeChecker::STRING_TY,
                                                                    TypeChecker::STRING_TY, TypeChecker::STRING_TY)}},
            {"substring", std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(
                                  TypeChecker::STRING_TY, TypeChecker::INT_TY, TypeChecker::STRING_TY)}},
            {"substring",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::STRING_TY, TypeChecker::INT_TY,
                                                                    TypeChecker::INT_TY, TypeChecker::STRING_TY)}},
            {"trim",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::STRING_TY, TypeChecker::STRING_TY)}},
            {"lower",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::STRING_TY, TypeChecker::STRING_TY)}},
            {"upper",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::STRING_TY, TypeChecker::STRING_TY)}},
            {"split", std::vector<TypeChecker::FunctionTyPtr> {
                              getFunctionTy(TypeChecker::STRING_TY, TypeChecker::STRING_TY,
                                            std::make_shared<TypeChecker::ArrayTy>(TypeChecker::STRING_TY))}},
            {"sort", std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::ARRAY_TY, TypeChecker::FUNCTION_TY, TypeChecker::ARRAY_TY)}},
            {"filter", std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::ARRAY_TY, TypeChecker::FUNCTION_TY, TypeChecker::ARRAY_TY)}},
            {"map", std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::ARRAY_TY, TypeChecker::FUNCTION_TY, TypeChecker::ARRAY_TY)}},
            {"foreach", std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::ARRAY_TY, TypeChecker::FUNCTION_TY, TypeChecker::VOID_TY)}}};

    /* ========== Implementations ========== */

    FlowController::CommanderInt parseIntAsInt(FlowController::CommanderInt number);

    FlowController::CommanderInt parseFloatAsInt(FlowController::CommanderFloat number);

    FlowController::CommanderInt parseBoolAsInt(FlowController::CommanderBool value);

    FlowController::CommanderInt parseStringAsInt(FlowController::CommanderString string);

    FlowController::CommanderFloat parseIntAsFloat(FlowController::CommanderInt number);

    FlowController::CommanderFloat parseFloatAsFloat(FlowController::CommanderFloat number);

    FlowController::CommanderFloat parseBoolAsFloat(FlowController::CommanderBool value);

    FlowController::CommanderFloat parseStringAsFloat(FlowController::CommanderString string);

    FlowController::CommanderBool parseIntAsBool(FlowController::CommanderInt number);

    FlowController::CommanderBool parseFloatAsBool(FlowController::CommanderFloat number);

    FlowController::CommanderBool parseBoolAsBool(FlowController::CommanderBool value);

    FlowController::CommanderBool parseStringAsBool(FlowController::CommanderString string);

    void println(FlowController::CommanderString string);

    void print(FlowController::CommanderString string);

    FlowController::CommanderString toString(FlowController::CommanderInt value);

    FlowController::CommanderFloat sqrt(FlowController::CommanderInt value);

    FlowController::CommanderFloat sqrt(FlowController::CommanderFloat value);

    FlowController::CommanderFloat ln(FlowController::CommanderInt value);

    FlowController::CommanderFloat ln(FlowController::CommanderFloat value);

    FlowController::CommanderFloat log(FlowController::CommanderInt value);

    FlowController::CommanderFloat log(FlowController::CommanderFloat value);

    FlowController::CommanderInt abs(FlowController::CommanderInt value);

    FlowController::CommanderFloat abs(FlowController::CommanderFloat value);

    FlowController::CommanderInt floor(FlowController::CommanderInt value);

    FlowController::CommanderInt floor(FlowController::CommanderFloat value);

    FlowController::CommanderInt ceil(FlowController::CommanderInt value);

    FlowController::CommanderInt ceil(FlowController::CommanderFloat value);

    FlowController::CommanderInt round(FlowController::CommanderInt value);

    FlowController::CommanderInt round(FlowController::CommanderFloat value);

    FlowController::CommanderFloat sin(FlowController::CommanderInt value);

    FlowController::CommanderFloat sin(FlowController::CommanderFloat value);

    FlowController::CommanderFloat cos(FlowController::CommanderInt value);

    FlowController::CommanderFloat cos(FlowController::CommanderFloat value);

    FlowController::CommanderFloat tan(FlowController::CommanderInt value);

    FlowController::CommanderFloat tan(FlowController::CommanderFloat value);

    FlowController::CommanderFloat csc(FlowController::CommanderInt value);

    FlowController::CommanderFloat csc(FlowController::CommanderFloat value);

    FlowController::CommanderFloat sec(FlowController::CommanderInt value);

    FlowController::CommanderFloat sec(FlowController::CommanderFloat value);

    FlowController::CommanderFloat cot(FlowController::CommanderInt value);

    FlowController::CommanderFloat cot(FlowController::CommanderFloat value);

    FlowController::CommanderFloat sinh(FlowController::CommanderInt value);

    FlowController::CommanderFloat sinh(FlowController::CommanderFloat value);

    FlowController::CommanderFloat cosh(FlowController::CommanderInt value);

    FlowController::CommanderFloat cosh(FlowController::CommanderFloat value);

    FlowController::CommanderFloat tanh(FlowController::CommanderInt value);

    FlowController::CommanderFloat tanh(FlowController::CommanderFloat value);

    FlowController::CommanderFloat csch(FlowController::CommanderInt value);

    FlowController::CommanderFloat csch(FlowController::CommanderFloat value);

    FlowController::CommanderFloat sech(FlowController::CommanderInt value);

    FlowController::CommanderFloat sech(FlowController::CommanderFloat value);

    FlowController::CommanderFloat coth(FlowController::CommanderInt value);

    FlowController::CommanderFloat coth(FlowController::CommanderFloat value);

    FlowController::CommanderFloat arcsin(FlowController::CommanderInt value);

    FlowController::CommanderFloat arcsin(FlowController::CommanderFloat value);

    FlowController::CommanderFloat arccos(FlowController::CommanderInt value);

    FlowController::CommanderFloat arccos(FlowController::CommanderFloat value);

    FlowController::CommanderFloat arctan(FlowController::CommanderInt value);

    FlowController::CommanderFloat arctan(FlowController::CommanderFloat value);

    FlowController::CommanderFloat arccsc(FlowController::CommanderInt value);

    FlowController::CommanderFloat arccsc(FlowController::CommanderFloat value);

    FlowController::CommanderFloat arcsec(FlowController::CommanderInt value);

    FlowController::CommanderFloat arcsec(FlowController::CommanderFloat value);

    FlowController::CommanderFloat arccot(FlowController::CommanderInt value);

    FlowController::CommanderFloat arccot(FlowController::CommanderFloat value);

    FlowController::CommanderFloat arcsinh(FlowController::CommanderInt value);

    FlowController::CommanderFloat arcsinh(FlowController::CommanderFloat value);

    FlowController::CommanderFloat arccosh(FlowController::CommanderInt value);

    FlowController::CommanderFloat arccosh(FlowController::CommanderFloat value);

    FlowController::CommanderFloat arctanh(FlowController::CommanderInt value);

    FlowController::CommanderFloat arctanh(FlowController::CommanderFloat value);

    FlowController::CommanderFloat arccsch(FlowController::CommanderInt value);

    FlowController::CommanderFloat arccsch(FlowController::CommanderFloat value);

    FlowController::CommanderFloat arcsech(FlowController::CommanderInt value);

    FlowController::CommanderFloat arcsech(FlowController::CommanderFloat value);

    FlowController::CommanderFloat arccoth(FlowController::CommanderInt value);

    FlowController::CommanderFloat arccoth(FlowController::CommanderFloat value);

    FlowController::CommanderFloat random();

    FlowController::CommanderInt time();

    FlowController::CommanderTuple date();

    void sleep(FlowController::CommanderInt timeToSleep);

    // STRING API METHODS
    FlowController::CommanderString charAt(FlowController::CommanderInt index, FlowController::CommanderString sourceString);

    FlowController::CommanderBool startsWith(FlowController::CommanderString expected,
                                          FlowController::CommanderString sourceString);

    FlowController::CommanderBool endsWith(FlowController::CommanderString expected,
                                        FlowController::CommanderString sourceString);

    FlowController::CommanderBool includes(FlowController::CommanderString expected,
                                        FlowController::CommanderString sourceString);

    FlowController::CommanderInt indexOf(FlowController::CommanderString expected, FlowController::CommanderString sourceString);

    FlowController::CommanderInt length(FlowController::CommanderString sourceString);

    FlowController::CommanderString replace(FlowController::CommanderString oldPhrase, FlowController::CommanderString newPhrase,
                                         FlowController::CommanderString sourceString);

    FlowController::CommanderString replaceAll(FlowController::CommanderString oldPhrase,
                                            FlowController::CommanderString newPhrase,
                                            FlowController::CommanderString sourceString);

    FlowController::CommanderString substring(FlowController::CommanderInt startingIndex,
                                           FlowController::CommanderString sourceString);

    FlowController::CommanderString substring(FlowController::CommanderInt startingIndex,
                                           FlowController::CommanderInt endingIndex,
                                           FlowController::CommanderString sourceString);

    FlowController::CommanderString trim(FlowController::CommanderString sourceString);

    FlowController::CommanderString lower(FlowController::CommanderString sourceString);

    FlowController::CommanderString upper(FlowController::CommanderString sourceString);

    FlowController::CommanderArray<FlowController::CommanderString> split(FlowController::CommanderString splitToken,
                                                                    FlowController::CommanderString sourceString);

    FlowController::CommanderStringPtr toString(FlowController::CommanderTypePtr type);

    FlowController::CommanderBoolPtr includes(FlowController::CommanderTuplePtr tuple, FlowController::CommanderTypePtr data);

    FlowController::CommanderIntPtr indexOf(FlowController::CommanderTuplePtr tuple, FlowController::CommanderTypePtr data);

    FlowController::CommanderIntPtr length(FlowController::CommanderTuplePtr tuple);


    FlowController::CommanderArrayPtr sort(FlowController::CommanderArrayPtr array, FlowController::CommanderLambdaPtr function);

    FlowController::CommanderArrayPtr filter(FlowController::CommanderArrayPtr array, FlowController::CommanderLambdaPtr function);

    FlowController::CommanderArrayPtr map(FlowController::CommanderArrayPtr array, FlowController::CommanderLambdaPtr function);

    FlowController::CommanderTuplePtr foreach(FlowController::CommanderArrayPtr array, FlowController::CommanderLambdaPtr function);

    FlowController::CommanderBoolPtr includes(FlowController::CommanderArrayPtr array, FlowController::CommanderTypePtr data);

    FlowController::CommanderIntPtr indexOf(FlowController::CommanderArrayPtr array, FlowController::CommanderTypePtr data);

    FlowController::CommanderIntPtr length(FlowController::CommanderArrayPtr array);


    FlowController::CommanderStringPtr toString(FlowController::CommanderLambdaPtr function);

    struct AnyCallable {
        AnyCallable() {}
        template<typename F>
        AnyCallable(F&& fun) : AnyCallable(std::function(std::forward<F>(fun))) {}
        template<typename Ret, typename... Args>
        AnyCallable(std::function<Ret(Args...)> fun) : m_any(fun) {}
        std::any m_any;
    };


    using functionType = std::function<FlowController::CommanderTypePtr(...)>;
    /**
     * TODO: Not sure which way we should do this yet
     * https://www.geeksforgeeks.org/passing-a-function-as-a-parameter-in-cpp/
     * https://stackoverflow.com/questions/45715219/store-functions-with-different-signatures-in-a-map
     */
    //inline const std::unordered_map<std::string, std::vector<functionType>> functionImplementations {{"toString", std::vector<functionType>{*toString}}};

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
            if ((FlowController::CommanderInt)originalVal > 0) { return "true"; }
            return "false";
        }
        return std::to_string(originalVal);
    }

    template<typename U>
    U parseStringAsType(std::string originalValue) {
        if (typeid(U) == typeid(bool)) {
            // may need a regex for this, match any instance of "TRUE" or "FALSE", otherwise an exception throws?
            // e.g. "TruE", "tRUe", etc. should yield true
        }
    }
}  // namespace Function

#endif  // COMMANDER_FUNCTIONS_HPP