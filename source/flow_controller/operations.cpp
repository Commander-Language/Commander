#include "source/flow_controller/operations.hpp"
#include "source/util/commander_exception.hpp"
#include <cmath>

namespace FlowController {

    using TypeChecker::Type;

    CommanderTypePtr lesserOperation(const CommanderTypePtr &left, const CommanderTypePtr &right) {
        const Type leftType = left->getType();
        const Type rightType = right->getType();

        if (leftType == Type::INT && rightType == Type::INT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderBool>(leftInt->value < rightInt->value);
        }
        if (leftType == Type::INT && rightType == Type::FLOAT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderBool>(leftInt->value < rightFloat->value);
        }
        if (leftType == Type::FLOAT && rightType == Type::INT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderBool>(leftFloat->value < rightInt->value);
        }
        if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderBool>(leftFloat->value < rightFloat->value);
        }
        throw Util::CommanderException(
                "Operation Error: Can't \"<\" with types " + TypeChecker::typeToString(leftType) + " and " +
                TypeChecker::typeToString(rightType));
    }

    CommanderTypePtr lesserEqualOperation(const CommanderTypePtr &left, const CommanderTypePtr &right) {
        const Type leftType = left->getType();
        const Type rightType = right->getType();
        if (leftType == Type::INT && rightType == Type::INT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderBool>(leftInt->value <= rightInt->value);
        }
        if (leftType == Type::INT && rightType == Type::FLOAT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderBool>(leftInt->value <= rightFloat->value);
        }
        if (leftType == Type::FLOAT && rightType == Type::INT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderBool>(leftFloat->value <= rightInt->value);
        }
        if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderBool>(leftFloat->value <= rightFloat->value);
        }
        throw Util::CommanderException(
                "Operation Error: Can't \"<=\" with types " + TypeChecker::typeToString(leftType) + " and " +
                TypeChecker::typeToString(rightType));
    }

    CommanderTypePtr greaterOperation(const CommanderTypePtr &left, const CommanderTypePtr &right) {
        const Type leftType = left->getType();
        const Type rightType = right->getType();

        if (leftType == Type::INT && rightType == Type::INT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderBool>(leftInt->value > rightInt->value);
        }
        if (leftType == Type::INT && rightType == Type::FLOAT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderBool>(leftInt->value > rightFloat->value);
        }
        if (leftType == Type::FLOAT && rightType == Type::INT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderBool>(leftFloat->value > rightInt->value);
        }
        if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderBool>(leftFloat->value > rightFloat->value);
        }
        throw Util::CommanderException(
                "Operation Error: Can't \">\" with types " + TypeChecker::typeToString(leftType) + " and " +
                TypeChecker::typeToString(rightType));
    }

    CommanderTypePtr greaterEqualOperation(const CommanderTypePtr &left, const CommanderTypePtr &right) {
        const Type leftType = left->getType();
        const Type rightType = right->getType();
        if (leftType == Type::INT && rightType == Type::INT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderBool>(leftInt->value >= rightInt->value);
        }
        if (leftType == Type::INT && rightType == Type::FLOAT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderBool>(leftInt->value >= rightFloat->value);
        }
        if (leftType == Type::FLOAT && rightType == Type::INT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderBool>(leftFloat->value >= rightInt->value);
        }
        if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderBool>(leftFloat->value >= rightFloat->value);
        }
        throw Util::CommanderException(
                "Operation Error: Can't \">=\" with types " + TypeChecker::typeToString(leftType) + " and " +
                TypeChecker::typeToString(rightType));
    }

    CommanderTypePtr equalOperation(const CommanderTypePtr &left, const CommanderTypePtr &right) {
        const Type leftType = left->getType();
        const Type rightType = right->getType();
        if (leftType == Type::INT && rightType == Type::INT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderBool>(leftInt->value == rightInt->value);
        }
        if (leftType == Type::INT && rightType == Type::FLOAT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderBool>(leftInt->value == rightFloat->value);
        }
        if (leftType == Type::FLOAT && rightType == Type::INT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderBool>(leftFloat->value == rightInt->value);
        }
        if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderBool>(leftFloat->value == rightFloat->value);
        }
        throw Util::CommanderException(
                "Operation Error: Can't \"==\" with types " + TypeChecker::typeToString(leftType) + " and " +
                TypeChecker::typeToString(rightType));
    }

    CommanderTypePtr notEqualOperation(const CommanderTypePtr &left, const CommanderTypePtr &right) {
        const Type leftType = left->getType();
        const Type rightType = right->getType();
        if (leftType == Type::INT && rightType == Type::INT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderBool>(leftInt->value != rightInt->value);
        }
        if (leftType == Type::INT && rightType == Type::FLOAT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderBool>(leftInt->value != rightFloat->value);
        }
        if (leftType == Type::FLOAT && rightType == Type::INT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderBool>(leftFloat->value != rightInt->value);
        }
        if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderBool>(leftFloat->value != rightFloat->value);
        }
        throw Util::CommanderException(
                "Operation Error: Can't \"!=\" with types " + TypeChecker::typeToString(leftType) + " and " +
                TypeChecker::typeToString(rightType));
    }

    CommanderTypePtr andOperation(const CommanderTypePtr &left, const CommanderTypePtr &right) {
        auto leftBool = std::static_pointer_cast<CommanderBool>(left);
        auto rightBool = std::static_pointer_cast<CommanderBool>(right);

        return std::make_shared<CommanderBool>(leftBool->value && rightBool->value);
    }

    CommanderTypePtr orOperation(const CommanderTypePtr &left, const CommanderTypePtr &right) {
        auto leftBool = std::static_pointer_cast<CommanderBool>(left);
        auto rightBool = std::static_pointer_cast<CommanderBool>(right);

        return std::make_shared<CommanderBool>(leftBool->value || rightBool->value);
    }

    CommanderTypePtr exponentiateOperation(const CommanderTypePtr &left, const CommanderTypePtr &right) {
        const Type leftType = left->getType();
        const Type rightType = right->getType();
        if (leftType == Type::INT && rightType == Type::INT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderInt>(std::pow(leftInt->value, rightInt->value));
        }
        if (leftType == Type::INT && rightType == Type::FLOAT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderFloat>(std::pow(leftInt->value, rightFloat->value));
        }
        if (leftType == Type::FLOAT && rightType == Type::INT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderFloat>(std::pow(leftFloat->value, rightInt->value));
        }
        if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderFloat>(std::pow(leftFloat->value, rightFloat->value));
        }
        throw Util::CommanderException(
                "Operation Error: Can't \"**\" with types " + TypeChecker::typeToString(leftType) + " and " +
                TypeChecker::typeToString(rightType));
    }

    CommanderTypePtr multiplyOperation(const CommanderTypePtr &left, const CommanderTypePtr &right) {
        const Type leftType = left->getType();
        const Type rightType = right->getType();
        if (leftType == Type::INT && rightType == Type::INT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderInt>(leftInt->value * rightInt->value);
        }
        if (leftType == Type::INT && rightType == Type::FLOAT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderFloat>(leftInt->value * rightFloat->value);
        }
        if (leftType == Type::FLOAT && rightType == Type::INT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderFloat>(leftFloat->value * rightInt->value);
        }
        if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderFloat>(leftFloat->value * rightFloat->value);
        }
        throw Util::CommanderException(
                "Operation Error: Can't \"*\" with types " + TypeChecker::typeToString(leftType) + " and " +
                TypeChecker::typeToString(rightType));
    }

    CommanderTypePtr divideOperation(const CommanderTypePtr &left, const CommanderTypePtr &right) {
        const Type leftType = left->getType();
        const Type rightType = right->getType();
        if (rightType == Type::INT) {
            if (std::static_pointer_cast<CommanderInt>(right)->value == 0)
                throw Util::CommanderException("DivisionError: unable to divide by zero");
        }
        if (rightType == Type::FLOAT) {
            if (std::static_pointer_cast<CommanderFloat>(right)->value == 0)
                throw Util::CommanderException("DivisionError: unable to divide by zero");
        }

        if (leftType == Type::INT && rightType == Type::INT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderInt>(leftInt->value / rightInt->value);
        }
        if (leftType == Type::INT && rightType == Type::FLOAT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderFloat>(leftInt->value / rightFloat->value);
        }
        if (leftType == Type::FLOAT && rightType == Type::INT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderFloat>(leftFloat->value / rightInt->value);
        }
        if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderFloat>(leftFloat->value / rightFloat->value);
        }
        throw Util::CommanderException(
                "Operation Error: Can't \"/\" with types " + TypeChecker::typeToString(leftType) + " and " +
                TypeChecker::typeToString(rightType));
    }

    CommanderTypePtr moduloOperation(const CommanderTypePtr &left, const CommanderTypePtr &right) {
        const Type leftType = left->getType();
        const Type rightType = right->getType();
        if (leftType == Type::INT && rightType == Type::INT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderInt>(leftInt->value % rightInt->value);
        }
        if (leftType == Type::INT && rightType == Type::FLOAT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderFloat>(std::fmod((PrimitiveFloat) leftInt->value, rightFloat->value));
        }
        if (leftType == Type::FLOAT && rightType == Type::INT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderFloat>(std::fmod(leftFloat->value, (PrimitiveFloat) rightInt->value));
        }
        if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderFloat>(std::fmod(leftFloat->value, rightFloat->value));
        }
        throw Util::CommanderException(
                "Operation Error: Can't \"%\" with types " + TypeChecker::typeToString(leftType) + " and " +
                TypeChecker::typeToString(rightType));
    }

    CommanderTypePtr subtractOperation(const CommanderTypePtr &left, const CommanderTypePtr &right) {
        const Type leftType = left->getType();
        const Type rightType = right->getType();
        if (leftType == Type::INT && rightType == Type::INT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderInt>(leftInt->value - rightInt->value);
        }
        if (leftType == Type::INT && rightType == Type::FLOAT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderFloat>(leftInt->value - rightFloat->value);
        }
        if (leftType == Type::FLOAT && rightType == Type::INT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderFloat>(leftFloat->value - rightInt->value);
        }
        if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderFloat>(leftFloat->value - rightFloat->value);
        }
        throw Util::CommanderException(
                "Operation Error: Can't \"-\" with types " + TypeChecker::typeToString(leftType) + " and " +
                TypeChecker::typeToString(rightType));
    }

    CommanderTypePtr addOperation(const CommanderTypePtr &left, const CommanderTypePtr &right) {
        const Type leftType = left->getType();
        const Type rightType = right->getType();
        if (leftType == Type::INT && rightType == Type::INT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderInt>(leftInt->value + rightInt->value);
        }
        if (leftType == Type::INT && rightType == Type::FLOAT) {
            auto leftInt = std::static_pointer_cast<CommanderInt>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderFloat>(leftInt->value + rightFloat->value);
        }
        if (leftType == Type::FLOAT && rightType == Type::INT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightInt = std::static_pointer_cast<CommanderInt>(right);

            return std::make_shared<CommanderFloat>(leftFloat->value + rightInt->value);
        }
        if (leftType == Type::FLOAT && rightType == Type::FLOAT) {
            auto leftFloat = std::static_pointer_cast<CommanderFloat>(left);
            auto rightFloat = std::static_pointer_cast<CommanderFloat>(right);

            return std::make_shared<CommanderFloat>(leftFloat->value + rightFloat->value);
        }
        if (leftType == Type::STRING && rightType == Type::STRING) {
            auto leftString = std::static_pointer_cast<CommanderString>(left);
            auto rightString = std::static_pointer_cast<CommanderString>(right);
            return std::make_shared<CommanderString>(leftString->value + rightString->value);
        }
        throw Util::CommanderException(
                "Operation Error: Can't \"+\" with types " + TypeChecker::typeToString(leftType) + " and " +
                TypeChecker::typeToString(rightType));
    }
} // namespace FlowController
