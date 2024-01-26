/**
 * @file
 * @bried
 * @details
 */

#include "type.hpp"

Type IntTy::getType() const {
    return Type::INT;
}

Type FloatTy::getType() const {
    return Type::FLOAT;
}

Type BooleanTy::getType() const {
    return Type::BOOLEAN;
}

TupleTy::TupleTy(std::vector<std::unique_ptr<Ty>> types) {
    _contentTypes = std::move(types);
}

Type TupleTy::getType() const {
    return Type::TUPLE;
}

ArrayTy::ArrayTy(std::unique_ptr<Ty> type, unsigned int rank) {
    _baseType = std::move(type);
    _rank = rank;
}

Type ArrayTy::getType() const {
    return Type::ARRAY;
}

FunctionTy::FunctionTy(std::vector<std::unique_ptr<Ty>> params) {
    _parameters = std::move(params);
}

Type FunctionTy::getType() const {
    return Type::FUNCTION;
}

Type StringTy::getType() const {
    return Type::STRING;
}