/**
 * @file
 */
#ifndef COMMANDER_POWERSHELL_TRANSPILER
#define COMMANDER_POWERSHELL_TRANSPILER
#include "source/parser/ast_node.hpp"
namespace PowerShellTranspiler {
    class PowerShellTranspiler {
    public:
        PowerShellTranspiler(Parser::ASTNodeList nodes);
        std::string transpile();

    private:
        Parser::ASTNodeList _ast;
        std::string _output;
        void _binding(Parser::BindingNodePtr node);
        void _bindings(Parser::BindingsNodePtr node);
        void _exprs(Parser::ExprsNodePtr  node);
        void _prgm(Parser::PrgmNodePtr  node);
        void _stmts(Parser::StmtsNodePtr  node);
        void _string(Parser::StringNodePtr  node);
        void _string_exprs(Parser::StringExprsNodePtr  node);
        void _types(Parser::TypesNodePtr  node);
        void _cmd(Parser::CmdNodePtr  node);
        void _basic_cmd(Parser::BasicCmdNodePtr  node);
        void _pipe_cmd(Parser::PipeCmdNodePtr  node);
        void _async_cmd(Parser::AsyncCmdNodePtr  node);
        void _lvalue(Parser::LValueNodePtr  node);
        void _expr(Parser::ExprNodePtr  node);
        void _stmt(Parser::StmtNodePtr  node);
        void _type(Parser::TypeNodePtr  node);

        void _writeLine(const std::string& str);
        void _write(const std::string& str);
    };
}  // namespace PowerShellTranspiler
#endif