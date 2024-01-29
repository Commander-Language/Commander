/**
 * @file type.hpp
 * @brief Represent a type
 * @details For expressions and types in Commander, each has a type. This class represents all seven types that each
 * expression or type can evaluate to.
 */

#ifndef COMMANDER_TYPE_H
#define COMMANDER_TYPE_H
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace TypeChecker {

    /**
     * @brief Enumeration of all possible (base) types in Commander
     *
     */
    enum Type : std::uint8_t { INT, FLOAT, BOOL, TUPLE, ARRAY, FUNCTION, STRING };

    /**
     * @brief An abstract class representing a type
     *
     */
    class Ty {
    public:
        /**
         * @brief Destructor
         */
        virtual ~Ty() = default;
        /**
         * @brief Gets the type that the Ty class represents
         *
         * @return The type of the Ty class
         */
        [[nodiscard]] virtual Type getType() const = 0;
    };

    /**
     * @brief Pointer to a type
     */
    using TyPtr = std::shared_ptr<Ty>;

    /**
     * @brief Gets the string version of the given type
     *
     */
    std::string typeToString(const Type& type);

    /**
     * @brief Checks if two types are equal
     *
     * @param type1 First type
     * @param type2 Second type
     * @return True if the types are the same, false otherwise
     */
    bool areTypesEqual(const TyPtr& type1, const TyPtr& type2);

    /**
     * @brief Gets the string representation of the given type for S-Expressions
     * @param tyPtr The given type
     *
     * @return The string representation of the given type
     */
    std::string getTypeString(const TypeChecker::TyPtr& tyPtr);

    /**
     * @brief A class representing a Commander int type
     *
     */
    class IntTy : public Ty {
        /**
         * @brief Gets the type that the class represents
         *
         * @return `INT` always.
         */
        [[nodiscard]] Type getType() const override;
    };

    /**
     * @brief A class representing a Commander float type
     *
     */
    class FloatTy : public Ty {
        /**
         * @brief Gets the type that the class represents
         *
         * @return `FLOAT` always.
         */
        [[nodiscard]] Type getType() const override;
    };

    /**
     * @brief A class representing a Commander bool type
     *
     */
    class BoolTy : public Ty {
        /**
         * @brief Gets the type that the class represents
         *
         * @return `BOOL` always.
         */
        [[nodiscard]] Type getType() const override;
    };

    /**
     * @brief A class representing a Commander string type
     *
     */
    class StringTy : public Ty {
        /**
         * @brief Gets the type that the class represents
         *
         * @return `STRING` always.
         */
        [[nodiscard]] Type getType() const override;
    };

    /**
     * @brief A class representing a Commander tuple type
     *
     */
    class TupleTy : public Ty {
    public:
        /**
         * @brief Constructor
         */
        TupleTy(std::vector<std::shared_ptr<Ty>> types);
        /**
         * @brief Gets the type that the class represents
         *
         * @return `TUPLE` always.
         */
        [[nodiscard]] Type getType() const override;
        /**
         * @brief The types of the items inside the tuple
         */
        std::vector<std::shared_ptr<Ty>> contentTypes;
    };

    /**
     * @brief A class representing a Commander array type
     *
     */
    class ArrayTy : public Ty {
    public:
        /**
         * @brief Constructor
         */
        ArrayTy(std::shared_ptr<Ty> type);
        /**
         * @brief Gets the type that the class represents
         *
         * @return `ARRAY` always.
         */
        [[nodiscard]] Type getType() const override;
        /**
         * @brief The base type of the array (i.e. the type of everything inside the array)
         */
        std::shared_ptr<Ty> baseType;
    };

    /**
     * @brief A class representing a Commander function type
     *
     */
    class FunctionTy : public Ty {
    public:
        /**
         * @brief Constructor
         */
        FunctionTy(std::vector<std::shared_ptr<Ty>> params, std::shared_ptr<Ty> retType);
        /**
         * @brief Gets the type that the class represents
         *
         * @return `FUNCTION` always.
         */
        [[nodiscard]] Type getType() const override;
        /**
         * @brief The types of all the function parameters
         */
        std::vector<std::shared_ptr<Ty>> parameters;
        /**
         * @brief The return type of the function
         */
        std::shared_ptr<Ty> returnType;
    };

}  //  namespace TypeChecker

#endif  // COMMANDER_TYPE_H
