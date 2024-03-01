/**
 * @brief Implementations of Commander Operations
 * @details Each operation will have to check the type of the incoming Commander Types
 */
#include "source/flow_controller/types.hpp"

namespace FlowController {
    //  =============================
    //  ||    Binary Operations    ||
    //  =============================

    /**
     * @brief Implementation of the lesser operator <
     * @param left The left side of the binary operation
     * @param right The right side of the binary operation
     * @return The value of doing the operation (as a Commander Type)
     */
    CommanderTypePtr lesserOperation(const CommanderTypePtr& left, const CommanderTypePtr& right);

    /**
     * @brief Implementation of the lesser-equal operator <=
     * @param left The left side of the binary operation
     * @param right The right side of the binary operation
     * @return The value of doing the operation (as a Commander Type)
     */
    CommanderTypePtr lesserEqualOperation(const CommanderTypePtr& left, const CommanderTypePtr& right);

    /**
     * @brief Implementation of the greater operator >
     * @param left The left side of the binary operation
     * @param right The right side of the binary operation
     * @return The value of doing the operation (as a Commander Type)
     */
    CommanderTypePtr greaterOperation(const CommanderTypePtr& left, const CommanderTypePtr& right);

    /**
     * @brief Implementation of the greater-equal operator >=
     * @param left The left side of the binary operation
     * @param right The right side of the binary operation
     * @return The value of doing the operation (as a Commander Type)
     */
    CommanderTypePtr greaterEqualOperation(const CommanderTypePtr& left, const CommanderTypePtr& right);

    /**
     * @brief Implementation of the equal operator ==
     * @param left The left side of the binary operation
     * @param right The right side of the binary operation
     * @return The value of doing the operation (as a Commander Type)
     */
    CommanderTypePtr equalOperation(const CommanderTypePtr& left, const CommanderTypePtr& right);

    /**
     * @brief Implementation of the not-equal operator !=
     * @param left The left side of the binary operation
     * @param right The right side of the binary operation
     * @return The value of doing the operation (as a Commander Type)
     */
    CommanderTypePtr notEqualOperation(const CommanderTypePtr& left, const CommanderTypePtr& right);

    /**
     * @brief Implementation of the and operator &&
     * @param left The left side of the binary operation
     * @param right The right side of the binary operation
     * @return The value of doing the operation (as a Commander Type)
     */
    CommanderTypePtr andOperation(const CommanderTypePtr& left, const CommanderTypePtr& right);

    /**
     * @brief Implementation of the or operator ||
     * @param left The left side of the binary operation
     * @param right The right side of the binary operation
     * @return The value of doing the operation (as a Commander Type)
     */
    CommanderTypePtr orOperation(const CommanderTypePtr& left, const CommanderTypePtr& right);

    /**
     * @brief Implementation of the exponetiate operator **
     * @param left The left side of the binary operation
     * @param right The right side of the binary operation
     * @return The value of doing the operation (as a Commander Type)
     */
    CommanderTypePtr exponentiateOperation(const CommanderTypePtr& left, const CommanderTypePtr& right);

    /**
     * @brief Implementation of the multiply operator *
     * @param left The left side of the binary operation
     * @param right The right side of the binary operation
     * @return The value of doing the operation (as a Commander Type)
     */
    CommanderTypePtr multiplyOperation(const CommanderTypePtr& left, const CommanderTypePtr& right);

    /**
     * @brief Implementation of the divide operator /
     * @param left The left side of the binary operation
     * @param right The right side of the binary operation
     * @return The value of doing the operation (as a Commander Type)
     */
    CommanderTypePtr divideOperation(const CommanderTypePtr& left, const CommanderTypePtr& right);

    /**
     * @brief Implementation of the modulo operator %
     * @param left The left side of the binary operation
     * @param right The right side of the binary operation
     * @return The value of doing the operation (as a Commander Type)
     */
    CommanderTypePtr moduloOperation(const CommanderTypePtr& left, const CommanderTypePtr& right);

    /**
     * @brief Implementation of the add operator +
     * @param left The left side of the binary operation
     * @param right The right side of the binary operation
     * @return The value of doing the operation (as a Commander Type)
     */
    CommanderTypePtr addOperation(const CommanderTypePtr& left, const CommanderTypePtr& right);

    /**
     * @brief Implementation of the subtract operator -
     * @param left The left side of the binary operation
     * @param right The right side of the binary operation
     * @return The value of doing the operation (as a Commander Type)
     */
    CommanderTypePtr subtractOperation(const CommanderTypePtr& left, const CommanderTypePtr& right);
}  // namespace FlowController
