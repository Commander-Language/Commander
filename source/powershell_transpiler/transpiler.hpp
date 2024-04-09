/**
 * @file transpiler.hpp
 * @brief Definition of the PowerShellTranspiler class
 */

#ifndef COMMANDER_POWERSHELL_TRANSPILER_HPP
#define COMMANDER_POWERSHELL_TRANSPILER_HPP
#include "source/parser/ast_node.hpp"
namespace PowerShellTranspiler {
    /**
     * @brief Class that transpiles Commander source to PowerShell script
     */
    class PowerShellTranspiler {
    public:
        /**
         * @brief Constructor
         * @param nodes - The AST nodes to evaluate
         */
        PowerShellTranspiler(Parser::ASTNodeList nodes);

        /**
         * @brief Start transpiling
         * @return The transpiled PowerShell code
         */
        std::string transpile();

    private:
        /**
         * @brief The Abstract Syntax Tree to evaluate
         */
        Parser::ASTNodeList _ast;

        /**
         * @brief The PowerShell code we will output at the end of transpiling
         */
        std::string _output;

        /**
         * @brief A counter of the number of times we transpiled a `timeout`
         * @details Used to create a unique identifier
         */
        unsigned int _timeoutCount = 1;

        /**
         * @brief The current indent level
         */
        size_t _indentLevel = 0;

        /**
         * @brief The size of an indent
         */
        const size_t _indentSize = 4;

        /**
         * @brief Should we indent
         */
        bool _indent = true;

        /**
         * @brief Evaluate a 'binding' node
         * @param node - The 'binding' node
         */
        void _binding(const Parser::BindingNodePtr& node);

        /**
         * @brief Evaluate a 'bindings' node
         * @param node - The 'bindings' node
         */
        void _bindings(const Parser::BindingsNodePtr& node);

        /**
         * @brief Evaluate a 'exprs' node
         * @param node - The 'exprs' node
         */
        void _exprs(const Parser::ExprsNodePtr& node);

        /**
         * @brief Evaluate a 'prgm' node
         * @param node - The 'prgm' node
         */
        void _prgm(const Parser::PrgmNodePtr& node);

        /**
         * @brief Evaluate a 'stmts' node
         * @param node - The 'stmts' node
         */
        void _stmts(const Parser::StmtsNodePtr& node);

        /**
         * @brief Evaluate a 'string' node
         * @param node - The 'string' node
         */
        void _string(const Parser::StringNodePtr& node);

        /**
         * @brief Evaluate a 'string_expr' node
         * @param node - The 'string_expr' node
         */
        void _string_exprs(const Parser::StringExprsNodePtr& node);

        /**
         * @brief Evaluate a 'types' node
         * @param node - The 'types' node
         */
        void _types(const Parser::TypesNodePtr& node);

        /**
         * @brief Evaluate a 'cmd' node
         * @param node - The 'cmd' node
         */
        void _cmd(const Parser::CmdNodePtr& node);

        /**
         * @brief Evaluate a 'lvalue' node
         * @param node - The 'lvalue' node
         */
        void _lvalue(const Parser::LValueNodePtr& node);

        /**
         * @brief Evaluate a 'expr' node
         * @param node - The 'expr' node
         */
        void _expr(const Parser::ExprNodePtr& node);

        /**
         * @brief Evaluate a 'stmt' node
         * @param node - The 'stmt' node
         */
        void _stmt(const Parser::StmtNodePtr& node, bool skipScope = false);

        /**
         * @brief Evaluate a 'type' node
         * @param node - The 'type' node
         */
        void _type(const Parser::TypeNodePtr& node);

        /**
         * @brief Evaluate a 'binOpExpr' node
         * @param node - The 'binOpExpr' node
         */
        void _binary(const Parser::BinOpExprNodePtr& node);

        /**
         * @brief Evaluate a 'unOpExpr' node
         * @param node - The 'unOpExpr' node
         */
        void _unary(const Parser::UnOpExprNodePtr& node);

        /**
         * @brief Evaluate an ast node
         * @param node - The ast node
         */
        void _node(const Parser::ASTNodePtr& node);

        /**
         * @brief A helper to increase the indent level
         */
        void _increaseIndent();

        /**
         * @brief A helper to decrease the indent level
         */
        void _decreaseIndent();

        /**
         * @brief A helper to turn on indentation
         */
        void _indentOn();

        /**
         * @brief A helper to turn off indentation
         */
        void _indentOff();

        /**
         * @brief A helper to write a line to output
         * @details Interacts with the _indent flag to
         *          add an indentation or not
         */
        void _writeLine(const std::string& str);

        /**
         * @brief A helper to write a line to output
         * @details Interacts with the _indent flag to
         *          add an indentation or not
         */
        void _write(const std::string& str);
    };
}  // namespace PowerShellTranspiler
#endif