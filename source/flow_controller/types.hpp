/**
 * @file types.hpp
 * @brief Represents a type and its value.
 */
#ifndef COMMANDER_TYPES_REP_HPP
#define COMMANDER_TYPES_REP_HPP

#include "source/parser/ast_node.hpp"
#include "source/type_checker/type.hpp"
#include <memory>
#include <string>

namespace FlowController {
    /**
     * @brief A commander type.
     */
    struct CommanderType {
        virtual ~CommanderType() = default;

        /**
         * @brief Get the type.
         * @return The type of this.
         */
        [[nodiscard]] virtual TypeChecker::Type getType() const = 0;

        /**
         * @brief Get a string representation of a type.
         * @return The string representation.
         */
        [[nodiscard]] virtual std::string getStringRepresentation() const = 0;

        /**
         * @brief Makes a copy of the type
         */
        [[nodiscard]] virtual std::shared_ptr<CommanderType> copy() const = 0;
    };
    /**
     * @brief A pointer to a Commander Type.
     */
    using CommanderTypePtr = std::shared_ptr<CommanderType>;

    //  =================
    //  ||    Array    ||
    //  =================

    /**
     * @brief A Commander array representation.
     */
    struct CommanderArray : public CommanderType {
        /**
         * @brief Class Constructor
         * @param values The values to set the array.
         */
        CommanderArray(std::vector<CommanderTypePtr> values);

        /**
         * @brief The values in the array.
         */
        std::vector<CommanderTypePtr> values;

        /**
         * @brief Get a string representation of the type.
         * @return The string representation of a commander array.
         */
        [[nodiscard]] std::string getStringRepresentation() const override;

        /**
         * @brief Get the type of this.
         * @return `ARRAY` always.
         */
        [[nodiscard]] TypeChecker::Type getType() const override;

        /**
         * @brief Makes a copy of the type by reference
         */
        [[nodiscard]] std::shared_ptr<CommanderType> copy() const override;
    };
    /**
     * @brief A pointer to a Commander array.
     */
    using CommanderArrayPtr = std::shared_ptr<CommanderArray>;

    //  =================
    //  ||     Bool    ||
    //  =================

    /**
     * @brief A Commander bool representation
     */
    struct CommanderBool : public CommanderType {
        /**
         * @brief Class Constructor
         * @param value The value to set to.
         */
        CommanderBool(bool value);

        /**
         * @brief That value of this Commander bool.
         */
        bool value;

        /**
         * @brief Get a string representation of the type.
         * @return The string representation of a commander bool.
         */
        [[nodiscard]] std::string getStringRepresentation() const override;

        /**
         * @brief Get the type of this.
         * @return `BOOL` always.
         */
        [[nodiscard]] TypeChecker::Type getType() const override;

        /**
         * @brief Makes a copy of the type by value
         */
        [[nodiscard]] std::shared_ptr<CommanderType> copy() const override;
    };
    /**
     * @brief A pointer to a Commander bool.
     */
    using CommanderBoolPtr = std::shared_ptr<CommanderBool>;

    //  =================
    //  ||    Tuple    ||
    //  =================

    /**
     * @brief A Commander tuple representation.
     */
    struct CommanderTuple : public CommanderType {
        /**
         * @brief Class Constructor
         * @param values The values of the tuple to set
         */
        CommanderTuple(std::vector<CommanderTypePtr> values);

        /**
         * @brief The values of a tuple
         */
        std::vector<CommanderTypePtr> values;

        /**
         * @brief Get a string representation of the type.
         * @return The string representation of a commander tuple.
         */
        [[nodiscard]] std::string getStringRepresentation() const override;

        /**
         * @brief Get the type of this.
         * @return `TUPLE` always.
         */
        [[nodiscard]] TypeChecker::Type getType() const override;

        /**
         * @brief Makes a copy of the type by reference
         */
        [[nodiscard]] std::shared_ptr<CommanderType> copy() const override;
    };
    /**
     * @brief A pointer to a Commander tuple.
     */
    using CommanderTuplePtr = std::shared_ptr<CommanderTuple>;

    //  =================
    //  ||   String    ||
    //  =================

    /**
     * @brief A Commander string representation.
     */
    struct CommanderString : public CommanderType {
        /**
         * @brief Class Constructor
         * @param value The value to set to.
         */
        CommanderString(std::string value);

        /**
         * @brief The value of this string
         */
        std::string value;

        /**
         * @brief Get a string representation of the type.
         * @return The string representation of a commander string.
         */
        [[nodiscard]] std::string getStringRepresentation() const override;

        /**
         * @brief Get the type of this.
         * @return `STRING` always.
         */
        [[nodiscard]] TypeChecker::Type getType() const override;

