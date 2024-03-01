/**
 * @file types.cpp
 * @brief Implementation of Commander types and methods.
 */
#include "types.hpp"

namespace FlowController {

    //  =================
    //  ||    Array    ||
    //  =================

    [[nodiscard]] std::string CommanderArray::getStringRepresentation() const {
        std::string output = "[";
        for (int i = 0; i < values.size(); i++) {
            if (i != values.size() - 1)
                output.append(values[i]->getStringRepresentation() + ", ");
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

    [[nodiscard]] std::string CommanderBool::getStringRepresentation() const { return value ? "true" : "false"; }

    [[nodiscard]] TypeChecker::Type CommanderBool::getType() const { return TypeChecker::BOOL; }

    //  =================
    //  ||     Int     ||
    //  =================

    [[nodiscard]] std::string CommanderInt::getStringRepresentation() const { return std::to_string(value); }

    [[nodiscard]] TypeChecker::Type CommanderInt::getType() const { return TypeChecker::INT; }

    //  =================
    //  ||    Float    ||
    //  =================

    [[nodiscard]] std::string CommanderFloat::getStringRepresentation() const { return std::to_string(value); }

    [[nodiscard]] TypeChecker::Type CommanderFloat::getType() const { return TypeChecker::FLOAT; }

    //  =================
    //  ||    Tuple    ||
    //  =================

    [[nodiscard]] std::string CommanderTuple::getStringRepresentation() const {
        std::string output = "(";
        for (int i = 0; i < values.size(); i++) {
            if (i != values.size() - 1)
                output.append(values[i]->getStringRepresentation() + ", ");
            else
                output.append(values[i]->getStringRepresentation());
        }
        output.append(")");
        return output;
    }

    //  =================
    //  ||   Lambda    ||
    //  =================

    [[nodiscard]] TypeChecker::Type CommanderTuple::getType() const { return TypeChecker::TUPLE; }

    [[nodiscard]] std::string CommanderLambda::getStringRepresentation() const {
        if (name.empty())
            return "<Anonymous Lambda>";
        else
            return "<Function " + name + ">";
    }

    [[nodiscard]] TypeChecker::Type CommanderLambda::getType() const { return TypeChecker::FUNCTION; }

    //  =================
    //  ||   String    ||
    //  =================

    [[nodiscard]] std::string CommanderString::getStringRepresentation() const { return value; }

    [[nodiscard]] TypeChecker::Type CommanderString::getType() const { return TypeChecker::STRING; }
} // namespace FlowController