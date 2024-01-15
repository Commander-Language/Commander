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
        void _doBindings(Parser::BindingNode);
        Parser::ASTNodeList _nodes;
        SymbolTableOrganizer _symbolTable;
    };
}  // namespace FlowController
#endif
