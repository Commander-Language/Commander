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
                    _binding(std::dynamic_pointer_cast<Parser::BindingNode>(node));
                }
                case Parser::BINDINGS: {
                    _bindings(std::dynamic_pointer_cast<Parser::BindingsNode>(node));
                }
                case Parser::CMD: {
                    _cmd(std::dynamic_pointer_cast<Parser::CmdNode>(node));
                }
                case Parser::EXPR: {
                    _expr(std::dynamic_pointer_cast<Parser::ExprNode>(node));
                }
                case Parser::EXPRS: {
                    _exprs(std::dynamic_pointer_cast<Parser::ExprsNode>(node));
                }
                case Parser::PRGM: {
                    _prgm(std::dynamic_pointer_cast<Parser::PrgmNode>(node));
                }
                case Parser::STMT: {
                    _stmt(std::dynamic_pointer_cast<Parser::StmtNode>(node));
                }
                case Parser::STMTS: {
                    _stmts(std::dynamic_pointer_cast<Parser::StmtsNode>(node));
                }
                case Parser::STRING: {
                    _string(std::dynamic_pointer_cast<Parser::StringNode>(node));
                }
                case Parser::TYPE: {
                    _type(std::dynamic_pointer_cast<Parser::TypeNode>(node));
                }
                case Parser::VARIABLE: {
                    _variable(std::dynamic_pointer_cast<Parser::VariableNode>(node));
                }
            }
        }
    }

    void FlowController::_binding(Parser::BindingNodePtr node) {
    }
    void FlowController::_bindings(Parser::BindingsNodePtr node) {
    }
    void FlowController::_cmd(Parser::CmdNodePtr node) {
    }
    void FlowController::_expr(Parser::ExprNodePtr node) {
    }
    void FlowController::_exprs(Parser::ExprsNodePtr node) {
    }
    void FlowController::_prgm(std::shared_ptr<Parser::PrgmNode> node) {
    }
    void FlowController::_stmt(Parser::StmtNodePtr node) {
    }
    void FlowController::_stmts(Parser::StmtsNodePtr node) {
    }
    void FlowController::_string(Parser::StringNodePtr node) {
    }
    void FlowController:: _type(Parser::TypeNodePtr node) {
    }
    void FlowController:: _variable(Parser::VariableNodePtr node) {
    }

}  // namespace FlowController
