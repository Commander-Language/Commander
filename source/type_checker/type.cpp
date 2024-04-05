/**
 * @file type.cpp
 * @brief type.cpp contains the implementation for the prototype methods in type.hpp
 * @details For method prototypes, see type.hpp
 */

#include "type.hpp"

namespace TypeChecker {

    std::string typeToString(const Type& type) {
        switch (type) {
            case COMMAND:
                return "COMMAND";
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

    std::string getErrorTypeString(const TyPtr& tyPtr) {
        std::string typeString = getTypeString(tyPtr);
        return typeString == "" ? " UNKNOWN" : typeString;
    }

    std::string getTypeString(const TyPtr& tyPtr) {
        if (!tyPtr) { return ""; }
        switch (tyPtr->getType()) {
            case COMMAND:
            case INT:
            case FLOAT:
            case BOOL:
            case STRING:
                return " " + typeToString(tyPtr->getType());
            case ARRAY: {
                const TyPtr baseTy = std::static_pointer_cast<ArrayTy>(tyPtr)->baseType;
                return (baseTy ? getTypeString(baseTy) : " ANY") + "[]";
            }
            case FUNCTION: {
                const FunctionTyPtr functionTy = std::static_pointer_cast<FunctionTy>(tyPtr);
                return " ((" + getTypeSequenceString(functionTy->parameters) + ") ->"
                     + (functionTy->returnType ? getTypeString(functionTy->returnType) : " ANY") + ")";
            }
            case TUPLE: {
                std::vector<TyPtr> types = std::static_pointer_cast<TupleTy>(tyPtr)->contentTypes;
                if (types.empty()) { return " VOID"; }
                return " (" + getTypeSequenceString(types) + ")";
            }
            default:
                return "";
        }
    }

    std::string getTypeSequenceString(const std::vector<TyPtr>& types) {
        if (types.empty()) { return ""; }
        std::stringstream builder;
        builder << (types[0] ? getTypeString(types[0]).substr(1) : "ANY");
        for (int i = 1; i < types.size(); i++) { builder << "," << (types[i] ? getTypeString(types[i]) : " ANY"); }
        return builder.str();
    }

    bool areTypesEqual(const TyPtr& type1, const TyPtr& type2) {
        if (!type1 || !type2) { return true; }
        Type const typ = type1->getType();
        if (typ != type2->getType()) { return false; }
        if (type1->any() || type2->any()) { return true; }
        switch (typ) {
            case ARRAY: {
                std::shared_ptr<ArrayTy> const ty1 = std::static_pointer_cast<ArrayTy>(type1);
                std::shared_ptr<ArrayTy> const ty2 = std::static_pointer_cast<ArrayTy>(type2);
                return areTypesEqual(ty1->baseType, ty2->baseType);
            }
            case TUPLE: {
                std::shared_ptr<TupleTy> const ty1 = std::static_pointer_cast<TupleTy>(type1);
                std::shared_ptr<TupleTy> const ty2 = std::static_pointer_cast<TupleTy>(type2);
                size_t const size = ty1->contentTypes.size();
                if (size != ty2->contentTypes.size()) { return false; }
                for (int i = 0; i < size; i++) {
                    if (!areTypesEqual(ty1->contentTypes[i], ty2->contentTypes[i])) { return false; }
                }
                return true;
            }
            case FUNCTION: {
                std::shared_ptr<FunctionTy> const ty1 = std::static_pointer_cast<FunctionTy>(type1);
                std::shared_ptr<FunctionTy> const ty2 = std::static_pointer_cast<FunctionTy>(type2);
                size_t const size = ty1->parameters.size();
                if (size != ty2->parameters.size() || !areTypesEqual(ty1->returnType, ty2->returnType)) {
                    return false;
                }
                for (int i = 0; i < size; i++) {
                    if (!areTypesEqual(ty1->parameters[i], ty2->parameters[i])) { return false; }
                }
                return true;
            }
            default:
                return true;
        }
    }

    bool Ty::any() const { return _any; }

    Ty::Ty(bool any) : _any(any) {}
    CommandTy::CommandTy() : Ty(true) {}
    IntTy::IntTy() : Ty(true) {}
    FloatTy::FloatTy() : Ty(true) {}
    BoolTy::BoolTy() : Ty(true) {}
    StringTy::StringTy() : Ty(true) {}

    Type CommandTy::getType() const { return Type::COMMAND; }

    Type IntTy::getType() const { return Type::INT; }

    Type FloatTy::getType() const { return Type::FLOAT; }

    Type BoolTy::getType() const { return Type::BOOL; }

    TupleTy::TupleTy(bool any) : Ty(any) {}

    TupleTy::TupleTy(std::vector<std::shared_ptr<Ty>> types) : Ty(types.empty()), contentTypes(std::move(types)) {}

    Type TupleTy::getType() const { return Type::TUPLE; }

    ArrayTy::ArrayTy(std::shared_ptr<Ty> type) : Ty(type == ANY_TY), baseType(std::move(type)) {}

    ArrayTy::ArrayTy(bool any) : Ty(any) {}

    Type ArrayTy::getType() const { return Type::ARRAY; }

    FunctionTy::FunctionTy(std::vector<std::shared_ptr<Ty>> params, std::shared_ptr<Ty> retType)
        : Ty(false), parameters(std::move(params)), returnType(std::move(retType)) {}

    FunctionTy::FunctionTy(bool any) : Ty(any) {}

    Type FunctionTy::getType() const { return Type::FUNCTION; }

    Type StringTy::getType() const { return Type::STRING; }

}  //  namespace TypeChecker