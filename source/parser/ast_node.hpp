/**
 * @file source/parser/ast_node.hpp
 * @brief Defines the `ASTNode` class and subclasses.
 *
 */

#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <sstream>

namespace Parser {

    /**
     * @brief The overarching type of an AST node.
     *
     */
    enum ASTNodeType { BINDING, BINDINGS, CMD, CMD_STRING, EXPR, EXPRS, PRGM, STMT, STMTS, STRING, TYPE };

    /**
     * @brief The types of unop expressions
     */
    enum Unop { NEGATE, NOT, PRE_INCREMENT, POST_INCREMENT, PRE_DECREMENT, POST_DECREMENT };

    /**
     * @brief Helper method that gets the string representation of the Unop enum type
     * @param unop The unop to get the string version of
     * @return The string of the unop
     */
    std::string unopToString(const Unop& unop);

    /**
     * @brief The types of binop expressions
     */
    enum Binop { LESSER, GREATER, LESSER_EQUAL, GREATER_EQUAL, MODULO, DIVIDE, MULTIPLY, SUBTRACT, ADD, EXPONENTIATE, AND, OR, EQUAL, NOT_EQUAL, ADD_EQUAL, SUBTRACT_EQUAL, MULTIPLY_EQUAL, DIVIDE_EQUAL, MODULO_EQUAL, EXPONENTIATE_EQUAL };

    /**
     * @brief Helper method that gets the string representation of the Binop enum type
     * @param binop The binop to get the string version of
     * @return The string of the binop
     */
    std::string binopToString(const Binop& binop);

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
         * @brief Reports the type of this type node.
         *
         * @return `TYPE` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    using TypeNodePtr = std::shared_ptr<TypeNode>;

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
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
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
         * @brief Class constructor.
         *
         * @param bindings The list of binding nodes that makes up this AST node.
         */
        BindingsNode(const std::vector<BindingNodePtr>& bindings);

        /**
         * @brief Reports the type of this binding-list node.
         *
         * @return `BINDINGS` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;

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
         * The list of arguments for the command, consisting of variable expression nodes and strings nodes
         */
        std::vector<ASTNode> arguments;

        /**
         * @brief Reports the type of this command node.
         *
         * @return `CMD` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;

        /**
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };
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
         * @brief Reports the type of this expression node.
         *
         * @return `EXPR` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
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
         * @param stmt The single expression node that makes up this AST node.
         */
        ExprsNode(const ExprNodePtr& stmt);

        /**
         * @brief Class constructor from a list of expressions, plus one more expression.
         *
         * @param exprs The list of expression nodes that makes up this AST node.
         * @param expr The expression to add to the end of the list of statements.
         */
        ExprsNode(const std::vector<ExprNodePtr>& exprs, const ExprNodePtr& expr);


        /**
         * @brief Reports the type of this expression-list node.
         *
         * @return `EXPRS` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;

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
         * @brief Reports the type of this statement node.
         *
         * @return `STMT` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
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
        StmtsNode(const std::vector<StmtNodePtr>& stmts, const StmtNodePtr& stmt);

        /**
         * @brief Reports the type of this expression-list node.
         *
         * @return `EXPRS` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;

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
        /*
         * The list of string literals in the string
         */
        std::vector<std::string> literals;

        /*
         * The list of expressions in the string (from string interpolation)
         */
        std::vector<ExprNodePtr> expressions;

        /**
         * @brief Reports the type of this string node.
         *
         * @return `STRING` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;

        /**
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };
    using StringNodePtr = std::shared_ptr<StringNode>;

    /**
     * @brief A program AST node.
     *
     */
    class PrgmNode : public ASTNode {
    public:
        /*
         * All the statements in the program
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
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };


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
         * @brief Class constructor.
         * @details TODO: This.
         *
         */
        CmdCmdNode();
    };

