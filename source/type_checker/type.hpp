/**
 * @file
 * @brief
 * @details
 */

#ifndef COMMANDER_TYPE_H
#define COMMANDER_TYPE_H
#include <vector>
#include <memory>

namespace TypeChecker {

    enum Type : std::uint8_t { INT, FLOAT, BOOLEAN, TUPLE, ARRAY, FUNCTION, STRING };

    class Ty {
        // TODO: documentatiojn
    public:
        virtual ~Ty() = default;
        [[nodiscard]] virtual Type getType() const = 0;
    };

    /**
 * @brief Pointer to a type
     */
    using TyPtr = std::shared_ptr<Ty>;

    class IntTy : Ty {
        [[nodiscard]] Type getType() const override;
    };

    class FloatTy : Ty {
        [[nodiscard]] Type getType() const override;
    };

    class BooleanTy : Ty {
        [[nodiscard]] Type getType() const override;
    };

    class TupleTy : Ty {
        TupleTy(std::vector<std::unique_ptr<Ty>> types);
        [[nodiscard]] Type getType() const override;

        std::vector<std::unique_ptr<Ty>> _contentTypes;
    };

    class ArrayTy : Ty {
        ArrayTy(std::unique_ptr<Ty> type, unsigned int rank);
        [[nodiscard]] Type getType() const override;

        std::unique_ptr<Ty> _baseType;
        unsigned int _rank;
    };

    class FunctionTy : Ty {
        FunctionTy(std::vector<std::unique_ptr<Ty>> params, std::unique_ptr<Ty> returnType);
        [[nodiscard]] Type getType() const override;

        std::vector<std::unique_ptr<Ty>> _parameters;
        std::unique_ptr<Ty> _returnType;
    };

    class StringTy : Ty {
        [[nodiscard]] Type getType() const override;
    };

} //  namespace TypeChecker

#endif  // COMMANDER_TYPE_H
