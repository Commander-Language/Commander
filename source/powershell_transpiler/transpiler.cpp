#include "transpiler.hpp"
#include "source/parser/parser.hpp"
#include "source/type_checker/type.hpp"
#include "source/util/commander_exception.hpp"

#include <utility>
namespace PowerShellTranspiler {
    PowerShellTranspiler::PowerShellTranspiler(Parser::ASTNodeList nodes) : _ast(std::move(nodes)) {}
    std::string PowerShellTranspiler::transpile() {
        for (auto& node : _ast) {
            switch (Parser::getAbstractNodeType(node->nodeType())) {
                case Parser::BINDING: {
                    _binding(std::static_pointer_cast<Parser::BindingNode>(node));
                    break;
                }
                case Parser::BINDINGS: {
                    _bindings(std::static_pointer_cast<Parser::BindingsNode>(node));
                    break;
                }
                case Parser::EXPRS: {
                    _exprs(std::static_pointer_cast<Parser::ExprsNode>(node));
                    break;
                }
                case Parser::PRGM: {
                    _prgm(std::static_pointer_cast<Parser::PrgmNode>(node));
                    break;
                }
                case Parser::STMTS: {
                    _stmts(std::static_pointer_cast<Parser::StmtsNode>(node));
                    break;
                }
                case Parser::STRING: {
                    _string(std::static_pointer_cast<Parser::StringNode>(node));
                    break;
                }
                case Parser::STRING_EXPRS: {
                    _string_exprs(std::static_pointer_cast<Parser::StringExprsNode>(node));
                    break;
                }
                case Parser::TYPES: {
                    _types(std::static_pointer_cast<Parser::TypesNode>(node));
                    break;
                }
                case Parser::CMD: {
                    _cmd(std::static_pointer_cast<Parser::CmdNode>(node));
                    break;
                }
                case Parser::BASIC_CMD: {
                    _basic_cmd(std::static_pointer_cast<Parser::BasicCmdNode>(node));
                    break;
                }
                case Parser::PIPE_CMD: {
                    _pipe_cmd(std::static_pointer_cast<Parser::PipeCmdNode>(node));
                    break;
                }
                case Parser::ASYNC_CMD: {
                    _async_cmd(std::static_pointer_cast<Parser::AsyncCmdNode>(node));
                    break;
                }
                case Parser::LVALUE: {
                    _lvalue(std::static_pointer_cast<Parser::LValueNode>(node));
                    break;
                }
                case Parser::EXPR: {
                    _expr(std::static_pointer_cast<Parser::ExprNode>(node));
                    break;
                }
                case Parser::STMT: {
                    _stmt(std::static_pointer_cast<Parser::StmtNode>(node));
                    break;
                }
                case Parser::TYPE: {
                    _type(std::static_pointer_cast<Parser::TypeNode>(node));
                    break;
                }
                default: {
                    throw Util::CommanderException("Unknown node type found while transpiling "
                                                   + Parser::nodeTypeToString(node->nodeType()));
                }
            }
        }
        return _output;
    }

    void PowerShellTranspiler::_binding(const Parser::BindingNodePtr node) { _write("$" + node->variable + " "); }
    void PowerShellTranspiler::_bindings(const Parser::BindingsNodePtr node) {
        for (auto& binding : node->bindings) { _binding(binding); }
    }
    void PowerShellTranspiler::_exprs(const Parser::ExprsNodePtr node) {
        for (auto& expr : node->exprs) { _expr(expr); }
    }
    void PowerShellTranspiler::_prgm(const Parser::PrgmNodePtr node) { _stmts(node->stmts); }
    void PowerShellTranspiler::_stmts(const Parser::StmtsNodePtr node) {
        for (auto& stmt : node->stmts) { _stmt(stmt); }
    }
    void PowerShellTranspiler::_string(const Parser::StringNodePtr node) {
        if (node->isLiteral()) {
            _write("\"" + node->literal + "\"");
        } else {
            _string_exprs(node->expressions);
        }
    }
    void PowerShellTranspiler::_string_exprs(const Parser::StringExprsNodePtr node) {
        for (auto& expr : node->expressions) _expr(expr);
    }
    void PowerShellTranspiler::_types(const Parser::TypesNodePtr node) {
        for (auto& type : node->types) { _type(type); }
    }
    void PowerShellTranspiler::_cmd(const Parser::CmdNodePtr node) {}
    void PowerShellTranspiler::_basic_cmd(const Parser::BasicCmdNodePtr node) {}
    void PowerShellTranspiler::_pipe_cmd(const Parser::PipeCmdNodePtr node) {}
    void PowerShellTranspiler::_async_cmd(const Parser::AsyncCmdNodePtr node) {}
    void PowerShellTranspiler::_lvalue(const Parser::LValueNodePtr node) {}
    void PowerShellTranspiler::_expr(const Parser::ExprNodePtr node) {}
    void PowerShellTranspiler::_stmt(const Parser::StmtNodePtr node) {}
    void PowerShellTranspiler::_type(const Parser::TypeNodePtr node) {}

    void PowerShellTranspiler::_writeLine(const std::string& str) {
        _output.append(str);
        _output.append("\n");
    }
    void PowerShellTranspiler::_write(const std::string& str) { _output.append(str); }
}  // namespace PowerShellTranspiler
