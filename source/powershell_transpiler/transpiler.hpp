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
        unsigned int _timeoutCount = 1;

        size_t _indentLevel = 0;
        const size_t _indentSize = 4;

        bool _indent = true;

        void _binding(const Parser::BindingNodePtr& node);
        void _bindings(const Parser::BindingsNodePtr& node);
        void _exprs(const Parser::ExprsNodePtr& node);
        void _prgm(const Parser::PrgmNodePtr& node);
        void _stmts(const Parser::StmtsNodePtr& node);
        void _string(const Parser::StringNodePtr& node);
        void _string_exprs(const Parser::StringExprsNodePtr& node);
        void _types(const Parser::TypesNodePtr& node);
        void _cmd(const Parser::CmdNodePtr& node);
        void _lvalue(const Parser::LValueNodePtr& node);
        void _expr(const Parser::ExprNodePtr& node);
        void _stmt(const Parser::StmtNodePtr& node, bool skipScope = false);
        void _type(const Parser::TypeNodePtr& node);
        void _binary(const Parser::BinOpExprNodePtr& node);
        void _unary(const Parser::UnOpExprNodePtr& node);
        void _node(const Parser::ASTNodePtr& node);

        void _increaseIndent();
        void _decreaseIndent();
        void _indentOn();
        void _indentOff();
        void _writeLine(const std::string& str);
        void _write(const std::string& str);
    };
}  // namespace PowerShellTranspiler
#endif