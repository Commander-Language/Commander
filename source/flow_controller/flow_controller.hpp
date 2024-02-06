/**
 * @file flow_controller.hpp
 * @brief Definition for flow controller components
 */

#ifndef FLOW_CONTROLLER_HPP
#define FLOW_CONTROLLER_HPP

#include "source/parser/parser.hpp"
#include "source/symbol_table/symbol_table_organizer.hpp"

#include <any>
#include <tuple>
#include <iostream>

namespace FlowController {

    template<typename TType>
    /**
     * @brief A Commander array representation
     */
    using CommanderArray = std::vector<TType>;
    /**
     * @brief A Commander bool representation
     */
    using CommanderBool = bool;
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
        CommanderLambda(std::vector<Parser::BindingNodePtr>, Parser::StmtNodePtr);
        std::vector<Parser::BindingNodePtr> bindings;
        Parser::StmtNodePtr body;
        // TODO: save return type
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

        /**
         * @brief Check if flow controller could find a varible
         * @returns True or False
         */
        bool hasVariable(std::string);

        /**
         * @brief Get value of variable
         */
        CommanderInt getVariableValue(std::string);

    private:
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
         * @param name The name of the symbol
         * @param value The value of the symbol
         */
        void _setVariable(std::string name, std::any value);

        /**
         * @brief Get a variable from the symbol table.
         * @param name The name of the symbol
         * @returns The value of the symbol
         */
        std::any _getVariable(const std::string& name);

        /**
         * @brief Run a command
         */
        void _runCommand();

        /**
         * @brief Get a String representation of a Commander type value
         */
        static std::string _commanderTypeToString(std::any);

        /**
         * @brief Helper to interpret binding nodes
         */
        void _binding(const Parser::BindingNodePtr&);
        /**
         * @brief Helper to interpret bindings nodes
         */
        void _bindings(const Parser::BindingsNodePtr&);
        /**
         * @brief Helper to interpret cmd nodes
         */
        void _cmd(const Parser::CmdNodePtr&);
        /**
         * @brief Helper to interpret expr nodes
         */
        std::any _expr(const Parser::ExprNodePtr&);
        /**
         * @brief Helper to interpret exprs nodes
         */
        void _exprs(const Parser::ExprsNodePtr&);
        /**
         * @brief Helper to interpret prgm nodes
         */
        void _prgm(const std::shared_ptr<Parser::PrgmNode>&);
        /**
         * @brief Helper to interpret stmt nodes
         * @details Some statements need to return some generic value;
         * for example the body of a function call, so return value or nullptr if unnecessary.
         * @returns Generic value or nullptr if not needed.
         */
        std::any _stmt(const Parser::StmtNodePtr&);
        /**
         * @brief Helper to interpret stmts nodes
         */
        void _stmts(const Parser::StmtsNodePtr&);
        /**
         * @brief Helper to interpret string nodes
         */
        std::string _string(const Parser::StringNodePtr&);
        /**
         * @brief Helper to interpret type nodes
         */
        void _type(const Parser::TypeNodePtr&);
        /**
         * @brief Helper to interpret variable nodes
         */
        void _variable(const Parser::VariableNodePtr&);

        /**
         * @brief Helper to interpret unary operations
         */
        std::any _unaryOp(std::shared_ptr<Parser::UnOpExprNode>&);

        /**
         * @brief Helper to interpret binary operations
         */
        std::any _binaryOp(std::shared_ptr<Parser::BinOpExprNode>& binOp);
    };
}  // namespace FlowController
#endif
