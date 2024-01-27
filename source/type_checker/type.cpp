/**
* @file type.cpp
* @brief type.cpp contains the implementation for the prototype methods in type.hpp
* @details For method prototypes, see type.hpp
*/

#include "type.hpp"

namespace TypeChecker {

    std::string typeToString(Type type) {
        switch (type) {
            case INT:
                return "INT";
            case FLOAT:
                return "FLOAT";
            case BOOL:
                return "BOOL";
            case STRING:
                return "STRING";
            case ARRAY:
                return "ARRAY";
            case TUPLE:
                return "TUPLE";
            case FUNCTION:
                return "FUNCTION";
            default:
                return "UNKNOWN";
        }
    }

    std::string getTypeString(const TypeChecker::TyPtr& tyPtr) {
        return (tyPtr ? " " + TypeChecker::typeToString(tyPtr->getType()) : "");
    }

    Type IntTy::getType() const { return Type::INT; }

    Type FloatTy::getType() const { return Type::FLOAT; }

    Type BoolTy::getType() const { return Type::BOOL; }

    TupleTy::TupleTy(std::vector<std::shared_ptr<Ty>> types) { contentTypes = std::move(types); }

    Type TupleTy::getType() const { return Type::TUPLE; }

    ArrayTy::ArrayTy(std::shared_ptr<Ty> type) {
        baseType = std::move(type);
    }

    Type ArrayTy::getType() const { return Type::ARRAY; }

    FunctionTy::FunctionTy(std::vector<std::shared_ptr<Ty>> params, std::shared_ptr<Ty> retType) {
        parameters = std::move(params);
        returnType = std::move(retType);
    }

    Type FunctionTy::getType() const { return Type::FUNCTION; }

    Type StringTy::getType() const { return Type::STRING; }

} //  namespace TypeChecker