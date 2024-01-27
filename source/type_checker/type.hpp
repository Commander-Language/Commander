/**
 * @file
 * @brief
 * @details
 */

#ifndef COMMANDER_TYPE_H
#define COMMANDER_TYPE_H
#include <cstdint>
#include <memory>
#include <vector>

namespace TypeChecker {

    enum Type : std::uint8_t { INT, FLOAT, BOOL, TUPLE, ARRAY, FUNCTION, STRING };

    class Ty {
        // TODO: documentatiojn
    public:
        virtual ~Ty() = default;
        [[nodiscard]] virtual Type getType() const = 0;
    };

    std::string typeToString(Type type);

    /**
 * @brief Pointer to a type
     */
    using TyPtr = std::shared_ptr<Ty>;

    class IntTy : public Ty {
        [[nodiscard]] Type getType() const override;
    };

    class FloatTy : public Ty {
        [[nodiscard]] Type getType() const override;
    };

    class BoolTy : public Ty {
        [[nodiscard]] Type getType() const override;
    };

    class StringTy : public Ty {
        [[nodiscard]] Type getType() const override;
    };

    class TupleTy : public Ty {
    public:
        TupleTy(std::vector<std::shared_ptr<Ty>> types);
        [[nodiscard]] Type getType() const override;
        std::vector<std::shared_ptr<Ty>> contentTypes;
    };

    class ArrayTy : public Ty {
    public:
        ArrayTy(std::shared_ptr<Ty> type);
        [[nodiscard]] Type getType() const override;
        std::shared_ptr<Ty> baseType;
    };

    class FunctionTy : public Ty {
    public:
        FunctionTy(std::vector<std::shared_ptr<Ty>> params, std::shared_ptr<Ty> returnType);
        [[nodiscard]] Type getType() const override;
        std::vector<std::shared_ptr<Ty>> parameters;
        std::shared_ptr<Ty> returnType;
    };

} //  namespace TypeChecker

#endif  // COMMANDER_TYPE_H
