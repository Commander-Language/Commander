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
#include <functional>
#include <math.h>

namespace Function {

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
        auto push_args = [&](auto&&... xs) { (params.push_back(std::forward<decltype(xs)>(xs)), ...); };
        (push_args(args), ...);
        TypeChecker::TyPtr returnTy = params.back();
        params.pop_back();
        return std::make_shared<TypeChecker::FunctionTy>(params, returnTy);
    }

    /* ========== Types ========== */
    const TypeChecker::TupleTyPtr VOID = std::make_shared<TypeChecker::TupleTy>(std::vector<TypeChecker::TyPtr> {});
    const TypeChecker::IntTyPtr INT = std::make_shared<TypeChecker::IntTy>();
    const TypeChecker::FloatTyPtr FLOAT = std::make_shared<TypeChecker::FloatTy>();
    const TypeChecker::BoolTyPtr BOOL = std::make_shared<TypeChecker::BoolTy>();
    const TypeChecker::StringTyPtr STRING = std::make_shared<TypeChecker::StringTy>();

    /**
     * The built in function types
     */
    const std::vector<std::pair<std::string, TypeChecker::FunctionTyPtr>> functionTypes {
            {"parseInt", getFunctionTy(INT, INT)},      {"parseInt", getFunctionTy(FLOAT, INT)},
            {"parseInt", getFunctionTy(BOOL, INT)},     {"parseInt", getFunctionTy(STRING, INT)},
            {"parseFloat", getFunctionTy(INT, FLOAT)},  {"parseFloat", getFunctionTy(FLOAT, FLOAT)},
            {"parseFloat", getFunctionTy(BOOL, FLOAT)}, {"parseFloat", getFunctionTy(STRING, FLOAT)},
            {"parseBool", getFunctionTy(INT, BOOL)},    {"parseBool", getFunctionTy(FLOAT, BOOL)},
            {"parseBool", getFunctionTy(BOOL, BOOL)},   {"parseBool", getFunctionTy(STRING, BOOL)},
            {"print", getFunctionTy(STRING, VOID)},     {"toString", getFunctionTy(INT, STRING)},
            {"println", getFunctionTy(STRING, VOID)}}; //TODO: methods which use same name but different type?

    /* ========== Implementations ========== */
    std::vector<std::any> VOID_RETURN;

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

    TypeChecker::CommanderTuple println(TypeChecker::CommanderString string);

    TypeChecker::CommanderTuple print(TypeChecker::CommanderString string);

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

    struct AnyCallable
    {
        AnyCallable() {}
        template<typename F>
        AnyCallable(F&& fun) : AnyCallable(std::function(std::forward<F>(fun))) {}
        template<typename Ret, typename ... Args>
        AnyCallable(std::function<Ret(Args...)> fun) : m_any(fun) {}
        std::any m_any;
    };

    /**
     * TODO: Not sure which way we should do this yet
     * https://www.geeksforgeeks.org/passing-a-function-as-a-parameter-in-cpp/
     * https://stackoverflow.com/questions/45715219/store-functions-with-different-signatures-in-a-map
     */
    inline const std::unordered_map<std::string, AnyCallable> functionImplementations {
            {"println", println},       {"toString", toString},
            {"print", print}
    };

    /**
     * parseAsType() allows the user to parse the specified data as a specified type.
     * @tparam T - The type of the original data
     * @tparam U - The type of the data to parse as
     * @param originalVal - The unmodified data to parse as type U
     * @return - The original data parsed as type U (e.g. parseAsType<int, bool>(36) will convert 36 from an int to a boolean value)
     */
    template<typename T, typename U>
    U parseAsType(T originalVal) {
        return (U)originalVal;  // TODO: exceptions when string is passed in (or redirect to parseAsString / parseStringAsType)
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
}

#endif  // COMMANDER_FUNCTIONS_HPP