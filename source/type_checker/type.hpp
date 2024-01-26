/**
 * @file
 * @brief
 * @details
 */

#ifndef COMMANDER_TYPE_H
#define COMMANDER_TYPE_H
#include <vector>
#include <memory>

enum Type {INT, FLOAT, BOOLEAN, TUPLE, ARRAY, FUNCTION, STRING};

class Ty {
    //TODO: documentation
    virtual Type getType() const = 0;
};

class IntTy : Ty {
    Type getType() const override;
};

class FloatTy : Ty {
    Type getType() const override;
};

class BooleanTy : Ty {
    Type getType() const override;
};

class TupleTy : Ty {
    TupleTy(std::vector<std::unique_ptr<Ty>> types);
    ~TupleTy();
    //TupleTy(TupleTy &otherTuple);

    Type getType() const override;
    std::vector<std::unique_ptr<Ty>> _contentTypes;
};

class ArrayTy : Ty {
    ArrayTy(std::unique_ptr<Ty> type, unsigned int rank);
    Type getType() const override;

    std::unique_ptr<Ty> _baseType;
    unsigned int _rank;
};

class FunctionTy : Ty {
    FunctionTy(std::vector<std::unique_ptr<Ty>> params);
    Type getType() const override;
    std::vector<std::unique_ptr<Ty>> _parameters;
};

class StringTy : Ty {
    Type getType() const override;
};

#endif  // COMMANDER_TYPE_H
