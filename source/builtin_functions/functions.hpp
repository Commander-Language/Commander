/**
 * @file functions.hpp
 * @brief Contains header content for functions.cpp, which contains all built in functions
 *
 */

#ifndef COMMANDER_FUNCTIONS_HPP
#define COMMANDER_FUNCTIONS_HPP

#include "source/type_checker/type.hpp"
#include <chrono>
#include <cmath>
#include <functional>
#include <string>
#include <thread>
#include <vector>

namespace Function {

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
                                 TypeChecker::STRING_TY, TypeChecker::STRING_TY, TypeChecker::BOOL_TY)}},
            {"indexOf", std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(
                                TypeChecker::STRING_TY, TypeChecker::STRING_TY, TypeChecker::INT_TY)}},
            {"length",
             std::vector<TypeChecker::FunctionTyPtr> {getFunctionTy(TypeChecker::STRING_TY, TypeChecker::INT_TY)}},
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
                                            std::make_shared<TypeChecker::ArrayTy>(TypeChecker::STRING_TY))}}};

    /* ========== Implementations ========== */

    TypeChecker::CommanderInt parseIntAsInt(TypeChecker::CommanderInt number);

    TypeChecker::CommanderInt parseFloatAsInt(TypeChecker::CommanderFloat number);

    TypeChecker::CommanderInt parseBoolAsInt(TypeChecker::CommanderBool value);

    TypeChecker::CommanderInt parseStringAsInt(TypeChecker::CommanderString string);

    TypeChecker::CommanderFloat parseIntAsFloat(TypeChecker::CommanderInt number);

    TypeChecker::CommanderFloat parseFloatAsFloat(TypeChecker::CommanderFloat number);

    TypeChecker::CommanderFloat parseBoolAsFloat(TypeChecker::CommanderBool value);

    TypeChecker::CommanderFloat parseStringAsFloat(TypeChecker::CommanderString string);

    TypeChecker::CommanderBool parseIntAsBool(TypeChecker::CommanderInt number);

    TypeChecker::CommanderBool parseFloatAsBool(TypeChecker::CommanderFloat number);

    TypeChecker::CommanderBool parseBoolAsBool(TypeChecker::CommanderBool value);

    TypeChecker::CommanderBool parseStringAsBool(TypeChecker::CommanderString string);

    void println(TypeChecker::CommanderString string);

    void print(TypeChecker::CommanderString string);

    TypeChecker::CommanderString toString(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat sqrt(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat sqrt(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat ln(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat ln(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat log(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat log(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderInt abs(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat abs(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderInt floor(TypeChecker::CommanderInt value);

    TypeChecker::CommanderInt floor(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderInt ceil(TypeChecker::CommanderInt value);

    TypeChecker::CommanderInt ceil(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderInt round(TypeChecker::CommanderInt value);

    TypeChecker::CommanderInt round(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat sin(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat sin(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat cos(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat cos(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat tan(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat tan(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat csc(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat csc(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat sec(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat sec(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat cot(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat cot(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat sinh(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat sinh(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat cosh(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat cosh(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat tanh(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat tanh(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat csch(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat csch(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat sech(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat sech(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat coth(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat coth(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat arcsin(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat arcsin(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat arccos(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat arccos(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat arctan(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat arctan(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat arccsc(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat arccsc(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat arcsec(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat arcsec(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat arccot(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat arccot(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat arcsinh(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat arcsinh(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat arccosh(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat arccosh(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat arctanh(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat arctanh(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat arccsch(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat arccsch(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat arcsech(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat arcsech(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat arccoth(TypeChecker::CommanderInt value);

    TypeChecker::CommanderFloat arccoth(TypeChecker::CommanderFloat value);

    TypeChecker::CommanderFloat random();

    TypeChecker::CommanderInt time();

    TypeChecker::CommanderTuple date();

    void sleep(TypeChecker::CommanderInt timeToSleep);

    // STRING API METHODS
    TypeChecker::CommanderString charAt(TypeChecker::CommanderInt index, TypeChecker::CommanderString sourceString);

    TypeChecker::CommanderBool startsWith(TypeChecker::CommanderString expected,
                                          TypeChecker::CommanderString sourceString);

    TypeChecker::CommanderBool endsWith(TypeChecker::CommanderString expected,
                                        TypeChecker::CommanderString sourceString);

    TypeChecker::CommanderBool includes(TypeChecker::CommanderString expected,
                                        TypeChecker::CommanderString sourceString);

    TypeChecker::CommanderInt indexOf(TypeChecker::CommanderString expected, TypeChecker::CommanderString sourceString);

    TypeChecker::CommanderInt length(TypeChecker::CommanderString sourceString);

    TypeChecker::CommanderString replace(TypeChecker::CommanderString oldPhrase, TypeChecker::CommanderString newPhrase,
                                         TypeChecker::CommanderString sourceString);

    TypeChecker::CommanderString replaceAll(TypeChecker::CommanderString oldPhrase,
                                            TypeChecker::CommanderString newPhrase,
                                            TypeChecker::CommanderString sourceString);

    TypeChecker::CommanderString substring(TypeChecker::CommanderInt startingIndex,
                                           TypeChecker::CommanderString sourceString);

    TypeChecker::CommanderString substring(TypeChecker::CommanderInt startingIndex,
                                           TypeChecker::CommanderInt endingIndex,
                                           TypeChecker::CommanderString sourceString);

    TypeChecker::CommanderString trim(TypeChecker::CommanderString sourceString);

    TypeChecker::CommanderString lower(TypeChecker::CommanderString sourceString);

    TypeChecker::CommanderString upper(TypeChecker::CommanderString sourceString);

    TypeChecker::CommanderArray<TypeChecker::CommanderString> split(TypeChecker::CommanderString splitToken,
                                                                    TypeChecker::CommanderString sourceString);

    struct AnyCallable {
        AnyCallable() {}
        template<typename F>
        AnyCallable(F&& fun) : AnyCallable(std::function(std::forward<F>(fun))) {}
        template<typename Ret, typename... Args>
        AnyCallable(std::function<Ret(Args...)> fun) : m_any(fun) {}
        std::any m_any;
    };

    /**
     * TODO: Not sure which way we should do this yet
     * https://www.geeksforgeeks.org/passing-a-function-as-a-parameter-in-cpp/
     * https://stackoverflow.com/questions/45715219/store-functions-with-different-signatures-in-a-map
     */
    inline const std::unordered_map<std::string, AnyCallable> functionImplementations {{"println", println},
                                                                                       {"toString", toString},
                                                                                       {"print", print}};

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
            if ((TypeChecker::CommanderInt)originalVal > 0) { return "true"; }
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