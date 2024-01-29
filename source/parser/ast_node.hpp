/**
 * @file source/parser/ast_node.hpp
 * @brief Defines the `ASTNode` class and subclasses.
 *
 */

#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include "source/type_checker/type.hpp"
#include <cstdint>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace Parser {

    /**
     * @brief The overarching type of an AST node.
     *
     */
    enum ASTNodeType : uint8_t {
        BINDING,
        BINDINGS,
        CMD,
        PIPE_CMD,
        ASYNC_CMD,
        INT_EXPR,
        FLOAT_EXPR,
        STRING_EXPR,
        BOOL_EXPR,
        VAR_EXPR,
        ARRAY_EXPR,
        ARRAY_INDEX_EXPR,
        TUPLE_EXPR,
        TUPLE_INDEX_EXPR,
        TERNARY_EXPR,
        UNOP_EXPR,
        BINOP_EXPR,
        CALL_EXPR,
        LAMBDA_EXPR,
        CMD_EXPR,
        EXPR,
        EXPRS,
        PRGM,
        IF_STMT,
        FOR_STMT,
        WHILE_STMT,
        DO_WHILE_STMT,
        RETURN_STMT,
        SCOPE_STMT,
        CMD_STMT,
        EXPR_STMT,
        ALIAS_STMT,
        STMT,
        STMTS,
        STRING,
        INT_TYPE,
        FLOAT_TYPE,
        BOOL_TYPE,
        STRING_TYPE,
        ARRAY_TYPE,
        TUPLE_TYPE,
        FUNCTION_TYPE,
        TYPE,
        VARIABLE
    };

    /**
     * @brief The types of unary operation expressions
     */
    enum UnOpType : uint8_t { NEGATE, NOT, PRE_INCREMENT, POST_INCREMENT, PRE_DECREMENT, POST_DECREMENT };

    /**
     * @brief Helper method that gets the string representation of the Unop enum type
     * @param unop The unop to get the string version of
     * @return The string of the unop
     */
    std::string unOpToString(UnOpType unop);

    /**
     * @brief The types of binop expressions
     */
    enum BinOpType : uint8_t {
        LESSER,
        GREATER,
        LESSER_EQUAL,
        GREATER_EQUAL,
        MODULO,
        DIVIDE,
        MULTIPLY,
        SUBTRACT,
        ADD,
        EXPONENTIATE,
        AND,
        OR,
        EQUAL,
        SET,
        NOT_EQUAL,
        ADD_EQUAL,
        SUBTRACT_EQUAL,
        MULTIPLY_EQUAL,
        DIVIDE_EQUAL,
        MODULO_EQUAL,
        EXPONENTIATE_EQUAL
    };

    /**
     * @brief Helper method that gets the string representation of the Binop enum type
     * @param binop The binop to get the string version of
     * @return The string of the binop
     */
    std::string binOpToString(BinOpType binop);

    /**
     * @brief Represents a single Abstract Syntax Tree node.
     * @details Subclassed for each AST node type.
     *
     */
    class ASTNode {
    public:
        /**
         * @brief Class destructor.
         * @details Necessary because this is a pure virtual class.
         *
         */
        virtual ~ASTNode() = default;

        /**
         * @brief Reports the type of this AST node.
         *
         * @return The type of this AST node.
         */
        [[nodiscard]] virtual ASTNodeType nodeType() const = 0;

        /**
         * @brief Gets the string representation of the AST node as an s-expression
         *
         * @return The s-expression string of the AST node
         */
        [[nodiscard]] virtual std::string sExpression() const = 0;
    };

    /**
     * @brief A pointer to an AST node.
     *
     */
    using ASTNodePtr = std::shared_ptr<ASTNode>;

    /**
     * @brief A list of AST nodes.
     *
     */
    using ASTNodeList = std::vector<ASTNodePtr>;


    //  ==========================
    //  ||  Overarching types:  ||
    //  ==========================

    /**
     * @brief A type AST node.
     *
     */
    class TypeNode : public ASTNode {
    public:
        /**
         * @brief The type of the node
         */
        TypeChecker::TyPtr type;
        /**
         * @brief Gets the string representation of the type
         *
         * @return The string representation of the type
         */
        [[nodiscard]] std::string getTypeString() const;
    };
    /**
     * @brief A pointer to a type node.
     *
     */
    using TypeNodePtr = std::shared_ptr<TypeNode>;

    /**
     * @brief A binding AST node.
     *
     */
    class BindingNode : public ASTNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param variable The name of the bound variable.
         * @param type The type of the bound variable. (Defaults to `nullptr`.)
         */
        BindingNode(std::string variable, TypeNodePtr type = nullptr);

        /**
         * The variable name of the associated binding
         */
        std::string variable;

        /**
         * The (optional) type of the associated binding
         */
        TypeNodePtr type;

        /**
         * @brief Reports the type of this binding node.
         *
         * @return `BINDING` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;
    };
    /**
     * @brief A pointer to a binding node.
     *
     */
    using BindingNodePtr = std::shared_ptr<BindingNode>;

    /**
     * @brief A list of binding AST nodes.
     * @details Zero or more.
     *
     */
    class BindingsNode : public ASTNode {
    public:
        /**
         * @brief Class constructor from a single binding.
         *
         * @param bindings The list of binding nodes that makes up this AST node.
         */
        BindingsNode(BindingNodePtr binding);

        /**
         * @brief Class constructor from a list of bindings, plus one more binding at the end.
         *
         * @param bindings The list of binding nodes at the beginning.
         * @param binding The binding node at the end.
         */
        BindingsNode(const std::vector<BindingNodePtr>& bindings, BindingNodePtr binding);

        /**
         * @brief Reports the type of this binding-list node.
         *
         * @return `BINDINGS` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;

        /**
         * @brief Gets the string representation of the node as an s-expression.
         *
         * @return The s-expression string of the node.
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief The list of binding nodes that makes up this AST node.
         *
         */
        std::vector<BindingNodePtr> bindings;
    };
    /**
     * @brief A pointer to a binding-list node.
     *
     */
    using BindingsNodePtr = std::shared_ptr<BindingsNode>;

    /**
     * @brief A command node.
     *
     */
    class CmdNode : public ASTNode {};

    /**
     * @brief A smart pointer to a command node.
     *
     */
    using CmdNodePtr = std::shared_ptr<CmdNode>;

    /**
     * @brief An expression AST node.
     *
     */
    class ExprNode : public ASTNode {
    public:
        /**
         * @brief The type of the node
         */
        TypeChecker::TyPtr type;
        /**
         * @brief Gets the string representation of the type
         *
         * @return The string representation of the type
         */
        [[nodiscard]] std::string getTypeString() const;
    };
    /**
     * @brief A pointer to an expression node.
     *
     */
    using ExprNodePtr = std::shared_ptr<ExprNode>;

    /**
     * @brief A list of expression AST nodes.
     * @details Zero or more.
     *
     */
    class ExprsNode : public ASTNode {
    public:
        /**
         * @brief Class constructor from a single expression.
         *
         * @param expr The single expression node that makes up this AST node.
         */
        ExprsNode(ExprNodePtr expr);

        /**
         * @brief Class constructor from a list of expressions, plus one more expression.
         *
         * @param exprs The list of expression nodes that makes up this AST node.
         * @param expr The expression to add to the end of the list of expressions.
         */
        ExprsNode(const std::vector<ExprNodePtr>& exprs, ExprNodePtr expr);

        /**
         * @brief Reports the type of this expression-list node.
         *
         * @return `EXPRS` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;

        /**
         * @brief Gets the string representation of the node as an s-expression.
         *
         * @return The s-expression string of the node.
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief The list of expression nodes that makes up this AST node.
         *
         */
        std::vector<ExprNodePtr> exprs;
    };
    /**
     * @brief A pointer to an expression-list node.
     *
     */
    using ExprsNodePtr = std::shared_ptr<ExprsNode>;

    /**
     * @brief A statement AST node.
     *
     */
    class StmtNode : public ASTNode {};

    /**
     * @brief A pointer to a statement node.
     *
     */
    using StmtNodePtr = std::shared_ptr<StmtNode>;

    /**
     * @brief A list of statement AST nodes.
     * @details Zero or more.
     *
     */
    class StmtsNode : public ASTNode {
    public:
        /**
         * @brief Class constructor from a single statement.
         *
         * @param stmt The single statement node that makes up this AST node.
         */
        StmtsNode(StmtNodePtr stmt);

        /**
         * @brief Class constructor from a list of statement, plus one more statement.
         *
         * @param stmts The list of statement nodes that makes up this AST node.
         * @param stmt The statement to add to the end of the list of statements.
         */
        StmtsNode(const std::vector<StmtNodePtr>& stmts, StmtNodePtr stmt);

        /**
         * @brief Reports the type of this expression-list node.
         *
         * @return `STMTS` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;

        /**
         * @brief Gets the string representation of the node as an s-expression.
         *
         * @return The s-expression string of the node.
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief The list of statement nodes that makes up this AST node.
         *
         */
        std::vector<StmtNodePtr> stmts;
    };
    /**
     * @brief A pointer to a statement-list node.
     *
     */
    using StmtsNodePtr = std::shared_ptr<StmtsNode>;

    /**
     * @brief A string AST node.
     * @details It contains a list of string literals and expression node pointers. When interpreting this node, it will
     * take the first string literal, append the value of the first expression to it, then append the second string
     * literal, then append the value of the second expression to it, and so on until it iterates through everything.
     *
     */
    class StringNode : public ASTNode {
    public:
        /**
         * @brief The list of string literals in the string
         *
         */
        std::vector<std::string> literals;

        /**
         * @brief The list of expressions in the string (from string interpolation)
         *
         */
        std::vector<ExprNodePtr> expressions;

        /**
         * @brief Reports the type of this string node.
         *
         * @return `STRING` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;
    };
    /**
     * @brief A pointer to a string node.
     *
     */
    using StringNodePtr = std::shared_ptr<StringNode>;

    /**
     * @brief A program AST node.
     *
     */
    class PrgmNode : public ASTNode {
    public:
        /**
         * @brief All the statements in the program
         */
        std::vector<StmtNodePtr> stmts;

        /**
         * @brief Class constructor.
         *
         * @param stmts The statements that make up the program.
         */
        PrgmNode(const std::vector<StmtNodePtr>& stmts);

        /**
         * @brief Reports the type of this program node.
         *
         * @return `PRGM` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;
    };
    /**
     * @brief A pointer to a program node.
     *
     */
    using PrgmNodePtr = std::shared_ptr<PrgmNode>;

    /**
     * @brief A variable AST node.
     */
    class VariableNode : public ASTNode {};
    /**
     * @brief A pointer to a variable node.
     *
     */
    using VariableNodePtr = std::shared_ptr<VariableNode>;


    //  =================
    //  ||  Commands:  ||
    //  =================

    /**
     * @brief A regular command AST node.
     *
     */
    class CmdCmdNode : public CmdNode {
    public:
        /**
         * The list of arguments for the command, consisting of variable expression nodes and strings nodes
         */
        std::vector<ASTNodePtr> arguments;

        /**
         * @brief Class constructor.
         *
         * @param arguments The arguments for the command
         */
        //  CmdCmdNode();
        CmdCmdNode(std::vector<ASTNodePtr> arguments);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `CMD` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a command node.
     *
     */
    using CmdCmdNodePtr = std::shared_ptr<CmdCmdNode>;

    /**
     * @brief An asynchronous command AST node.
     *
     */
    class AsyncCmdNode : public CmdNode {
    public:
        /**
         * @brief The command to run asynchronously.
         */
        CmdNodePtr cmd;

        /**
         * @brief Class constructor.
         *
         * @param cmd The command to run asynchronously.
         */
        AsyncCmdNode(CmdNodePtr cmd);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `ASYNC_CMD` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a async command node.
     *
     */
    using AsyncCmdNodePtr = std::shared_ptr<AsyncCmdNode>;

    /**
     * @brief A pipe between two command nodes.
     *
     */
    class PipeCmdNode : public CmdNode {
    public:
        /**
         * The left command, whose output will be the input of the right command
         */
        CmdNodePtr leftCmd;

        /**
         * The right command, whose input will be the output of the left command
         */
        CmdNodePtr rightCmd;

        /**
         * @brief Class constructor.
         *
         * @param leftCmd The "source" command on the left. Its output goes into the pipe.
         * @param rightCmd The "destination" command on the right. The pipe goes into its input.
         */
        PipeCmdNode(CmdNodePtr leftCmd, CmdNodePtr rightCmd);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `PIPE_CMD` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a pipe command node.
     *
     */
    using PipeCmdNodePtr = std::shared_ptr<PipeCmdNode>;


    //  ====================
    //  ||  Expressions:  ||
    //  ====================

    /**
     * @brief An integer literal expression AST node.
     *
     */
    class IntExprNode : public ExprNode {
    public:
        /**
         * The integer value of the node
         */
        int64_t value;

        /**
         * @brief Class constructor.
         *
         * @param value The value of this AST node.
         */
        IntExprNode(int64_t value);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `INT_EXPR` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to an int expression node.
     *
     */
    using IntExprNodePtr = std::shared_ptr<IntExprNode>;

    /**
     * @brief A floating-point literal expression AST node.
     */
    class FloatExprNode : public ExprNode {
    public:
        /**
         * The double value of the floating point number of the node
         */
        double value;

        /**
         * @brief Class constructor.
         *
         * @param value The value of this AST node.
         */
        FloatExprNode(double value);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `FLOAT_EXPR` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to an float expression node.
     *
     */
    using FloatExprNodePtr = std::shared_ptr<FloatExprNode>;

    class StringExprNode : public ExprNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param stringNode The string node that makes up this expression.
         */
        StringExprNode(StringNodePtr stringNode);

        /**
         * @brief Gets the string representation of the node as an s-expression.
         *
         * @return The s-expression string of the node.
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `STRING_EXPR` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;

        /**
         * @brief The string node that makes up this expression.
         *
         */
        StringNodePtr stringNode;
    };
    /**
     * @brief A pointer to an string expression node.
     *
     */
    using StringExprNodePtr = std::shared_ptr<StringExprNode>;

    /**
     * @brief A boolean literal expression AST node.
     *
     */
    class BoolExprNode : public ExprNode {
    public:
        /**
         * The boolean value of the node
         */
        bool value;

        /**
         * @brief Class constructor.
         *
         * @param value The value of this AST node.
         */
        BoolExprNode(bool value);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `BOOL_EXPR` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to an bool expression node.
     *
     */
    using BoolExprNodePtr = std::shared_ptr<BoolExprNode>;

    /**
     * @brief A variable reference expression AST node.
     *
     */
    class VarExprNode : public ExprNode {
    public:
        /**
         * The referenced variable.
         */
        VariableNodePtr variable;

        /**
         * @brief Class constructor.
         *
         * @param variable The variable to reference.
         */
        VarExprNode(VariableNodePtr variable);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `VAR_EXPR` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to an variable expression node.
     *
     */
    using VarExprNodePtr = std::shared_ptr<VarExprNode>;

    /**
     * @brief An array literal expression node.
     *
     */
    class ArrayExprNode : public ExprNode {
    public:
        /**
         * The expressions that make up the array's values
         */
        std::vector<ExprNodePtr> expressions;

        /**
         * @brief Class constructor.
         *
         * @param expressions The elements that go inside the array.
         */
        ArrayExprNode(const std::vector<ExprNodePtr>& expressions);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `ARRAY_EXPR` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to an array expression node.
     *
     */
    using ArrayExprNodePtr = std::shared_ptr<ArrayExprNode>;

    /**
     * @brief An array index expression node.
     *
     */
    class ArrayIndexExprNode : public ExprNode {
    public:
        /**
         * The expression being indexed
         */
        ExprNodePtr array;

        /**
         * The expressions that make up the array indices
         */
        std::vector<ExprNodePtr> indexExprs;

        /**
         * @brief Class constructor.
         *
         * @param array The array into which we're indexing.
         * @param expressions The indices inside the square brackets.
         */
        ArrayIndexExprNode(ExprNodePtr array, const std::vector<ExprNodePtr>& indexExprs);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `ARRAY_INDEX_EXPR` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to an array index expression node.
     *
     */
    using ArrayIndexExprNodePtr = std::shared_ptr<ArrayIndexExprNode>;

    /**
     * @brief A tuple literal expression node.
     *
     */
    class TupleExprNode : public ExprNode {
    public:
        /**
         * The expressions that make up the tuple's values
         */
        std::vector<ExprNodePtr> expressions;

        /**
         * @brief Class constructor.
         *
         * @param expressions The elements that go inside the tuple.
         */
        TupleExprNode(const std::vector<ExprNodePtr>& expressions);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `TUPLE_EXPR` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to an tuple expression node.
     *
     */
    using TupleExprNodePtr = std::shared_ptr<TupleExprNode>;

    /**
     * @brief An tuple index expression node.
     *
     */
    class TupleIndexExprNode : public ExprNode {
    public:
        /**
         * The expression being indexed
         */
        ExprNodePtr tuple;

        /**
         * The expression for the tuple index
         */
        ExprNodePtr index;

        /**
         * @brief Class constructor.
         *
         * @param tuple The tuple being indexed.
         * @param index The index into the tuple.
         */
        TupleIndexExprNode(ExprNodePtr tuple, ExprNodePtr index);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `TUPLE_INDEX_EXPR` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a tuple index expression node.
     *
     */
    using TupleIndexExprNodePtr = std::shared_ptr<TupleIndexExprNode>;

    /**
     * @brief A ternary expression node.
     *
     */
    class TernaryExprNode : public ExprNode {
    public:
        /**
         * The expression representing the boolean condition
         */
        ExprNodePtr condition;

        /**
         * The expression that is evaluated when condition is true
         */
        ExprNodePtr trueExpr;

        /**
         * The expression that is evaluated when the condition is false
         */
        ExprNodePtr falseExpr;

        /**
         * @brief Class constructor.
         *
         * @param condition The condition to evaluate.
         * @param trueExpr The expression to evaluate if the condition was true.
         * @param falseExpr The expression to evaluate if the condition was false.
         */
        TernaryExprNode(ExprNodePtr condition, ExprNodePtr trueExpr, ExprNodePtr falseExpr);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `TERNARY_EXPR` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to an ternary expression node.
     *
     */
    using TernaryExprNodePtr = std::shared_ptr<TernaryExprNode>;

    /**
     * @brief Unary operation expression node.
     */
    class UnOpExprNode : public ExprNode {
    public:
        /**
         * @brief The type of unary operation being done.
         */
        UnOpType opType;

        /**
         * @brief The expression being operated on.
         */
        ExprNodePtr expr;

        /**
         * @brief Class constructor for a unary operation on an expression.
         *
         * @param opType The unary operation type being performed.
         * @param expr The expression on which the unary operation is being performed.
         */
        UnOpExprNode(UnOpType opType, ExprNodePtr expr);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `UNOP_EXPR` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a unary operation node.
     *
     */
    using UnOpExprNodePtr = std::shared_ptr<UnOpExprNode>;

    /**
     * @brief Binary operation expression node.
     */
    class BinOpExprNode : public ExprNode {
    public:
        /**
         * The type of binary operation
         */
        BinOpType opType;

        /**
         * @brief The expression on the left-hand side of the operator,
         *     for expressions that don't set a variable.
         */
        ExprNodePtr leftExpr;

        /**
         * @brief The variable on the left-hand side of the operator,
         *     for expressions that do set a variable.
         */
        VariableNodePtr leftVariable;

        /**
         * @brief The expression on the right-hand side of the operator.
         */
        ExprNodePtr rightExpr;

        /**
         * @brief Class constructor.
         * @details For binary operations that don't set a variable.
         *
         * @param leftExpr The expression on the left-hand side of the operator.
         * @param opType The type of operation being done.
         * @param rightExpr The expression on the right-hand side of the operator.
         */
        BinOpExprNode(ExprNodePtr leftExpr, BinOpType opType, ExprNodePtr rightExpr);

        /**
         * @brief Class constructor.
         * @details For binary operations that do set a variable.
         *
         * @param leftVariable The variable on the left-hand side of the operator.
         * @param opType The type of operation being done.
         * @param rightExpr The expression on the right-hand side of the operator.
         */
        BinOpExprNode(VariableNodePtr leftVariable, BinOpType opType, ExprNodePtr rightExpr);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `BINOP_EXPR` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a binary operation expression node.
     *
     */
    using BinOpExprNodePtr = std::shared_ptr<BinOpExprNode>;

    /**
     * @brief A function-call expression node.
     *
     */
    class CallExprNode : public ExprNode {
    public:
        /**
         * @brief The function to call (usually a variable, but might be a lambda expression)
         */
        ExprNodePtr func;

        /**
         * @brief The arguments to pass into the function
         */
        std::vector<ExprNodePtr> args;

        /**
         * @brief Class constructor.
         *
         * @param func The function to call (usually just a variable name).
         * @param args The arguments for the function call.
         */
        CallExprNode(ExprNodePtr func, const std::vector<ExprNodePtr>& args);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `CALL_EXPR` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a call expression node.
     *
     */
    using CallExprNodePtr = std::shared_ptr<CallExprNode>;

    /**
     * @brief A lambda expression node.
     *
     */
    class LambdaExprNode : public ExprNode {
    public:
        /**
         * The bindings representing the arguments of the function
         */
        std::vector<BindingNodePtr> bindings;

        /**
         * The body of the function
         */
        StmtNodePtr body;

        /**
         * The (optional) return type of the function
         */
        TypeNodePtr returnType;

        /**
         * @brief Class constructor with a statement body (the default).
         *
         * @param bindings The bindings (arguments) of the function.
         * @param body The body of the function.
         * @param returnType The return type of the function.
         */
        LambdaExprNode(const std::vector<BindingNodePtr>& bindings, StmtNodePtr body, TypeNodePtr returnType = nullptr);

        /**
         * @brief Class constructor with a return type for lambdas with just an expression for the body.
         *
         * @param bindings The bindings (arguments) of the function.
         * @param body The body of the function.
         * @param returnType The return type of the function.
         */
        LambdaExprNode(const std::vector<BindingNodePtr>& bindings, ExprNodePtr body, TypeNodePtr returnType = nullptr);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `LAMBDA_EXPR` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a lambda expression node.
     *
     */
    using LambdaExprNodePtr = std::shared_ptr<LambdaExprNode>;

    /**
     * @brief A command expression node.
     *
     */
    class CmdExprNode : public ExprNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param cmd The command to run.
         */
        CmdExprNode(CmdNodePtr cmd);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `CMD_EXPR` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;

        /**
         * The command for the expression
         */
        CmdNodePtr cmd;
    };
    /**
     * @brief A pointer to an command expression node.
     *
     */
    using CmdExprNodePtr = std::shared_ptr<CmdExprNode>;


    //  ===================
    //  ||  Statements:  ||
    //  ===================

    /**
     * @brief An `if` statement node.
     *
     */
    class IfStmtNode : public StmtNode {
    public:
        /**
         * The expressions representing the boolean conditions
         */
        std::vector<ExprNodePtr> conditions;

        /**
         * The statements that are evaluated when the conditions are true
         */
        std::vector<StmtNodePtr> trueStmts;

        /**
         * The statements that are evaluated when all the conditions are false
         */
        StmtNodePtr falseStmt;

        /**
         * @brief Class constructor.
         *
         * @param conditions The conditions to test.
         * @param trueStmts The statements to evaluate if true.
         * @param falseStmt The statement to evaluate if false. (Defaults to `nullptr`).
         */
        IfStmtNode(const std::vector<ExprNodePtr>& conditions, std::vector<StmtNodePtr> trueStmts,
                   StmtNodePtr falseStmt = nullptr);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this statement node.
         *
         * @return `IF_STMT` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a if statement node.
     *
     */
    using IfStmtNodePtr = std::shared_ptr<IfStmtNode>;

    /**
     * @brief A `for` loop statement node.
     *
     */
    class ForStmtNode : public StmtNode {
    public:
        /**
         * The statement for the initialization step of the loop
         */
        StmtNodePtr initial;

        /**
         * The condition expression
         */
        ExprNodePtr condition;

        /**
         * The statement that is executed at the end of each iteration
         */
        StmtNodePtr update;

        /**
         * The statement that contains the body of the loop
         */
        StmtNodePtr body;

        /**
         * @brief Class constructor.
         *
         * @param initial The initialization statement in the parentheses.
         * @param condition The condition to test in the parentheses.
         * @param update The variable-update expression in the parentheses.
         * @param body The body of the loop.
         */
        ForStmtNode(StmtNodePtr initial, ExprNodePtr condition, StmtNodePtr update, StmtNodePtr body);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this statement node.
         *
         * @return `FOR_STMT` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a for statement node.
     *
     */
    using ForStmtNodePtr = std::shared_ptr<ForStmtNode>;

    /**
     * @brief A `while` loop statement.
     *
     */
    class WhileStmtNode : public StmtNode {
    public:
        /**
         * The condition expression
         */
        ExprNodePtr condition;

        /**
         * The statement that contains the body of the loop
         */
        StmtNodePtr body;

        /**
         * @brief Class constructor.
         *
         * @param condition The condition to test.
         * @param body The body of the loop.
         */
        WhileStmtNode(ExprNodePtr condition, StmtNodePtr body);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this statement node.
         *
         * @return `WHILE_STMT` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a while statement node.
     *
     */
    using WhileStmtNodePtr = std::shared_ptr<WhileStmtNode>;

    /**
     * @brief A `do`/`while` loop statement.
     *
     */
    class DoWhileStmtNode : public StmtNode {
    public:
        /**
         * The condition expression
         */
        ExprNodePtr condition;

        /**
         * The statement that contains the body of the loop
         */
        StmtNodePtr body;

        /**
         * @brief Class constructor.
         *
         * @param condition The condition to test.
         * @param body The body of the loop.
         */
        DoWhileStmtNode(ExprNodePtr condition, StmtNodePtr body);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this statement node.
         *
         * @return `DO_WHILE_STMT` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a do while statement node.
     *
     */
    using DoWhileStmtNodePtr = std::shared_ptr<DoWhileStmtNode>;

    /**
     * @brief A return statement.
     */
    class ReturnStmtNode : public StmtNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param retExpr The expression that's evaluated and returned.
         */
        ReturnStmtNode(ExprNodePtr retExpr);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief The expression that's evaluated and returned.
         */
        ExprNodePtr retExpr;

        /**
         * @brief Reports the type of this statement node.
         *
         * @return `RETURN_STMT` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a return statement node.
     *
     */
    using ReturnStmtNodePtr = std::shared_ptr<ReturnStmtNode>;

    /**
     * @brief A new scope statement.
     *
     */
    class ScopeStmtNode : public StmtNode {
    public:
        /**
         * The statements contained within the scope
         */
        std::vector<StmtNodePtr> stmts;

        /**
         * @brief Class constructor.
         *
         * @param stmts The body of the scope.
         */
        ScopeStmtNode(const std::vector<StmtNodePtr>& stmts);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this statement node.
         *
         * @return `SCOPE_STMT` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a scope statement node.
     *
     */
    using ScopeStmtNodePtr = std::shared_ptr<ScopeStmtNode>;

    /**
     * @brief A command statement node.
     *
     */
    class CmdStmtNode : public StmtNode {
    public:
        /**
         * The command
         */
        CmdNodePtr command;

        /**
         * @brief Class constructor.
         *
         * @param command The command to run.
         */
        CmdStmtNode(CmdNodePtr command);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this statement node.
         *
         * @return `CMD_STMT` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a command statement node.
     *
     */
    using CmdStmtNodePtr = std::shared_ptr<CmdStmtNode>;

    /**
     * @brief An expression statement node.
     */
    class ExprStmtNode : public StmtNode {
    public:
        /**
         * @brief The expression that this statement node will evaluate.
         */
        ExprNodePtr expression;

        /**
         * @brief Class constructor.
         *
         * @param expression The expression that this statement node will evaluate.
         */
        ExprStmtNode(ExprNodePtr expression);

        /**
         * @brief Gets the string representation of the node as an s-expression.
         *
         * @return The s-expression string of the node.
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this statement node.
         *
         * @return `EXPR_STMT` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a expr statement node.
     *
     */
    using ExprStmtNodePtr = std::shared_ptr<ExprStmtNode>;

    /**
     * @brief An `alias` statement node.
     *
     */
    class AliasStmtNode : public StmtNode {
    public:
        /**
         * The alias name
         */
        std::string alias;

        /**
         * The command being aliased
         */
        CmdNodePtr command;

        /**
         * @brief Class constructor.
         *
         * @param variable The new alias name.
         * @param command The alias's command.
         */
        AliasStmtNode(std::string alias, CmdNodePtr command);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this statement node.
         *
         * @return `ALIAS_STMT` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to an alias statement node.
     *
     */
    using AliasStmtNodePtr = std::shared_ptr<AliasStmtNode>;

    //  ==============
    //  ||  Types:  ||
    //  ==============

    /**
     * @brief An `int` type node.
     *
     */
    class IntTypeNode : public TypeNode {
    public:
        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this type node.
         *
         * @return `INT_TYPE` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to an int type node.
     *
     */
    using IntTypeNodePtr = std::shared_ptr<IntTypeNode>;

    /**
     * @brief A `float` type node.
     *
     */
    class FloatTypeNode : public TypeNode {
    public:
        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this type node.
         *
         * @return `FLOAT_TYPE` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a float type node.
     *
     */
    using FloatTypeNodePtr = std::shared_ptr<FloatTypeNode>;

    /**
     * @brief A `bool` type node.
     *
     */
    class BoolTypeNode : public TypeNode {
    public:
        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this type node.
         *
         * @return `BOOL_TYPE` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a bool type node.
     *
     */
    using BoolTypeNodePtr = std::shared_ptr<BoolTypeNode>;

    /**
     * @brief A `string` type node.
     *
     */
    class StringTypeNode : public TypeNode {
    public:
        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this type node.
         *
         * @return `STRING_TYPE` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a string type node.
     *
     */
    using StringTypeNodePtr = std::shared_ptr<StringTypeNode>;

    /**
     * @brief An array-type node.
     *
     */
    class ArrayTypeNode : public TypeNode {
    public:
        /**
         * @brief The type of the sub-items of this array type.
         */
        TypeNodePtr subtype;

        /**
         * @brief Class constructor.
         *
         * @param subtype The type of the array's items.
         */
        ArrayTypeNode(TypeNodePtr subtype);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this type node.
         *
         * @return `ARRAY_TYPE` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to an array type node.
     *
     */
    using ArrayTypeNodePtr = std::shared_ptr<ArrayTypeNode>;

    /**
     * @brief A tuple-type node.
     *
     */
    class TupleTypeNode : public TypeNode {
    public:
        /**
         * @brief subtypes The type of each of the tuple's items.
         */
        std::vector<TypeNodePtr> subtypes;

        /**
         * @brief Class constructor.
         *
         * @param subtypes The type of each of the tuple's items.
         */
        TupleTypeNode(const std::vector<TypeNodePtr>& subtypes);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this type node.
         *
         * @return `TUPLE_TYPE` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a tuple type node.
     *
     */
    using TupleTypeNodePtr = std::shared_ptr<TupleTypeNode>;

    /**
     * @brief A function-type node.
     *
     */
    class FunctionTypeNode : public TypeNode {
    public:
        /**
         * @brief params The type of each of the function's params
         */
        std::vector<TypeNodePtr> params;

        /**
         * @brief Class constructor.
         *
         * @param params The type of each of the function's params
         */
        FunctionTypeNode(const std::vector<TypeNodePtr>& params);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this type node.
         *
         * @return `FUNCTION_TYPE` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a function type node.
     *
     */
    using FunctionTypeNodePtr = std::shared_ptr<FunctionTypeNode>;


    //  ==================
    //  ||  Variables:  ||
    //  ==================

    /**
     * @brief A variable made of just an identifier.
     */
    class IdentVariableNode : public VariableNode {
    public:
        /**
         * @brief The name of the variable.
         *
         */
        std::string varName;

        /**
         * @brief Class constructor.
         *
         * @param varName The name of the variable.
         */
        IdentVariableNode(std::string varName);

        /**
         * @brief Reports the type of this program node.
         *
         * @return `VARIABLE` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;
    };
    /**
     * @brief A pointer to an identity variable node.
     *
     */
    using IdentVariableNodePtr = std::shared_ptr<IdentVariableNode>;

}  //  namespace Parser

#endif  //  AST_NODE_HPP
