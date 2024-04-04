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

    std::string getTypeString(const TyPtr& tyPtr) {
        if (!tyPtr) { return ""; }
        switch (tyPtr->getType()) {
            case COMMAND:
            case INT:
            case FLOAT:
            case BOOL:
            case STRING:
                return " " + typeToString(tyPtr->getType());
            case ARRAY:
                return " " + typeToString(std::static_pointer_cast<ArrayTy>(tyPtr)->baseType->getType()) + "[]";
            case FUNCTION: {
                const FunctionTyPtr functionTy = std::static_pointer_cast<FunctionTy>(tyPtr);
                std::stringstream builder;
                builder << "(";
                for (const TyPtr& type : functionTy->parameters) { builder << " " << typeToString(type->getType()); }
                builder << " ) -> " << typeToString(functionTy->returnType->getType());
                return " " + builder.str();
            }
            case TUPLE: {
                std::stringstream builder;
                builder << "{";
                for (const TyPtr& type : std::static_pointer_cast<TupleTy>(tyPtr)->contentTypes) {
                    builder << " " << typeToString(type->getType());
                }
                builder << " }";
                return " " + builder.str();
            }
            default:
                return "";
        }
    }

    bool areTypesEqual(const TyPtr& type1, const TyPtr& type2) {
        if (!type1 || !type2) { return false; }
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