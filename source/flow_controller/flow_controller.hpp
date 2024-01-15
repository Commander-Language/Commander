#ifndef FLOW_CONTROLLER_HPP
#define FLOW_CONTROLLER_HPP

#include "source/parser/parser.hpp"
#include "source/symbol_table/symbol_table_organizer.hpp"

#include <any>

namespace FlowController {
    class FlowController {
    public:
        FlowController(Parser::ASTNodeList&);
        void setVariable(std::string name, std::any value);
        void runCommand();
        void runtime();

    private:
        Parser::ASTNodeList _nodes;
        SymbolTableOrganizer _symbolTable;

        void _binding(Parser::BindingNodePtr);
        void _bindings(Parser::BindingsNodePtr);
        void _cmd(Parser::CmdNodePtr);
        void _expr(Parser::ExprNodePtr);
        void _exprs(Parser::ExprsNodePtr);
        void _prgm(std::shared_ptr<Parser::PrgmNode>);
        void _stmt(Parser::StmtNodePtr);
        void _stmts(Parser::StmtsNodePtr);
        void _string(Parser::StringNodePtr);
        void _type(Parser::TypeNodePtr);
        void _variable(Parser::VariableNodePtr);
    };
}  // namespace FlowController
#endif
