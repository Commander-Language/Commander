/**
 * @file types.cpp
 * @brief Implementation of Commander types and methods.
 */
#include "types.hpp"

#include <utility>

namespace FlowController {

    //  =================
    //  ||    Array    ||
    //  =================
    CommanderArray::CommanderArray(std::vector<CommanderTypePtr> values) : values(std::move(values)) {}

    [[nodiscard]] std::string CommanderArray::getStringRepresentation() const {
        std::string output = "[";
        for (int i = 0; i < values.size(); i++) {
            if (i != values.size() - 1) output.append(values[i]->getStringRepresentation() + ", ");
            else
                output.append(values[i]->getStringRepresentation());
        }
        output.append("]");
        return output;
    }

    [[nodiscard]] TypeChecker::Type CommanderArray::getType() const { return TypeChecker::ARRAY; }

    //  =================
    //  ||     Bool    ||
    //  =================

    CommanderBool::CommanderBool(bool value) : value(value) {}

    [[nodiscard]] std::string CommanderBool::getStringRepresentation() const { return value ? "true" : "false"; }

    [[nodiscard]] TypeChecker::Type CommanderBool::getType() const { return TypeChecker::BOOL; }

    //  =================
    //  ||     Int     ||
    //  =================
    CommanderInt::CommanderInt(int64_t value) : value(value) {}

    [[nodiscard]] std::string CommanderInt::getStringRepresentation() const { return std::to_string(value); }

    [[nodiscard]] TypeChecker::Type CommanderInt::getType() const { return TypeChecker::INT; }

    //  =================
    //  ||    Float    ||
    //  =================

    CommanderFloat::CommanderFloat(double value) : value(value) {}

    [[nodiscard]] std::string CommanderFloat::getStringRepresentation() const { return std::to_string(value); }

    [[nodiscard]] TypeChecker::Type CommanderFloat::getType() const { return TypeChecker::FLOAT; }

    //  =================
    //  ||    Tuple    ||
    //  =================
    CommanderTuple::CommanderTuple(std::vector<CommanderTypePtr> values) : values(std::move(values)) {}

    [[nodiscard]] std::string CommanderTuple::getStringRepresentation() const {
        std::string output = "(";
        for (int i = 0; i < values.size(); i++) {
            if (i != values.size() - 1) output.append(values[i]->getStringRepresentation() + ", ");
            else
                output.append(values[i]->getStringRepresentation());
        }
        output.append(")");
        return output;
    }

    //  =================
    //  ||   Lambda    ||
    //  =================
    CommanderLambda::CommanderLambda(Parser::BindingsNodePtr bindings, Parser::StmtNodePtr body,
                                     TypeChecker::TyPtr returnType, std::string name)
        : bindings(std::move(bindings)), body(std::move(body)), name(std::move(name)),
          returnType(std::move(returnType)) {}

    [[nodiscard]] TypeChecker::Type CommanderTuple::getType() const { return TypeChecker::TUPLE; }

    [[nodiscard]] std::string CommanderLambda::getStringRepresentation() const {
        std::stringstream builder;
        builder << "(";
        for (int i = 0; i < bindings->bindings.size(); i++) {
            if (i != 0) { builder << ", "; }
            auto binding = bindings->bindings[i];
            if (binding->constant) { builder << "const "; }
            builder << binding->variable;
            if (binding->type) { builder << ":" << TypeChecker::getTypeString(binding->type->type); }
        }
        builder << ") ->" << TypeChecker::getTypeString(returnType);
        return builder.str();
    }

    [[nodiscard]] TypeChecker::Type CommanderLambda::getType() const { return TypeChecker::FUNCTION; }

    //  =================
    //  ||   String    ||
    //  =================

    CommanderString::CommanderString(std::string value) : value(std::move(value)) {}

    [[nodiscard]] std::string CommanderString::getStringRepresentation() const { return value; }

    [[nodiscard]] TypeChecker::Type CommanderString::getType() const { return TypeChecker::STRING; }

    //  ==================
    //  ||   Command    ||
    //  ==================

    CommanderCommand::CommanderCommand(Parser::CmdNodePtr cmdNode, std::string command)
        : cmdNode(std::move(cmdNode)), command(std::move(command)) {}

    std::string CommanderCommand::getStringRepresentation() const { return command; }

    TypeChecker::Type CommanderCommand::getType() const { return TypeChecker::COMMAND; }

}  // namespace FlowController
