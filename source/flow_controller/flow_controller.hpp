/**
* @file flow_controller.hpp
* @brief Definition for flow controller components
*/

#ifndef FLOW_CONTROLLER_HPP
#define FLOW_CONTROLLER_HPP

#include "source/parser/parser.hpp"
#include "source/symbol_table/symbol_table_organizer.hpp"

#include <any>

namespace FlowController {
    /**
     * @brief A Commander tuple representation
     */
    using CommanderTuple = std::vector<std::any>;
    /**
     * @brief A Commander int representation
     */
    using CommanderInt = int64_t;
    /**
     * @brief A commander lambda representation
     */
    struct CommanderLambda {
    public:
        CommanderLambda();
    private:

    };

    /**
     * @brief Interpret the AST Nodes given from the Parser
     * @details
     *    The responsibilities of the Flow Controller will include interpreting the AST nodes,
     *    keeping track of variables/functions in a symbol table, and calling terminal programs.
     */
    class FlowController {
    public:
        /**
         * @brief Class Constructor
         */
        FlowController(Parser::ASTNodeList&);
        /**
         * @brief Parse the AST Nodes
         */
        void runtime();

    private:
       /**
        * @brief
        */
        CommanderLambda findFunctionOrLambda(const Parser::ExprNodePtr&);

        /**
         * @brief The AST nodes in a list
         */
        Parser::ASTNodeList _nodes;

        /**
         * @brief The symbol table
         */
        SymbolTableOrganizer _symbolTable;

        /**
         * @brief Set a variable in the symbol table.
         * @details Sets a variable in the symbol table
         * @param name The name of the symbol
         * @param value The value of the symbol
         */
        void setVariable(std::string name, std::any value);

        /**
         * @brief Set a variable in the symbol table.
         * @details Sets a variable in the symbol table
         * @param name The name of the symbol
         * @param value The value of the symbol
         */
        std::any getVariable(std::string name);

        /**
         * @brief Run a command
         */
        void runCommand();

        /**
         * @brief Helper to interpret binding nodes
         */
        void _binding(Parser::BindingNodePtr);
        /**
         * @brief Helper to interpret bindings nodes
         */
        void _bindings(Parser::BindingsNodePtr);
        /**
         * @brief Helper to interpret cmd nodes
         */
        void _cmd(Parser::CmdNodePtr);
        /**
         * @brief Helper to interpret expr nodes
         */
        std::any _expr(Parser::ExprNodePtr);
        /**
         * @brief Helper to interpret exprs nodes
         */
        void _exprs(Parser::ExprsNodePtr);
        /**
         * @brief Helper to interpret prgm nodes
         */
        void _prgm(std::shared_ptr<Parser::PrgmNode>);
        /**
         * @brief Helper to interpret stmt nodes
         */
        void _stmt(Parser::StmtNodePtr);
        /**
         * @brief Helper to interpret stmts nodes
         */
        void _stmts(Parser::StmtsNodePtr);
        /**
         * @brief Helper to interpret string nodes
         */
        void _string(Parser::StringNodePtr);
        /**
         * @brief Helper to interpret type nodes
         */
        void _type(Parser::TypeNodePtr);
        /**
         * @brief Helper to interpret variable nodes
         */
        void _variable(Parser::VariableNodePtr);
    };
}  // namespace FlowController
#endif