        /**
         * @brief Makes a copy of the type by reference
         */
        [[nodiscard]] std::shared_ptr<CommanderType> copy() const override;
    };
    /**
     * @brief A pointer to a Commander string.
     */
    using CommanderStringPtr = std::shared_ptr<CommanderString>;

    //  =================
    //  ||     Int     ||
    //  =================

    /**
     * @brief A Commander int representation.
     */
    struct CommanderInt : public CommanderType {
        /**
         * @brief Class Constructor
         * @param value The value to set to.
         */
        CommanderInt(int64_t value);

        /**
         * @brief The value of this int.
         */
        int64_t value;

        /**
         * @brief Get a string representation of the type.
         * @return The string representation of a commander int.
         */
        [[nodiscard]] std::string getStringRepresentation() const override;

        /**
         * @brief Get the type of this.
         * @return `INT` always.
         */
        [[nodiscard]] TypeChecker::Type getType() const override;

        /**
         * @brief Makes a copy of the type by value
         */
        [[nodiscard]] std::shared_ptr<CommanderType> copy() const override;
    };
    /**
     * @brief A pointer to a Commander int.
     */
    using CommanderIntPtr = std::shared_ptr<CommanderInt>;

    //  =================
    //  ||    Float    ||
    //  =================

    /**
     * @brief A Commander float representation.
     */
    struct CommanderFloat : public CommanderType {
        /**
         * @brief Class Constructor
         * @param value The value to set to
         */
        CommanderFloat(double value);

        /**
         * @brief The value of this float.
         */
        double value;

        /**
         * @brief Get a string representation of the type.
         * @return The string representation of a commander float.
         */
        [[nodiscard]] std::string getStringRepresentation() const override;

        /**
         * @brief Get the type of this.
         * @return `FLOAT` always.
         */
        [[nodiscard]] TypeChecker::Type getType() const override;

        /**
         * @brief Makes a copy of the type by value
         */
        [[nodiscard]] std::shared_ptr<CommanderType> copy() const override;
    };
    /**
     * @brief A pointer to a Commander float.
     */
    using CommanderFloatPtr = std::shared_ptr<CommanderFloat>;

    //  =================
    //  ||   Lambda    ||
    //  =================

    /**
     * @brief A commander lambda representation
     */
    struct CommanderLambda : public CommanderType {
        /**
         * @brief Class Constructor
         * @param bindings The bindings for the lambda/function
         * @param body The body of the lambda/function
         * @param name The name of the lambda/function (defaults to empty)
         */
        CommanderLambda(Parser::BindingsNodePtr bindings, Parser::StmtNodePtr body, TypeChecker::TyPtr returnType,
                        std::string name = "");

        std::string name;
        Parser::BindingsNodePtr bindings;
        Parser::StmtNodePtr body;
        TypeChecker::TyPtr returnType;

        /**
         * @brief Get a string representation of the type.
         * @return The string representation of a commander lambda.
         */
        [[nodiscard]] std::string getStringRepresentation() const override;

        /**
         * @brief Get the type of this.
         * @return `FUNCTION` always.
         */
        [[nodiscard]] TypeChecker::Type getType() const override;

        /**
         * @brief Makes a copy of the type by reference
         */
        [[nodiscard]] std::shared_ptr<CommanderType> copy() const override;
    };
    /**
     * @brief A pointer to a Commander lambda.
     */
    using CommanderLambdaPtr = std::shared_ptr<CommanderLambda>;

    //  ==================
    //  ||   Command    ||
    //  ==================

    /**
     * @brief A commander command representation
     */
    struct CommanderCommand : public CommanderType {
        /**
         * @brief Class Constructor
         * @param command The command
         */
        CommanderCommand(Parser::CmdNodePtr cmdNode, std::string command);

        Parser::CmdNodePtr cmdNode;
        std::string command;

        /**
         * @brief Get a string representation of the type.
         * @return The string representation of a commander lambda.
         */
        [[nodiscard]] std::string getStringRepresentation() const override;

        /**
         * @brief Get the type of this.
         * @return `COMMAND` always.
         */
        [[nodiscard]] TypeChecker::Type getType() const override;

        /**
         * @brief Makes a copy of the type by reference
         */
        [[nodiscard]] std::shared_ptr<CommanderType> copy() const override;
    };
    /**
     * @brief A pointer to a Commander lambda.
     */
    using CommanderCommandPtr = std::shared_ptr<CommanderCommand>;

    // Primitive representation of Commander Types
    using PrimitiveInt = int64_t;
    using PrimitiveFloat = double;
}  // namespace FlowController

#endif
