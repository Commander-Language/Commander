/**
 * @file source/parser/parser.hpp
 * @brief Defines the `Parser` class.
 *
 */

#ifndef PARSER_HPP
#define PARSER_HPP

#include <stack>

#include "ast_node.hpp"
#include "parse_table.hpp"

#include "source/lexer/lexer.hpp"

namespace Parser {

    /**
     * @brief Parses a series of tokens into a series of AST nodes.
     * @details
     *     This is an LR(1) parser.
     *
     *     Production rules are stated below.
     *     Note that this list deviates from the regular grammar specification syntax,
     *     which uses terms like `<expr>` and `<int>`;
     *     this list is more specific, in that AST nodes and tokens are represented
     *     as `<NodeType>` and `[TOKEN_TYPE]`, respectively.
     *     This is for clarity, because it is convenient to differentiate
     *     between specific AST node types and tokens when it comes to implementation.
     *     Furthermore, in the case of repeated or optional terms in a production rule,
     *     a regular-expression-like syntax is used:
     *         * A group is created with parentheses.
     *         * Any number of appearances of a group is specified with an asterisk (`*`).
     *         * One or more appearances of a group is specified with a plus (`+`).
     *         * A conditional appearance of a group is specified with a question mark (`?`).
     *         * Any one of a few different options in a group is specified with a vertical pipe (`|`).
     *
     *     Supported production rules:
     *
     *     To-be-done production rules:
     *         Statements:
     *           * <IfStmt>         :  [IF] [LPAREN] <Expr> [RPAREN] <Stmt> ([ELSE] <Stmt>)?
     *           * <ForStmt>        :  [FOR] [LPAREN] <Stmt> [SEMICOLON] <Expr> [SEMICOLON] <Stmt> [RPAREN] <Stmt>
     *           * <WhileStmt>      :  [WHILE] [LPAREN] <Expr> [RPAREN] <Stmt>
     *           * <DoWhileStmt>    :  [DO] <Stmt> [WHILE] [LPAREN] <Expr> [RPAREN]
     *
     *           * <ScopeStmt>      :  [LCURLY] (<Stmt>)* [RCURLY]
     *
     *           * <CmdStmt>        :  <Cmd>
     *           * <AliasStmt>      :  [ALIAS] [VARIABLE] [EQUALS] <Cmd>
     *
     *         Expressions:
     *           * <IntExpr>        :  [INTVAL]
     *           * <FloatExpr>      :  [FLOATVAL]
     *           * <StringExpr>     :  [STRINGVAL]
     *           * <CmdStringExpr>  :  [CMDSTRINGVAL]
     *           * <BoolExpr>       :  [TRUE]
     *           * <BoolExpr>       :  [FALSE]
     *           * <ArrayExpr>      :  [LSQUARE] ((<Expr> [COMMA])* <Expr>)? [RSQUARE]
     *           * <TupleExpr>      :  [LPAREN] ((<Expr> [COMMA])* <Expr>)? [RPAREN]
     *           * <LambdaExpr>     :  [LPAREN] ((<Binding> [COMMA])* <Binding>)? [RPAREN]
     *                                 ([COLON] <Type>)?
     *                                 [LCURLY]
     *                                 (<Stmt>)*
     *                                 [RCURLY]
     *           * <VarExpr>        :  [VARIABLE]
     *
     *           * <TernaryExpr>    :  <Expr> [QUESTION] <Expr> [COLON] <Expr>
     *           * <CallExpr>       :  [VARIABLE] [LPAREN] ((<Expr> [COMMA])* <Expr>)? [RPAREN]
     *           * <UnOpExpr>       :  [OP] <Expr>
     *           * <BinOpExpr>      :  <Expr> [OP] <Expr>
     *           * <ScanExpr>       :  [SCAN] [STRINGVAL]
     *           * <EchoExpr>       :  [ECHO] <STRINGVAL>
     *           * <CmdExpr>        :  [BACKTICK] <Cmd> [BACKTICK]
     *
     *         Commands:
     *           * <Cmd>            :  ([CMDSTRINGVAL] | [STRINGVAL] | [VARIABLE])+
     *           * <AsyncCmd>       :  <Cmd> [QUESTION]
     *           * <PipeCmd>        :  <Cmd> [PIPE] <Cmd>
     *
     *         Types:
     *           * <IntType>        :  [INT]
     *           * <FloatType>      :  [FLOAT]
     *           * <ArrayType>
     *           * <TupleType>
     */
    class Parser {
    public:
        /**
         * @brief Class constructor.
         * @details Does a lot of initialization work under the hood. Avoid calling more than once.
         *
         */
        Parser();

        /**
         * @brief Parses the given list of tokens into a list of AST nodes.
         *
         * @param tokens The tokens to parse.
         * @return The parsed AST nodes.
         */
        ASTNodeList parse(const lexer::TokenList& tokens);

    private:
        /**
         * @brief The table of state transitions to follow.
         *
         */
        ParseTable _parseTable;
    };

}  //  namespace Parser

#endif  //  PARSER_HPP
