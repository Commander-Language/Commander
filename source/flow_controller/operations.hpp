/**
 * @brief Implementations of Commander Operations
 * @details Each operation will have to check the type of the incoming
 */
#include "source/flow_controller/types.hpp"

namespace FlowController {

    CommanderTypePtr lesserOperation(const CommanderTypePtr &left, const CommanderTypePtr &right);

    CommanderTypePtr lesserEqualOperation(const CommanderTypePtr &left, const CommanderTypePtr &right);

    CommanderTypePtr greaterOperation(const CommanderTypePtr &left, const CommanderTypePtr &right);

    CommanderTypePtr greaterEqualOperation(const CommanderTypePtr &left, const CommanderTypePtr &right);

    CommanderTypePtr equalOperation(const CommanderTypePtr &left, const CommanderTypePtr &right);

    CommanderTypePtr notEqualOperation(const CommanderTypePtr &left, const CommanderTypePtr &right);

    CommanderTypePtr andOperation(const CommanderTypePtr &left, const CommanderTypePtr &right);

    CommanderTypePtr orOperation(const CommanderTypePtr &left, const CommanderTypePtr &right);

    CommanderTypePtr exponentiateOperation(const CommanderTypePtr &left, const CommanderTypePtr &right);

    CommanderTypePtr multiplyOperation(const CommanderTypePtr &left, const CommanderTypePtr &right);

    CommanderTypePtr divideOperation(const CommanderTypePtr &left, const CommanderTypePtr &right);

    CommanderTypePtr moduloOperation(const CommanderTypePtr &left, const CommanderTypePtr &right);

    CommanderTypePtr addOperation(const CommanderTypePtr &left, const CommanderTypePtr &right);

    CommanderTypePtr subtractOperation(const CommanderTypePtr &left, const CommanderTypePtr &right);
} // namespace FlowController
