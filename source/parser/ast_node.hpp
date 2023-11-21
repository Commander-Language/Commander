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
    enum ASTNodeType { BINDING, CMD, EXPR, STMT, TYPE };

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
     * @brief An expression AST node.
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
         * @details TODO: This.
         *
         */
        StringExprNode();
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
         * @brief Class constructor.
         *
         * @param bindings The bindings (arguments) of the function.
         * @param body The body of the function.
         */
        LambdaExprNode(const std::vector<BindingNodePtr>& bindings, const std::vector<StmtNodePtr>& body);
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
                    const std::vector<StmtNodePtr>& body);
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
        WhileStmtNode(const ExprNodePtr& condition, const std::vector<StmtNodePtr>& body);
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
        DoWhileStmtNode(const ASTNodePtr& condition, const std::vector<ASTNodePtr>& stmts);
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
        ScopeStmtNode(const std::vector<ASTNodePtr>& stmts);
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

}  //  namespace Parser

#endif  //  AST_NODE_HPP
