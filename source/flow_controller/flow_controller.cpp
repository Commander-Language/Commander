#include "flow_controller.hpp"

namespace FlowController {
    FlowController::FlowController(Parser::ASTNodeList& nodes) : _nodes(std::move(nodes)) {}
    void FlowController::runCommand() {}
    void FlowController::setVariable(std::string name, std::any value) {
        _symbolTable.addOrUpdateVariable(std::move(name), 0);
    }
    void FlowController::runtime() {
        for (auto& node : _nodes) {
            switch (node->nodeType()) {
                case Parser::BINDING: {

                }
                case Parser::BINDINGS: {
                }
                case Parser::CMD: {
                }
                case Parser::EXPR: {
                }
                case Parser::EXPRS: {
                }
                case Parser::PRGM: {
                }
                case Parser::STMT: {
                }
                case Parser::STMTS: {
                }
                case Parser::STRING: {
                }
                case Parser::TYPE: {
                }
                case Parser::VARIABLE: {
                }
            }
        }
    }

    void FlowController::_doBindings(Parser::BindingNode node) {
        node.
    }

}  // namespace FlowController
