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

namespace Parser {

    /**
     * @brief The overarching type of an AST node.
     *
     */
    enum ASTNodeType { BINDING, BINDINGS, CMD, CMD_STRING, EXPR, EXPRS, PRGM, STMT, STMTS, STRING, TYPE };

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
     * @brief A binding AST node.
     *
     */
    class BindingNode : public ASTNode {
    public:
        /**
         * @brief Reports the type of this binding node.
         *
         * @return `BINDING` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
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
         * @brief Reports the type of this command node.
         *
         * @return `CMD` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    /**
     * @brief A smart pointer to a command node.
     *
     */
    using CmdNodePtr = std::shared_ptr<CmdNode>;

    /**
     * @brief A command string AST node.
     *
     */
    class CmdStringNode : public ASTNode {
    public:
        /**
         * @brief Reports the type of this command string node.
         *
         * @return `CMD_STRING` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    using CmdStringNodePtr = std::shared_ptr<CmdStringNode>;

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
     *
     */
    class StringNode : public ASTNode {
    public:
        /**
         * @brief Reports the type of this string node.
         *
         * @return `STRING` always.
         */
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    using StringNodePtr = std::shared_ptr<StringNode>;

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
     * @brief A program AST node.
     *
     */
    class PrgmNode : public ASTNode {
    public:
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
         * @brief Class constructor.
         *
         * @param leftCmd The "source" command on the left. Its output goes into the pipe.
         * @param rightCmd The "destination" command on the right. The pipe goes into its input.
         */
        PipeCmdNode(const CmdNodePtr& leftCmd, const CmdNodePtr& rightCmd);
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
         * @brief Class constructor.
         *
         * @param value The value of this AST node.
         */
        IntExprNode(int64_t value);
    };

    /**
     * @brief A floating-point literal expression AST node.
     */
    class FloatExprNode : public ExprNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param value The value of this AST node.
         */
        FloatExprNode(double value);
    };

    /**
     * @brief A string literal expression node.
     *
     */
    class StringExprNode : public ExprNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param strNode The string AST node to turn into an expression.
         */
        StringExprNode(const StringNodePtr& strNode);
    };

    /**
     * @brief A boolean literal expression AST node.
     *
     */
    class BoolExprNode : public ExprNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param value The value of this AST node.
         */
        BoolExprNode(bool value);
    };

    /**
     * @brief A variable reference expression AST node.
     *
     */
    class VarExprNode : public ExprNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param variable The variable to reference.
         */
        VarExprNode(const std::string& variable);
    };

    /**
     * @brief An array literal expression node.
     *
     */
    class ArrayExprNode : public ExprNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param expressions The elements that go inside the array.
         */
        ArrayExprNode(const std::vector<ExprNodePtr>& expressions);
    };

    /**
     * @brief A tuple literal expression node.
     *
     */
    class TupleExprNode : public ExprNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param expressions The elements that go inside the tuple.
         */
        TupleExprNode(const std::vector<ExprNodePtr>& expressions);
    };

    /**
     * @brief A ternary expression node.
     *
     */
    class TernaryExprNode : public ExprNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param condition The condition to evaluate.
         * @param positive The expression to evaluate if the condition was true.
         * @param negative The expression to evaluate if the condition was false.
         */
        TernaryExprNode(const ExprNodePtr& condition, const ExprNodePtr& positive, const ExprNodePtr& negative);
    };

    /**
     * @brief Unary operator expression node.
     *
     */
    class UnOpExprNode : public ExprNode {
    public:
        /**
         * @brief Class constructor.
         * @details TODO: This.
         *
         */
        UnOpExprNode();
    };

    /**
     * @brief Binary operator expression node.
     *
     */
    class BinOpExprNode : public ExprNode {
    public:
        /**
         * @brief Class constructor.
         * @details TODO: This.
         *
         */
        BinOpExprNode();
    };

    /**
     * @brief A function-call expression node.
     *
     */
    class CallExprNode : public ExprNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param func The function to call (usually just a variable name).
         * @param args The arguments for the function call.
         */
        CallExprNode(const ExprNodePtr& func, const std::vector<ExprNodePtr>& args);
    };

    /**
     * @brief A lambda expression node.
     *
     */
    class LambdaExprNode : public ExprNode {
    public:
        /**
         * @brief Untyped class constructor.
         *
         * @param bindings The bindings (arguments) of the function.
         * @param body The body of the function.
         */
        LambdaExprNode(const std::vector<BindingNodePtr>& bindings, const std::vector<StmtNodePtr>& body);

        /**
         * @brief Class constructor with a return type.
         *
         * @param bindings The bindings (arguments) of the function.
         * @param body The body of the function.
         * @param type The return type of the function.
         */
        LambdaExprNode(const std::vector<BindingNodePtr>& bindings, const std::vector<StmtNodePtr>& body,
                       const TypeNodePtr& type);
    };

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
        CmdExprNode(const CmdNodePtr& cmd);
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
         * @brief Class constructor.
         *
         * @param condition The condition to test.
         * @param positive The statement to evaluate if true.
         * @param negative The statement to evaluate if false. (Defaults to `nullptr`).
         */
        IfStmtNode(const ExprNodePtr& condition, const StmtNodePtr& positive, const StmtNodePtr negative = nullptr);
    };

    /**
     * @brief A `for` loop statement node.
     *
     */
    class ForStmtNode : public StmtNode {
    public:
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
    };

    /**
     * @brief A `while` loop statement.
     *
     */
    class WhileStmtNode : public StmtNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param condition The condition to test.
         * @param body The body of the loop.
         */
        WhileStmtNode(const ExprNodePtr& condition, const StmtNodePtr& body);
    };

    /**
     * @brief A `do`/`while` loop statement.
     *
     */
    class DoWhileStmtNode : public StmtNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param condition The condition to test.
         * @param stmts The body of the loop.
         */
        DoWhileStmtNode(const ExprNodePtr& condition, const StmtNodePtr& stmts);
    };

    /**
     * @brief A new scope statement.
     *
     */
    class ScopeStmtNode : public StmtNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param stmts The body of the scope.
         */
        ScopeStmtNode(const std::vector<StmtNodePtr>& stmts);
    };

    /**
     * @brief A command statement node.
     *
     */
    class CmdStmtNode : public StmtNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param command The command to run.
         */
        CmdStmtNode(const ASTNodePtr& command);
    };

    /**
     * @brief An `alias` statement node.
     *
     */
    class AliasStmtNode : public StmtNode {
    public:
        /**
         * @brief Class constructor.
         *
         * @param variable The variable name.
         * @param command The alias's command.
         */
        AliasStmtNode(const std::string& variable, const ASTNodePtr& command);
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
    };

}  //  namespace Parser

#endif  //  AST_NODE_HPP
