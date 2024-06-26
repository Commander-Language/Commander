/**
 * @file source/parser/ast_node.hpp
 * @brief Defines the `ASTNode` class and subclasses.
 *
 */

#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include "source/lexer/file_position.hpp"
#include "source/type_checker/type.hpp"
#include "source/type_checker/variable_table.hpp"
#include <cstdint>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
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
        BASIC_CMD,
        PIPE_CMD,
        ASYNC_CMD,
        LVALUE,
        VAR_LVALUE,
        INDEX_LVALUE,
        INT_EXPR,
        FLOAT_EXPR,
        STRING_EXPR,
        BOOL_EXPR,
        VAR_EXPR,
        LVALUE_EXPR,
        ARRAY_EXPR,
        INDEX_EXPR,
        TUPLE_EXPR,
        TERNARY_EXPR,
        UNOP_EXPR,
        BINOP_EXPR,
        CALL_EXPR,
        API_CALL_EXPR,
        LAMBDA_EXPR,
        CMD_EXPR,
        SCAN_EXPR,
        READ_EXPR,
        EXPR,
        EXPRS,
        PRGM,
        IF_STMT,
        FOR_STMT,
        WHILE_STMT,
        DO_WHILE_STMT,
        RETURN_STMT,
        BREAK_STMT,
        CONTINUE_STMT,
        SCOPE_STMT,
        CMD_STMT,
        EXPR_STMT,
        ALIAS_STMT,
        IMPORT_STMT,
        PRINT_STMT,
        PRINTLN_STMT,
        WRITE_STMT,
        TYPE_STMT,
        FUNCTION_STMT,
        TIMEOUT_STMT,
        ASSERT_STMT,
        STMT,
        STMTS,
        STRING,
        STRING_EXPRS,
        INT_TYPE,
        FLOAT_TYPE,
        BOOL_TYPE,
        STRING_TYPE,
        ARRAY_TYPE,
        TUPLE_TYPE,
        FUNCTION_TYPE,
        VARIABLE_TYPE,
        TYPE,
        TYPES
    };

    /**
     * @brief Helper method that gets the string representation of the given AST node type.
     *
     * @param nodeType The named AST node type.
     * @return The string name of the given AST node type.
     */
    std::string nodeTypeToString(ASTNodeType nodeType);

    /**
     * @brief The types of unary operation expressions
     */
    enum UnOpType : std::uint8_t { NEGATE, NOT, PRE_INCREMENT, POST_INCREMENT, PRE_DECREMENT, POST_DECREMENT };

    /**
     * @brief Helper method that gets the string representation of the Unop enum type
     * @param unop The unop to get the string version of
     * @return The string of the unop
     */
    std::string unOpToString(UnOpType unop);

    /**
     * @brief The types of binop expressions
     */
    enum BinOpType : std::uint8_t {
        LESSER,
        GREATER,
        LESSER_EQUAL,
        GREATER_EQUAL,
        EQUAL,
        NOT_EQUAL,
        AND,
        OR,
        EXPONENTIATE,
        MULTIPLY,
        DIVIDE,
        MODULO,
        ADD,
        SUBTRACT,
        EXPONENTIATE_SET,
        MULTIPLY_SET,
        DIVIDE_SET,
        MODULO_SET,
        ADD_SET,
        SUBTRACT_SET,
        SET
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
         * File position of the AST node
         */
        Lexer::FilePosition position;

        /**
         * Constructor that sets position
         *
         * @param position The position of the node
         */
        ASTNode(Lexer::FilePosition position);

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
         * Constructor that sets position
         *
         * @param position The position of the node
         */
        TypeNode(Lexer::FilePosition position);

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
     * @brief A list of type nodes.
     * @details Zero or more.
     *
     */
    class TypesNode : public ASTNode {
    public:
        /**
         * @brief Default constructor
         *
         * @param position The position of the node
         */
        TypesNode(Lexer::FilePosition position);

        /**
         * @brief Class constructor from a single expression.
         *
         * @param type The single type node that makes up this AST node.
         */
        TypesNode(const TypeNodePtr& type);

        /**
         * @brief Class constructor from a list of types, plus one more type.
         *
         * @param types The list of type nodes that makes up this AST node.
         * @param type The type to add to the end of the list of expressions.
         */
        TypesNode(const std::shared_ptr<TypesNode>& types, TypeNodePtr type);

        /**
         * @brief Reports the type of this expression-list node.
         *
         * @return `TYPES` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;

        /**
         * @brief Gets the string representation of the node as an s-expression.
         *
         * @return The s-expression string of the node.
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief The list of type nodes that makes up this AST node.
         *
         */
        std::vector<TypeNodePtr> types;
    };
    /**
     * @brief A pointer to an type-list node.
     *
     */
    using TypesNodePtr = std::shared_ptr<TypesNode>;

    /**
     * @brief A binding AST node.
     *
     */
    class BindingNode : public ASTNode {
    public:
        /**
         * The variable name of the associated binding
         */
        std::string variable;

        /**
         * Tells whether the variable is constant or not (default is false)
         */
        bool constant;

        /**
         * The (optional) type of the associated binding
         */
        TypeNodePtr type;

        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         * @param variable The name of the bound variable.
         * @param constant Tells whether the variable is constant or not (default is false)
         * @param type The type of the bound variable. (Defaults to `nullptr`.)
         */
        BindingNode(Lexer::FilePosition position, std::string variable, bool constant = false,
                    TypeNodePtr type = nullptr);

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
         * @brief Default class constructor
         *
         * @param position The position of the node
         */
        BindingsNode(Lexer::FilePosition position);

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
        BindingsNode(const std::shared_ptr<BindingsNode>& bindings, BindingNodePtr binding);

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
    class CmdNode : public ASTNode {
    public:
        /**
         * Constructor that sets position
         *
         * @param position The position of the node
         */
        CmdNode(Lexer::FilePosition position);
    };

    /**
     * @brief A smart pointer to a command node.
     *
     */
    using CmdNodePtr = std::shared_ptr<CmdNode>;

    /**
     * @brief An Lvalue node.
     *
     */
    class LValueNode : public ASTNode {
    public:
        /**
         * @brief The type of the node
         */
        TypeChecker::TyPtr type;

        /**
         * Constructor that sets position
         *
         * @param position The position of the node
         */
        LValueNode(Lexer::FilePosition position);

        /**
         * @brief Gets the string representation of the type
         *
         * @return The string representation of the type
         */
        [[nodiscard]] std::string getTypeString() const;
    };

    /**
     * @brief A smart pointer to a command node.
     *
     */
    using LValueNodePtr = std::shared_ptr<LValueNode>;


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
         * Constructor that sets position
         *
         * @param position The position of the node
         */
        ExprNode(Lexer::FilePosition position);

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
         * @brief Default constructor
         *
         * @param position The position of the node
         */
        ExprsNode(Lexer::FilePosition position);

        /**
         * @brief Class constructor from a single expression.
         *
         * @param expr The single expression node that makes up this AST node.
         */
        ExprsNode(const ExprNodePtr& expr);

        /**
         * @brief Class constructor from a list of expressions, plus one more expression.
         *
         * @param exprs The list of expression nodes that makes up this AST node.
         * @param expr The expression to add to the end of the list of expressions.
         */
        ExprsNode(const std::shared_ptr<ExprsNode>& exprs, ExprNodePtr expr);

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
    class StmtNode : public ASTNode {
    public:
        /**
         * Constructor that sets position
         *
         * @param position The position of the node
         */
        StmtNode(Lexer::FilePosition position);
    };

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
         * @brief Default constructor.
         *
         * @param position The position of the node
         */
        StmtsNode(Lexer::FilePosition position);

        /**
         * @brief Class constructor from a single statement.
         *
         * @param stmt The single statement node that makes up this AST node.
         */
        StmtsNode(const StmtNodePtr& stmt);

        /**
         * @brief Class constructor from a list of statement, plus one more statement.
         *
         * @param stmts The list of statement nodes that makes up this AST node.
         * @param stmt The statement to add to the end of the list of statements.
         */
        StmtsNode(const std::shared_ptr<StmtsNode>& stmts, StmtNodePtr stmt);

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
     * @brief A string expressions AST node.
     * @details It contains e list of string expressions.
     *
     */
    class StringExprsNode : public ASTNode {
    public:
        /**
         * @brief Class constructor from a string literal.
         *
         * @param position The position of the node
         * @param literal The string literal.
         */
        StringExprsNode(const Lexer::FilePosition& position, std::string literal);

        /**
         * @brief Class constructor from a literal, expression, and another existing list
         *
         * @param position The position of the node
         * @param literal The string literal.
         * @param expr The additional expression
         * @param exprs The list of string expressions
         */
        StringExprsNode(Lexer::FilePosition position, std::string literal, ExprNodePtr expr,
                        const std::shared_ptr<StringExprsNode>& exprs);

        /**
         * @brief Reports the type of this string node.
         *
         * @return `STRING_EXPRS` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief The list of expressions in the string.
         *
         */
        std::vector<ExprNodePtr> expressions;
    };
    /**
     * @brief A pointer to a StringExprs node.
     *
     */
    using StringExprsNodePtr = std::shared_ptr<StringExprsNode>;

    /**
     * @brief A string AST node.
     * @details It contains either a literal string or a list of string expressions (if doing string interpolation).
     *
     */
    class StringNode : public ASTNode {
    public:
        /**
         * @brief Class constructor from a string literal.
         *
         * @param position The position of the node
         * @param literal The string literal.
         */
        StringNode(Lexer::FilePosition position, std::string literal);

        /**
         * @brief Class constructor from a list of string expressions
         *
         * @param position The position of the node
         * @param exprs The list of string expressions
         */
        StringNode(Lexer::FilePosition position, StringExprsNodePtr exprs);

        /**
         * @brief The string literal if it is a literal string
         *
         */
        std::string literal;

        /**
         * @brief The list of expressions in the string (from string interpolation).
         *
         */
        StringExprsNodePtr expressions;

        /**
         * @brief Tells whether the string is a literal
         */
        [[nodiscard]] bool isLiteral() const;

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
        StmtsNodePtr stmts;

        /**
         * @brief Class constructor.
         *
         * @param stmts The statements that make up the program.
         */
        PrgmNode(const StmtsNodePtr& stmts);

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


    //  =================
    //  ||  Commands:  ||
    //  =================

    /**
     * @brief A regular command AST node.
     *
     */
    class BasicCmdNode : public CmdNode {
    public:
        /**
         * @brief The list of arguments for the command, consisting of variable expression nodes and string nodes.
         */
        std::vector<ASTNodePtr> arguments;

        /**
         * @brief Class constructor from a single argument.
         *
         * @param argument The first argument of the command.
         */
        BasicCmdNode(const ASTNodePtr& argument);

        /**
         * @brief Class constructor from two commands
         *
         * @param first The first command.
         * @param second The second command.
         */
        BasicCmdNode(const std::shared_ptr<BasicCmdNode>& first, const std::shared_ptr<BasicCmdNode>& second);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `BASIC_CMD` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a command node.
     *
     */
    using BasicCmdNodePtr = std::shared_ptr<BasicCmdNode>;

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
        AsyncCmdNode(const CmdNodePtr& cmd);

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
        PipeCmdNode(const CmdNodePtr& leftCmd, CmdNodePtr rightCmd);

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


    //  ================
    //  ||  Lvalues:  ||
    //  ================

    /**
     * @brief An AST node representing variable that can be set
     */
    class VarLValueNode : public LValueNode {
    public:
        /**
         * The variable of the node
         */
        std::string variable;

        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         * @param variable The variable of this AST node.
         */
        VarLValueNode(Lexer::FilePosition position, std::string variable);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `VAR_LVALUE` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to the VarLValue node
     *
     */
    using VarLValueNodePtr = std::shared_ptr<VarLValueNode>;

    /**
     * @brief An AST node representing an index value that can be set
     */
    class IndexLValueNode : public LValueNode {
    public:
        /**
         * The expression being indexed
         */
        LValueNodePtr lvalue;

        /**
         * The expression that make up the index
         */
        ExprNodePtr index;

        /**
         * @brief Class constructor.
         *
         * @param lvalue The lvalue which we are indexing
         * @param index The index inside the square brackets.
         */
        IndexLValueNode(const LValueNodePtr& lvalue, ExprNodePtr index);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `INDEX_LVALUE` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to the IndexLValue node
     *
     */
    using IndexLValueNodePtr = std::shared_ptr<IndexLValueNode>;


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
         * @param position The position of the node
         * @param value The value of this AST node.
         */
        IntExprNode(Lexer::FilePosition position, int64_t value);

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
         * @param position The position of the node
         * @param value The value of this AST node.
         */
        FloatExprNode(Lexer::FilePosition position, double value);

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
        StringExprNode(const StringNodePtr& stringNode);

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
         * @param position The position of the node
         * @param value The value of this AST node.
         */
        BoolExprNode(Lexer::FilePosition position, bool value);

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
        std::string variable;

        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         * @param variable The variable to reference.
         */
        VarExprNode(Lexer::FilePosition position, std::string variable);

        /**
         * @brief Class constructor.
         *
         * @param lvalue The variable lvalue to reference.
         */
        VarExprNode(const VarLValueNodePtr& lvalue);

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
     * @brief An LValue reference expression AST node.
     *
     */
    class LValueExprNode : public ExprNode {
    public:
        /**
         * The referenced expression.
         */
        ExprNodePtr expr;

        /**
         * @brief Class constructor.
         *
         * @param lvalue The lvalue to reference.
         */
        LValueExprNode(const LValueNodePtr& lvalue);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `LVALUE_EXPR` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to an variable expression node.
     *
     */
    using LValueExprNodePtr = std::shared_ptr<LValueExprNode>;

    /**
     * @brief An array literal expression node.
     *
     */
    class ArrayExprNode : public ExprNode {
    public:
        /**
         * The expressions that make up the array's values
         */
        ExprsNodePtr expressions;

        /**
         * @brief Default constructor.
         *
         * @param position The position of the node
         */
        ArrayExprNode(const Lexer::FilePosition& position);

        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         * @param expressions The elements that go inside the array.
         */
        ArrayExprNode(Lexer::FilePosition position, ExprsNodePtr expressions);

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
     * @brief An index expression node.
     *
     */
    class IndexExprNode : public ExprNode {
    public:
        /**
         * The expression being indexed
         */
        ExprNodePtr expr;

        /**
         * The expression that make up the array index
         */
        ExprNodePtr index;

        /**
         * @brief Class constructor.
         *
         * @param indexLValue The lvalue to make into an expression
         */
        IndexExprNode(const IndexLValueNodePtr& indexLValue);

        /**
         * @brief Class constructor.
         *
         * @param expr The expression into which we're indexing.
         * @param index The index inside the square brackets.
         */
        IndexExprNode(const ExprNodePtr& expr, ExprNodePtr index);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `INDEX_EXPR` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to an index expression node.
     *
     */
    using IndexExprNodePtr = std::shared_ptr<IndexExprNode>;

    /**
     * @brief A tuple literal expression node.
     *
     */
    class TupleExprNode : public ExprNode {
    public:
        /**
         * The expressions that make up the tuple's values
         */
        ExprsNodePtr expressions;

        /**
         * @brief Default constructor.
         *
         * @param position The position of the node
         */
        TupleExprNode(const Lexer::FilePosition& position);

        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         * @param expressions The elements that go inside the tuple.
         */
        TupleExprNode(Lexer::FilePosition position, ExprsNodePtr expressions);

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
         * @param position The position of the node
         * @param condition The condition to evaluate.
         * @param trueExpr The expression to evaluate if the condition was true.
         * @param falseExpr The expression to evaluate if the condition was false.
         */
        TernaryExprNode(Lexer::FilePosition position, ExprNodePtr condition, ExprNodePtr trueExpr,
                        ExprNodePtr falseExpr);

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
         * @brief The node being operated on, either an lvalue or expr.
         */
        ASTNodePtr node;

        /**
         * @brief Class constructor for a unary operation on an expression.
         *
         * @param position The position of the node
         * @param opType The unary operation type being performed.
         * @param node The node on which the unary operation is being performed.
         */
        UnOpExprNode(Lexer::FilePosition position, UnOpType opType, ASTNodePtr node);

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
         * @brief The node on the left (could be expr, lvalue, or binding)
         */
        ASTNodePtr left;

        /**
         * @brief The expression on the right-hand side of the operator.
         */
        ExprNodePtr right;

        /**
         * @brief Class constructor.
         * @details For binary operations that don't set a variable.
         *
         * @param left The node on the left-hand side of the operator.
         * @param opType The type of operation being done.
         * @param right The expression on the right-hand side of the operator.
         */
        BinOpExprNode(const ASTNodePtr& left, BinOpType opType, ExprNodePtr right);

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
        ExprsNodePtr args;

        /**
         * @brief Class constructor for function with no arguments
         *
         * @param func The function to call (usually just a variable name).
         */
        CallExprNode(const ExprNodePtr& func);

        /**
         * @brief Class constructor.
         *
         * @param func The function to call (usually just a variable name).
         * @param args The arguments for the function call.
         */
        CallExprNode(const ExprNodePtr& func, ExprsNodePtr args);

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
     * @brief An api-call expression node.
     *
     */
    class ApiCallExprNode : public ExprNode {
    public:
        /**
         * @brief The expression being called on
         */
        ExprNodePtr expression;

        /**
         * @brief The function to call
         */
        std::string func;

        /**
         * @brief The function position
         */
        Lexer::FilePosition funcPosition;

        /**
         * @brief The arguments to pass into the function
         */
        ExprsNodePtr args;

        /**
         * @brief Class constructor for api function with no arguments
         *
         * @param expression The expression being called on
         * @param func The function to call
         * @param funcPosition Position of the func variable
         */
        ApiCallExprNode(const ExprNodePtr& expression, std::string func, Lexer::FilePosition funcPosition);

        /**
         * @brief Class constructor.
         *
         * @param expression The expression being called on
         * @param func The function to call
         * @param funcPosition Position of the func variable
         * @param args The arguments for the function call.
         */
        ApiCallExprNode(const ExprNodePtr& expression, std::string func, Lexer::FilePosition funcPosition,
                        ExprsNodePtr args);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `API_CALL_EXPR` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to an api call expression node.
     *
     */
    using ApiCallExprNodePtr = std::shared_ptr<ApiCallExprNode>;

    /**
     * @brief A lambda expression node.
     *
     */
    class LambdaExprNode : public ExprNode {
    public:
        /**
         * The bindings representing the arguments of the function
         */
        BindingsNodePtr bindings;

        /**
         * The body of the function
         */
        StmtNodePtr body;

        /**
         * The (optional) return type of the function
         */
        TypeNodePtr returnType;

        /**
         * The symbol table of the function
         */
        TypeChecker::VariableTablePtr table;

        /**
         * @brief Class constructor with a statement body (the default) and no bindings
         *
         * @param position The position of the node
         * @param body The body of the function.
         * @param returnType The return type of the function.
         */
        LambdaExprNode(const Lexer::FilePosition& position, StmtNodePtr body, TypeNodePtr returnType = nullptr);

        /**
         * @brief Class constructor with a return type for lambdas with just an expression for the body and no bindings.
         *
         * @param position The position of the node
         * @param body The body of the function.
         * @param returnType The return type of the function.
         */
        LambdaExprNode(const Lexer::FilePosition& position, const ExprNodePtr& body, TypeNodePtr returnType = nullptr);

        /**
         * @brief Class constructor with a statement body (the default).
         *
         * @param position The position of the node
         * @param bindings The bindings (arguments) of the function.
         * @param body The body of the function.
         * @param returnType The return type of the function.
         */
        LambdaExprNode(Lexer::FilePosition position, BindingsNodePtr bindings, StmtNodePtr body,
                       TypeNodePtr returnType = nullptr);

        /**
         * @brief Class constructor with a return type for lambdas with just an expression for the body.
         *
         * @param position The position of the node
         * @param bindings The bindings (arguments) of the function.
         * @param body The body of the function.
         * @param returnType The return type of the function.
         */
        LambdaExprNode(Lexer::FilePosition position, BindingsNodePtr bindings, const ExprNodePtr& body,
                       TypeNodePtr returnType = nullptr);

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
         * @param position The position of the node
         * @param cmd The command to run.
         */
        CmdExprNode(Lexer::FilePosition position, CmdNodePtr cmd);

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

    /**
     * @brief An `scan` expression node.
     *
     */
    class ScanExprNode : public ExprNode {
    public:
        /**
         * The string expression that evaluates to a prompt for the user
         */
        ExprNodePtr prompt;

        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         * @param expression The string expression that evaluates to a prompt for the user
         */
        ScanExprNode(Lexer::FilePosition position, ExprNodePtr prompt);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this expression node.
         *
         * @return `SCAN_EXPR` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to an scan expression node.
     *
     */
    using ScanExprNodePtr = std::shared_ptr<ScanExprNode>;

    /**
     * @brief An `read` expression node.
     *
     */
    class ReadExprNode : public ExprNode {
    public:
        /**
         * The expression that evaluates to a string file path
         */
        ExprNodePtr filePath;

        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         * @param filePath The string expression that evaluates to a file path
         */
        ReadExprNode(Lexer::FilePosition position, ExprNodePtr filePath);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this expression node.
         *
         * @return `READ_EXPR` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to an read expression node.
     *
     */
    using ReadExprNodePtr = std::shared_ptr<ReadExprNode>;


    //  ===================
    //  ||  Statements:  ||
    //  ===================

    /**
     * @brief An `if` statement node
     *
     */
    class IfStmtNode : public StmtNode {
    public:
        /**
         * The boolean condition expression
         */
        ExprNodePtr condition;

        /**
         * The statements that are evaluated when all the condition is true
         */
        StmtNodePtr trueStmt;

        /**
         * The statements that are evaluated when all the condition is false (default is nullptr)
         */
        StmtNodePtr falseStmt;

        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         * @param condition The condition to test.
         * @param trueStmt The statements to evaluate if true.
         * @param falseStmt The statement to evaluate if false.
         */
        IfStmtNode(Lexer::FilePosition position, ExprNodePtr condition, StmtNodePtr trueStmt,
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
        ExprNodePtr initial;

        /**
         * The condition expression
         */
        ExprNodePtr condition;

        /**
         * The statement that is executed at the end of each iteration
         */
        ExprNodePtr update;

        /**
         * The statement that contains the body of the loop
         */
        StmtNodePtr body;

        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         * @param initial The initialization statement in the parentheses.
         * @param condition The condition to test in the parentheses.
         * @param update The variable-update expression in the parentheses.
         * @param body The body of the loop.
         */
        ForStmtNode(Lexer::FilePosition position, ExprNodePtr initial, ExprNodePtr condition, ExprNodePtr update,
                    StmtNodePtr body);

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
         * @param position The position of the node
         * @param condition The condition to test.
         * @param body The body of the loop.
         */
        WhileStmtNode(Lexer::FilePosition position, ExprNodePtr condition, StmtNodePtr body);

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
         * @param position The position of the node
         * @param condition The condition to test.
         * @param body The body of the loop.
         */
        DoWhileStmtNode(Lexer::FilePosition position, ExprNodePtr condition, StmtNodePtr body);

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
         * @param position The position of the node
         * @param retExpr The expression that's evaluated and returned.
         */
        ReturnStmtNode(Lexer::FilePosition position, ExprNodePtr retExpr);

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
     * @brief A break statement.
     */
    class BreakStmtNode : public StmtNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         */
        BreakStmtNode(Lexer::FilePosition position);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this statement node.
         *
         * @return `BREAK_STMT` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a break statement node.
     *
     */
    using BreakStmtNodePtr = std::shared_ptr<BreakStmtNode>;

    /**
     * @brief A continue statement.
     */
    class ContinueStmtNode : public StmtNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         */
        ContinueStmtNode(Lexer::FilePosition position);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this statement node.
         *
         * @return `CONTINUE_STMT` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a continue statement node.
     *
     */
    using ContinueStmtNodePtr = std::shared_ptr<ContinueStmtNode>;

    /**
     * @brief A new scope statement.
     *
     */
    class ScopeStmtNode : public StmtNode {
    public:
        /**
         * The statements contained within the scope
         */
        StmtsNodePtr stmts;

        /**
         * @brief Default constructor.
         *
         * @param position The position of the node
         */
        ScopeStmtNode(const Lexer::FilePosition& position);

        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         * @param stmts The body of the scope.
         */
        ScopeStmtNode(Lexer::FilePosition position, StmtsNodePtr stmts);

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
        CmdStmtNode(const CmdNodePtr& command);

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
        ExprStmtNode(const ExprNodePtr& expression);

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
        BasicCmdNodePtr command;

        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         * @param variable The new alias name.
         * @param command The alias's command.
         */
        AliasStmtNode(Lexer::FilePosition position, std::string alias, BasicCmdNodePtr command);

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

    /**
     * @brief An `import` statement node.
     *
     */
    class ImportStmtNode : public StmtNode {
    public:
        /**
         * The expression that evaluates to a string file path
         */
        StringNodePtr filePath;

        /**
         * The ASTNodeList of nodes to evaluate for the given file
         */
        ASTNodeList prgm;

        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         * @param filePath The string that evaluates to a file path
         */
        ImportStmtNode(Lexer::FilePosition position, StringNodePtr filePath);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this statement node.
         *
         * @return `IMPORT_STMT` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to an import statement node.
     *
     */
    using ImportStmtNodePtr = std::shared_ptr<ImportStmtNode>;

    /**
     * @brief An `print` statement node.
     *
     */
    class PrintStmtNode : public StmtNode {
    public:
        /**
         * The expression that gets printed out
         */
        ExprNodePtr expression;

        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         * @param expression The expression that gets printed out
         */
        PrintStmtNode(Lexer::FilePosition position, ExprNodePtr expression);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this statement node.
         *
         * @return `PRINT_STMT` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to an print statement node.
     *
     */
    using PrintStmtNodePtr = std::shared_ptr<PrintStmtNode>;

    /**
     * @brief An `println` statement node.
     *
     */
    class PrintlnStmtNode : public StmtNode {
    public:
        /**
         * The expression that gets printed out
         */
        ExprNodePtr expression;

        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         * @param expression The expression that gets printed out
         */
        PrintlnStmtNode(Lexer::FilePosition position, ExprNodePtr expression);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this statement node.
         *
         * @return `PRINTLN_STMT` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to an println statement node.
     *
     */
    using PrintlnStmtNodePtr = std::shared_ptr<PrintlnStmtNode>;

    /**
     * @brief An `write` statement node.
     *
     */
    class WriteStmtNode : public StmtNode {
    public:
        /**
         * The expression that gets written to the file
         */
        ExprNodePtr fileData;

        /**
         * The expression that evaluates to a string file path
         */
        ExprNodePtr filePath;

        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         * @param fileData The expression that gets written to the file
         * @param filePath The expression that evaluates to a string file path
         */
        WriteStmtNode(Lexer::FilePosition position, ExprNodePtr fileData, ExprNodePtr filePath);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this statement node.
         *
         * @return `WRITE_STMT` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to an write statement node.
     *
     */
    using WriteStmtNodePtr = std::shared_ptr<WriteStmtNode>;

    /**
     * @brief A `type` statement node.
     *
     */
    class TypeStmtNode : public StmtNode {
    public:
        /**
         * The type alias name
         */
        std::string alias;

        /**
         * The type
         */
        TypeNodePtr type;

        /**
         * Class constructor.
         *
         * @param position The position of the node
         * @param alias The type alias name
         * @param type The type
         */
        TypeStmtNode(Lexer::FilePosition position, std::string alias, TypeNodePtr type);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this statement node.
         *
         * @return `TYPE_STMT` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a type statement node.
     *
     */
    using TypeStmtNodePtr = std::shared_ptr<TypeStmtNode>;

    /**
     * @brief A function statement node.
     *
     */
    class FunctionStmtNode : public StmtNode {
    public:
        /**
         * The function name
         */
        std::string name;

        /**
         * The bindings representing the arguments of the function
         */
        BindingsNodePtr bindings;

        /**
         * The body of the function
         */
        StmtNodePtr body;

        /**
         * The (optional) return type of the function
         */
        TypeNodePtr returnType;

        /**
         * The symbol table of the function
         */
        TypeChecker::VariableTablePtr table;

        /**
         * @brief Class constructor with a statement body (the default), and no bindings
         *
         * @param position The position of the node
         * @param name The name of the function
         * @param body The body of the function.
         * @param returnType The return type of the function.
         */
        FunctionStmtNode(Lexer::FilePosition position, std::string name, StmtNodePtr body,
                         TypeNodePtr returnType = nullptr);

        /**
         * @brief Class constructor with a statement body (the default).
         *
         * @param position The position of the node
         * @param name The name of the function
         * @param bindings The bindings (arguments) of the function.
         * @param body The body of the function.
         * @param returnType The return type of the function.
         */
        FunctionStmtNode(Lexer::FilePosition position, std::string name, BindingsNodePtr bindings, StmtNodePtr body,
                         TypeNodePtr returnType = nullptr);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `FUNCTION_STMT` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a function statement node.
     *
     */
    using FunctionStmtNodePtr = std::shared_ptr<FunctionStmtNode>;

    /**
     * @brief A timeout statement AST node.
     *
     */
    class TimeoutStmtNode : public StmtNode {
    public:
        /**
         * @brief Timeout in milliseconds
         */
        int64_t timeout;

        /**
         * @brief Timeout message
         */
        StringNodePtr message;

        /**
         * @brief The statement to run
         */
        StmtNodePtr stmt;

        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         * @param timeout Timeout in milliseconds
         * @param message The timeout message
         * @param stmt The statement to run
         */
        TimeoutStmtNode(Lexer::FilePosition position, const int64_t& timeout, StringNodePtr message, StmtNodePtr stmt);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this statement node.
         *
         * @return `TIMEOUT_STMT` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a timeout statement node.
     *
     */
    using TimeoutStmtNodePtr = std::shared_ptr<TimeoutStmtNode>;

    /**
     * @brief An assert statement AST node.
     *
     */
    class AssertStmtNode : public StmtNode {
    public:
        /**
         * @brief Boolean expr to assert
         */
        ExprNodePtr expr;

        /**
         * @brief Assert message
         */
        StringNodePtr message;

        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         * @param expr The expression to assert
         * @param message The assert message
         */
        AssertStmtNode(Lexer::FilePosition position, ExprNodePtr expr, StringNodePtr message);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this statement node.
         *
         * @return `ASSERT_STMT` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to an assert statement node.
     *
     */
    using AssertStmtNodePtr = std::shared_ptr<AssertStmtNode>;

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
         * @brief Default Constructor
         *
         * @param position The position of the node
         */
        IntTypeNode(Lexer::FilePosition position);

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
         * @brief Default Constructor
         *
         * @param position The position of the node
         */
        FloatTypeNode(Lexer::FilePosition position);

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
         * @brief Default Constructor
         *
         * @param position The position of the node
         */
        BoolTypeNode(Lexer::FilePosition position);

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
         * @brief Default Constructor
         *
         * @param position The position of the node
         */
        StringTypeNode(Lexer::FilePosition position);

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
     * @brief A `variable` type node.
     *
     */
    class VariableTypeNode : public TypeNode {
    public:
        /**
         * The variable name
         */
        std::string name;

        /**
         * @brief Constructor
         * @param position The position of the node
         * @param name
         */
        VariableTypeNode(Lexer::FilePosition position, std::string name);

        /**
         * @brief Gets the string representation of the node as an s-expression
         *
         * @return The s-expression string of the node
         */
        [[nodiscard]] std::string sExpression() const override;

        /**
         * @brief Reports the type of this type node.
         *
         * @return `VARIABLE_TYPE` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A pointer to a variable type node.
     *
     */
    using VariableTypeNodePtr = std::shared_ptr<VariableTypeNode>;

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
         * @param position The position of the node
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
        TypesNodePtr subtypes;

        /**
         * @brief Default constructor.
         *
         * @param position The position of the node
         */
        TupleTypeNode(const Lexer::FilePosition& position);

        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         * @param subtypes The type of each of the tuple's items.
         */
        TupleTypeNode(Lexer::FilePosition position, TypesNodePtr subtypes);

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
        TypesNodePtr params;

        /**
         * @brief returnType The return type
         */
        TypeNodePtr returnType;

        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         * @param returnType The return type
         */
        FunctionTypeNode(Lexer::FilePosition position, TypeNodePtr returnType);

        /**
         * @brief Class constructor.
         *
         * @param position The position of the node
         * @param params The type of each of the function's params
         * @param returnType The return type
         */
        FunctionTypeNode(Lexer::FilePosition position, TypesNodePtr params, TypeNodePtr returnType);

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

}  //  namespace Parser

#endif  //  AST_NODE_HPP
