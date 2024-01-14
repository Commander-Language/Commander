#include "flow_controller.hpp"

namespace FlowController {
    FlowController::FlowController(Parser::ASTNodeList& nodes) : _nodes(std::move(nodes)) {}
}  // namespace FlowController
