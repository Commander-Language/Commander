/**
 * @file transpiler.hpp
 * @brief Definition for bash transpiler
 */

#ifndef BASH_TRANSPILER_HPP
#define BASH_TRANSPILER_HPP

#include "source/parser/ast_node.hpp"
#include "source/parser/parser.hpp"
#include "source/type_checker/variable_table.hpp"
#include "source/util/commander_exception.hpp"
#include "source/util/constants.hpp"
#include <cmath>
#include <map>
#include <set>
#include <stack>

namespace BashTranspiler {

    /**
     * @brief Class that transpiles commander into Bash
     */
    class BashTranspiler {
    public:
        /**
         * @brief constructor
         *
         * @param symbolTable The symbol table from the type checker
         */
        BashTranspiler(const TypeChecker::VariableTablePtr& symbolTable);

        /**
         * @brief Evaluate the AST nodes
         *
         * @return The transpiled Bash code
         */
        std::string transpile(const Parser::ASTNodeList& nodes);

    private:
        /**
         * @brief The number of spaces per indent
         */
        int INDENT_SIZE = 4;

        /**
         * @brief Contains all the lines in the transpiler file
         */
        std::vector<std::string> _lines;

        /**
         * @brief Set of strings for the aliases in the program
         */
        std::set<std::string> _aliases;

        /**
         * @brief Function scopes
         */
        std::stack<std::map<std::string, std::string>> _scopes;

        /**
         * @brief Buffer that represents the current line being written
         */
        std::stringstream _buffer;

        /**
         * @brief Represents number of indents
         */
        int _indent = 0;

        /**
         * @brief The symbol table
         */
        TypeChecker::VariableTablePtr _table;

        /**
         * @brief Transpile a singular AST node recursively
         */
        void _transpile(const Parser::ASTNodePtr& astNode);

        /**
         * @brief Writes the line in the buffer to the lines vector
         */
        void _writeLine();

        /**
         * @brief Resets the current line
         */
        void _resetLine();

        /**
         * @brief Increments the indent
         */
        void _incrementIndent();

        /**
         * @brief Decrements the indent
         */
        void _decrementIndent();

        /**
         * @brief Pushes new scope on top of the stack
         */
        void _pushScope(const std::map<std::string, std::string>& scope, const TypeChecker::VariableTablePtr& table);

        /**
         * @brief Pops top scope of variables off the stack
         */
        void _popScope();

        /**
         * @biref Transpiles op set nodes
         * @param binop Binop op set node
         */
        void _transpileOpSetNode(const Parser::BinOpExprNodePtr& binop);

        /**
         * @brief Create BC binop expression
         */
        void _createBCBinopExpression(const Parser::BinOpExprNodePtr& binop, const std::string& op, const bool& useLib);

        /**
         * @brief Create BC function expression
         */
        void _createBCFunctionExpression(const Parser::ExprNodePtr& expr, const std::string& func);
    };

}  // namespace BashTranspiler
#endif