/**
 * @file var_info.hpp
 * @brief var_info.hpp contains classes related to storing information about variables
 */

#ifndef VAR_INFO_HPP
#define VAR_INFO_HPP

#include "type.hpp"

namespace TypeChecker {
    /**
     * @brief The overarching type of a VarInfo instance.
     *
     */
    enum InfoType : uint8_t { VARIABLE_INFO, FUNCTION_INFO, TYPE_INFO, ALIAS_INFO };

    /**
     * @brief Class for storing variable info
     *
     */
    class VarInfo {
    public:
        /**
         * Tells whether the variable is constant
         */
        bool constant;

        /**
         * The type(s) associated with the variable
         */
        std::vector<TyPtr> types;

        /**
         * @brief Class default constructor
         */
        VarInfo() = default;

        /**
         * @brief Class destructor.
         * @details Necessary because this is a pure virtual class.
         *
         */
        virtual ~VarInfo() = default;

        /**
         * @brief Class constructor
         * @param constant Tells whether the variable is constant or not
         * @param types The type(s) associated with the variable
         */
        VarInfo(bool constant, const std::vector<TyPtr>& types);

        /**
         * @brief Reports the type of this AST node.
         *
         * @return The type of this AST node.
         */
        [[nodiscard]] virtual InfoType infoType() const = 0;
    };
    /**
     * @brief A pointer to a VarInfo instance.
     *
     */
    using VarInfoPtr = std::shared_ptr<VarInfo>;

    /**
     * @brief Class for storing variable info
     *
     */
    class VariableInfo : public VarInfo {
    public:
        /**
         * @brief Default class constructor
         */
        VariableInfo() = default;

        /**
         * @brief Class constructor
         * @param constant Tells whether the variable is constant or not
         * @param types The type associated with the variable
         */
        VariableInfo(bool constant, TyPtr type);

        /**
         * @brief Reports the type of this VarInfo instance.
         *
         * @return `VARIABLE_INFO` always.
         */
        [[nodiscard]] InfoType infoType() const override;
    };
    /**
     * @brief A pointer to a VariableInfo instance.
     *
     */
    using VariableInfoPtr = std::shared_ptr<VariableInfo>;

    /**
     * @brief Class for storing function variable info
     *
     */
    class FunctionInfo : public VarInfo {
    public:
        /**
         * @brief Default class constructor
         */
        FunctionInfo() = default;

        /**
         * @brief Class constructor
         * @param types The type(s) associated with the function variable
         */
        FunctionInfo(const std::vector<TyPtr>& types);

        /**
         * @brief Reports the type of this VarInfo instance.
         *
         * @return `FUNCTION_INFO` always.
         */
        [[nodiscard]] InfoType infoType() const override;
    };
    /**
     * @brief A pointer to a FunctionInfo instance.
     *
     */
    using FunctionInfoPtr = std::shared_ptr<FunctionInfo>;

    /**
     * @brief Class for storing type variable info
     *
     */
    class TypeInfo : public VarInfo {
    public:
        /**
         * @brief Default class constructor
         */
        TypeInfo() = default;

        /**
         * @brief Class constructor
         * @param type The type associated with the type variable
         */
        TypeInfo(TyPtr type);

        /**
         * @brief Reports the type of this VarInfo instance.
         *
         * @return `TYPE_INFO` always.
         */
        [[nodiscard]] InfoType infoType() const override;
    };
    /**
     * @brief A pointer to a TypeInfo instance.
     *
     */
    using TypeInfoPtr = std::shared_ptr<TypeInfo>;

    /**
     * @brief Class for storing alias variable info
     *
     */
    class AliasInfo : public VarInfo {
    public:
        /**
         * @brief Default class constructor
         */
        AliasInfo();

        /**
         * @brief Reports the type of this VarInfo instance.
         *
         * @return `ALIAS_INFO` always.
         */
        [[nodiscard]] InfoType infoType() const override;
    };
    /**
     * @brief A pointer to a AliasInfo instance.
     *
     */
    using AliasInfoPtr = std::shared_ptr<AliasInfo>;
}  //  namespace TypeChecker

#endif  // VAR_INFO_HPP