/**
 * @file flow_controller.hpp
 * @brief Definition for flow controller components
 */

#ifndef FLOW_CONTROLLER_HPP
#define FLOW_CONTROLLER_HPP

#include "source/builtin_functions/functions.hpp"
#include "source/flow_controller/types.hpp"
#include "source/job_runner/job_runner.hpp"
#include "source/parser/ast_node.hpp"
#include "source/parser/parser.hpp"
#include "source/symbol_table/symbol_table_organizer.hpp"
#include "source/type_checker/type.hpp"
#include "source/util/io.hpp"
#include "source/util/print.hpp"

#include <cmath>
#include <memory>
#include <string>

namespace FlowController {
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
         * @param nodes An AST
         */
        FlowController(Parser::ASTNodeList& nodes);

        /**
         * @brief Evaluate the AST nodes
         */
        void runtime();

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
        void _setVariable(const std::string& name, const CommanderTypePtr& value);

        /**
         * @brief Get a variable from the symbol table.
         * @param name The name of the symbol
         * @returns The value of the symbol
         */
        CommanderTypePtr _getVariable(const std::string& name);

        /**
         * @brief Run a command
         * @param process The command to run
         */
        static JobRunner::JobInfo _runCommand(JobRunner::ProcessPtr process);

        /**
         * @brief Helper to interpret binding nodes
         * @param node The binding node
         */
        void _binding(const Parser::BindingNodePtr& node);

        /**
         * @brief Helper to interpret bindings nodes.
         * @param node The binding nodes to interpret
         */
        void _bindings(const Parser::BindingsNodePtr& nodes);

        /**
         * @brief Parse the list of ASTNodePtr from a CMD node to a list of strings.
         * @param args The list of ASTNodes to get strings from
         * @return The list of strings, used as args for a command
         */
        std::vector<std::string> _parseArguments(const std::vector<Parser::ASTNodePtr>& args);

        /**
         * @brief Parse the output from the Job Runner to something a CommanderTuple takes
         * @param info The job output to parse
         * @return A vector of [CommanderString, CommanderString, CommanderInt]
         */
        static std::vector<CommanderTypePtr> _parseJobReturnInfo(const JobRunner::JobInfo& info);

        /**
         * @brief Get the jobs from a PipeCmdNode
         * @details When doing depth-first search, the order we encounter leaf nodes is
         *          the correct order (This depends greatly on how the parser outputs the AST).
         * @param node A node in the PipeCmdNode tree
         * @param jobs The vector to place the jobs in
         */
        void _getJobs(const Parser::CmdNodePtr& node, std::vector<Parser::CmdCmdNodePtr>& jobs);

        /**
         * @brief Helper to interpret cmd nodes
         * @param node The command node
         * @param saveInfo Is this a command which we want to save return information?
         */
        CommanderTypePtr _cmd(const Parser::CmdNodePtr& node, bool saveInfo = false);

        /**
         * @brief Helper to interpret expr nodes
         * @param node The expression node
         */
        CommanderTypePtr _expr(const Parser::ExprNodePtr& node);

        /**
         * @brief Helper to interpret exprs nodes
         * @param nodes The expression nodes
         */
        void _exprs(const Parser::ExprsNodePtr& nodes);

        /**
         * @brief Helper to interpret prgm nodes
         * @param The program node
         */
        void _prgm(const std::shared_ptr<Parser::PrgmNode>& node);

        /**
         * @brief Helper to interpret stmt nodes
         * @details Some statements need to return some generic value;
         * for example the body of a function call (return statement).
         * @param node The statement node
         * @returns A Commander Value or nullptr
         */
        CommanderTypePtr _stmt(const Parser::StmtNodePtr& node);

        /**
         * @brief Helper to interpret stmts nodes
         * @param nodes The statement nodes
         */
        void _stmts(const Parser::StmtsNodePtr& nodes);

        /**
         * @brief Helper to interpret string nodes
         * @param node The string node
         */
        std::string _string(const Parser::StringNodePtr& node);

        /**
         * @brief Helper to interpret types nodes
         * @param types The types to interpret
         */
        void _types(const Parser::TypesNodePtr& types);

        /**
         * @brief Helper to interpret type nodes
         */
        void _type(const Parser::TypeNodePtr&);

        /**
         * @brief Helper to interpret variable nodes
         * @param node The variable node
         */
        void _variable(const Parser::VariableNodePtr& node);

        /**
         * @brief Helper to interpret unary operations
         * @param unOp The unary operator expression
         */
        CommanderTypePtr _unaryOp(std::shared_ptr<Parser::UnOpExprNode>& unOp);

        /**
         * @brief Helper to interpret binary operations
         * @param binOp The binary operator expression
         */
        CommanderTypePtr _binaryOp(std::shared_ptr<Parser::BinOpExprNode>& binOp);
    };

}  // namespace FlowController
#endif