    /**
     * @brief An asynchronous command AST node.
     *
     */
    class AsyncCmdNode : public CmdNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param cmd The command to run asynchronously.
         */
        AsyncCmdNode(const CmdNodePtr& cmd);
    };

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
        PipeCmdNode(const CmdNodePtr& leftCmd, const CmdNodePtr& rightCmd);

        /**
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };


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
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

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
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

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
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

    /**
     * @brief A variable reference expression AST node.
     *
     */
    class VarExprNode : public ExprNode {
    public:

        /**
         * The variable name
         */
        std::string variable;

        /**
         * @brief Class constructor.
         *
         * @param variable The variable to reference.
         */
        VarExprNode(const std::string& variable);

        /**
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

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
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

    /**
     * @brief An array index expression node.
     *
     */
    class ArrayIndexExprNode : public ExprNode {
    public:

        /**
         * The expression being indexed
         */
        ExprNodePtr expression;

        /**
         * The expressions that make up the array indeces
         */
        std::vector<ExprNodePtr> expressions;

        /**
         * @brief Class constructor.
         *
         * @param expressions The elements that go inside the array.
         */
        ArrayIndexExprNode(const ExprNodePtr& expression, const std::vector<ExprNodePtr>& expressions);

        /**
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

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
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

    /**
     * @brief An tuple index expression node.
     *
     */
    class TupleIndexExprNode : public ExprNode {
    public:

        /**
         * The expression being indexed
         */
        ExprNodePtr expression;

        /**
         * The expression for the tuple index
         */
        ExprNodePtr indexExpression;

        /**
         * @brief Class constructor.
         *
         * @param expressions The elements that go inside the array.
         */
        TupleIndexExprNode(const ExprNodePtr& expression, const ExprNodePtr& indexExpression);

        /**
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

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
         * @param positive The expression to evaluate if the condition was true.
         * @param negative The expression to evaluate if the condition was false.
         */
        TernaryExprNode(const ExprNodePtr& condition, const ExprNodePtr& positive, const ExprNodePtr& negative);

        /**
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

    /**
     * @brief Unary operator expression node.
     *
     */
    class UnOpExprNode : public ExprNode {
    public:

        /**
         * The type of Unop
         */
        Unop op;

        /**
         * Expression being operated on
         */
        ExprNodePtr expression;

        /**
         * @brief Class constructor.
         * @details TODO: This.
         *
         */
        UnOpExprNode();

        /**
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

    /**
     * @brief Binary operator expression node.
     *
     */
    class BinOpExprNode : public ExprNode {
    public:

        /**
         * The type of Binop
         */
        Binop op;

        /**
         * Expression being operated on
         */
        ExprNodePtr expression;

        /**
         * @brief Class constructor.
         * @details TODO: This.
         *
         */
        BinOpExprNode();

        /**
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

    /**
     * @brief A function-call expression node.
     *
     */
    class CallExprNode : public ExprNode {
    public:

        /**
         * The function to call (usually a variable, but might be a lambda expression)
         */
        ExprNodePtr function;

        /**
         * The arguments to pass into the function
         */
        std::vector<ExprNodePtr> arguments;

        /**
         * @brief Class constructor.
         *
         * @param func The function to call (usually just a variable name).
         * @param args The arguments for the function call.
         */
        CallExprNode(const ExprNodePtr& func, const std::vector<ExprNodePtr>& args);

        /**
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

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
         * The statement of the function
         */
        StmtNodePtr statement;

        /**
         * The (optional) return type of the function
         */
        TypeNodePtr& type;

        /**
         * @brief Untyped class constructor.
         *
         * @param bindings The bindings (arguments) of the function.
         * @param body The body of the function.
         */
        LambdaExprNode(const std::vector<BindingNodePtr>& bindings, const StmtNodePtr& body);

        /**
         * @brief Untyped class constructor for lambdas with just an expression for the body.
         *
         * @param bindings The bindings (arguments) of the function.
         * @param body The body of the function.
         */
        LambdaExprNode(const std::vector<BindingNodePtr>& bindings, const ExprNodePtr& body);

        /**
         * @brief Class constructor with a return type.
         *
         * @param bindings The bindings (arguments) of the function.
         * @param body The body of the function.
         * @param type The return type of the function.
         */
        LambdaExprNode(const std::vector<BindingNodePtr>& bindings, const std::vector<StmtNodePtr>& body,
                       const TypeNodePtr& type);

        /**
         * @brief Class constructor with a return type for lambdas with just an expression for the body.
         *
         * @param bindings The bindings (arguments) of the function.
         * @param body The body of the function.
         * @param type The return type of the function.
         */
        LambdaExprNode(const std::vector<BindingNodePtr>& bindings, const ExprNodePtr& body,
                       const TypeNodePtr& type);

        /**
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

    /**
     * @brief A command expression node.
     *
     */
    class CmdExprNode : public ExprNode {
    public:

        /**
         * The command for the expression
         */
        CmdNodePtr cmd;

        /**
         * @brief Class constructor.
         *
         * @param cmd The command to run.
         */
        CmdExprNode(const CmdNodePtr& cmd);

        /**
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };


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
         * The statements that are evalauated when the conditions are true
         */
        std::vector<StmtNodePtr> statements;

        /**
         * The statement that is evaluated when all the conditions are false
         */
        StmtNodePtr falseStatement;

        /**
         * @brief Class constructor.
         *
         * @param conditions The conditions to test.
         * @param positive The statements to evaluate if true.
         * @param negative The statement to evaluate if false. (Defaults to `nullptr`).
         */
        IfStmtNode(const std::vector<ExprNodePtr>& conditions, const std::vector<StmtNodePtr>& positive, const StmtNodePtr negative = nullptr);

        /**
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

    /**
     * @brief A `for` loop statement node.
     *
     */
    class ForStmtNode : public StmtNode {
    public:

        /**
         * The statement for the initialization step of the loop
         */
        StmtNodePtr initialization;

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
        ForStmtNode(const StmtNodePtr& initial, const ExprNodePtr& condition, const StmtNodePtr& update,
                    const StmtNodePtr& body);

        /**
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

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
        WhileStmtNode(const ExprNodePtr& condition, const StmtNodePtr& body);

        /**
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

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
         * @param stmts The body of the loop.
         */
        DoWhileStmtNode(const ExprNodePtr& condition, const StmtNodePtr& stmts);

        /**
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

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
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

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
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

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
         * @param variable The variable name.
         * @param command The alias's command.
         */
        AliasStmtNode(const std::string& variable, const CmdNodePtr& command);

        /**
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };


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
         * @brief Class constructor.
         *
         */
        IntTypeNode();

        /**
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

    /**
     * @brief A `float` type node.
     *
     */
    class FloatTypeNode : public TypeNode {
    public:
        /**
         * @brief Class constructor.
         *
         */
        FloatTypeNode();

        /**
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

    /**
     * @brief An array-type node.
     *
     */
    class ArrayTypeNode : public TypeNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param subtype The type of the array's items.
         */
        ArrayTypeNode(const TypeNodePtr& subtype);

        /**
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

    /**
     * @brief A tuple-type node.
     *
     */
    class TupleTypeNode : public TypeNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param subtypes The type of each of the tuple's items.
         */
        TupleTypeNode(const std::vector<TypeNodePtr>& subtypes);

        /**
         * @brief Gets the string representation of the binding node as an s-expression
         *
         * @return The s-expression string of the binding node
         */
        [[nodiscard]] std::string sExpression() const override;
    };

}  //  namespace Parser

#endif  //  AST_NODE_HPP
