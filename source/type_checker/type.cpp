/**
 * @file type.cpp
 * @brief type.cpp contains the implementation for the prototype methods in type.hpp
 * @details For method prototypes, see type.hpp
 */

#include "type.hpp"

namespace TypeChecker {

    std::string typeToString(const Type& type) {
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

    bool areTypesEqual(const TyPtr& type1, const TyPtr& type2) {
        if (!type1 || !type2) { return false; }
        Type t = type1->getType();
        if (t != type2->getType()) { return false; }
        switch (t) {
            case ARRAY: {
                std::shared_ptr<ArrayTy> ty1 = std::static_pointer_cast<ArrayTy>(type1);
                std::shared_ptr<ArrayTy> ty2 = std::static_pointer_cast<ArrayTy>(type2);
                return areTypesEqual(ty1->baseType, ty2->baseType);
            }
            case TUPLE: {
                std::shared_ptr<TupleTy> ty1 = std::static_pointer_cast<TupleTy>(type1);
                std::shared_ptr<TupleTy> ty2 = std::static_pointer_cast<TupleTy>(type2);
                int size = ty1->contentTypes.size();
                if (size != ty2->contentTypes.size()) { return false; }
                for (int i = 0; i < size; i++) {
                    if (!areTypesEqual(ty1->contentTypes[i], ty2->contentTypes[i])) { return false; }
                }
                return true;
            }
            case FUNCTION: {
                std::shared_ptr<FunctionTy> ty1 = std::static_pointer_cast<FunctionTy>(type1);
                std::shared_ptr<FunctionTy> ty2 = std::static_pointer_cast<FunctionTy>(type2);
                int size = ty1->parameters.size();
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

    Type IntTy::getType() const { return Type::INT; }

    Type FloatTy::getType() const { return Type::FLOAT; }

    Type BoolTy::getType() const { return Type::BOOL; }

    TupleTy::TupleTy(std::vector<std::shared_ptr<Ty>> types) { contentTypes = std::move(types); }

    Type TupleTy::getType() const { return Type::TUPLE; }

    ArrayTy::ArrayTy(std::shared_ptr<Ty> type) { baseType = std::move(type); }

    Type ArrayTy::getType() const { return Type::ARRAY; }

    FunctionTy::FunctionTy(std::vector<std::shared_ptr<Ty>> params, std::shared_ptr<Ty> retType) {
        parameters = std::move(params);
        returnType = std::move(retType);
    }

    Type FunctionTy::getType() const { return Type::FUNCTION; }

    Type StringTy::getType() const { return Type::STRING; }

}  //  namespace TypeChecker