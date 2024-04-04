/**
 * @file type.hpp
 * @brief Represent a type
 * @details For expressions and types in Commander, each has a type. This class represents all seven types that each
 * expression or type can evaluate to.
 */

#ifndef COMMANDER_TYPE_H
#define COMMANDER_TYPE_H
#include <any>
#include <cstdint>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace TypeChecker {
    /**
     * @brief Enumeration of all possible (base) types in Commander
     *
     */
    enum Type : std::uint8_t { INT, FLOAT, BOOL, TUPLE, ARRAY, FUNCTION, STRING, COMMAND };

    /**
     * @brief An abstract class representing a type
     *
     */
    class Ty {
    protected:
        /**
         * @brief Boolean that represents whether the type can be anything of the given type
         */
        bool _any = true;

        /**
         * @breif Constructor
         */
        Ty(bool any);

    public:
        /**
         * @brief Destructor
         */
        virtual ~Ty() = default;

        /**
         * @brief Tells whether the type can be anything of the given type
         */
        [[nodiscard]] bool any() const;
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
    std::string getTypeString(const TyPtr& tyPtr);

    /**
     * @brief A class representing a Commander command type
     *
     */
    class CommandTy : public Ty {
    public:
        /**
         * Default constructor
         */
        CommandTy();
        /**
         * @brief Gets the type that the class represents
         *
         * @return `COMMAND` always.
         */
        [[nodiscard]] Type getType() const override;
    };
    /**
     * @brief Pointer to an command type
     */
    using CommandTyPtr = std::shared_ptr<CommandTy>;


    /**
     * @brief A class representing a Commander int type
     *
     */
    class IntTy : public Ty {
    public:
        /**
         * Default constructor
         */
        IntTy();
        /**
         * @brief Gets the type that the class represents
         *
         * @return `INT` always.
         */
        [[nodiscard]] Type getType() const override;
    };
    /**
     * @brief Pointer to an int type
     */
    using IntTyPtr = std::shared_ptr<IntTy>;

    /**
     * @brief A class representing a Commander float type
     *
     */
    class FloatTy : public Ty {
    public:
        /**
         * Default constructor
         */
        FloatTy();
        /**
         * @brief Gets the type that the class represents
         *
         * @return `FLOAT` always.
         */
        [[nodiscard]] Type getType() const override;
    };
    /**
     * @brief Pointer to an float type
     */
    using FloatTyPtr = std::shared_ptr<FloatTy>;

    /**
     * @brief A class representing a Commander bool type
     *
     */
    class BoolTy : public Ty {
    public:
        /**
         * Default constructor
         */
        BoolTy();
        /**
         * @brief Gets the type that the class represents
         *
         * @return `BOOL` always.
         */
        [[nodiscard]] Type getType() const override;
    };
    /**
     * @brief Pointer to an bool type
     */
    using BoolTyPtr = std::shared_ptr<BoolTy>;

    /**
     * @brief A class representing a Commander string type
     *
     */
    class StringTy : public Ty {
    public:
        /**
         * Default constructor
         */
        StringTy();
        /**
         * @brief Gets the type that the class represents
         *
         * @return `STRING` always.
         */
        [[nodiscard]] Type getType() const override;
    };
    /**
     * @brief Pointer to an string type
     */
    using StringTyPtr = std::shared_ptr<StringTy>;

    /**
     * @brief A class representing a Commander tuple type
     *
     */
    class TupleTy : public Ty {
    public:
        /**
         * @brief Constructor
         */
        TupleTy(bool any);
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
     * @brief Pointer to an tuple type
     */
    using TupleTyPtr = std::shared_ptr<TupleTy>;

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
         * @brief Alternate Constructor
         */
        ArrayTy(bool any);
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
     * @brief Pointer to an array type
     */
    using ArrayTyPtr = std::shared_ptr<ArrayTy>;

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
         * @brief Alternate Constructor
         */
        FunctionTy(bool any);
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
    /**
     * @brief Pointer to a function type
     */
    using FunctionTyPtr = std::shared_ptr<FunctionTy>;

    const TupleTyPtr VOID_TY = std::make_shared<TupleTy>(false);
    const CommandTyPtr COMMAND_TY = std::make_shared<CommandTy>();
    const IntTyPtr INT_TY = std::make_shared<IntTy>();
    const FloatTyPtr FLOAT_TY = std::make_shared<FloatTy>();
    const BoolTyPtr BOOL_TY = std::make_shared<BoolTy>();
    const StringTyPtr STRING_TY = std::make_shared<StringTy>();
    const TupleTyPtr TUPLE_TY = std::make_shared<TupleTy>(true);
    const ArrayTyPtr ARRAY_TY = std::make_shared<ArrayTy>(true);
    const FunctionTyPtr FUNCTION_TY = std::make_shared<FunctionTy>(true);
    const TyPtr ANY_TY = nullptr;


}  //  namespace TypeChecker

#endif  // COMMANDER_TYPE_H
