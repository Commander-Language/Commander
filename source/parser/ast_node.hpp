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
    enum ASTNodeType { TYPE, EXPR, STMT, CMD };

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

    class CmdNode : public ASTNode {
    public:
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    using CmdNodePtr = std::shared_ptr<CmdNode>;

    class ExprNode : public ASTNode {
    public:
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    using ExprNodePtr = std::shared_ptr<ExprNode>;

    class StmtNode : public ASTNode {
    public:
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    using StmtNodePtr = std::shared_ptr<StmtNode>;

    class BindingNode : public ASTNode {
    public:
        [[nodiscard]] ASTNodeType nodeType() const override;
    };
    using BindingNodePtr = std::shared_ptr<BindingNode>;


    //  =================
    //  ||  Commands:  ||
    //  =================

    class CmdCmdNode : public CmdNode {
    public:
        //  TODO: This.
        CmdCmdNode();
    };

    class AsyncCmdNode : public CmdNode {
    public:
        AsyncCmdNode(const CmdNodePtr& cmd);
    };

    class PipeCmdNode : public CmdNode {
    public:
        PipeCmdNode(const CmdNodePtr& leftCmd, const CmdNodePtr& rightCmd);
    };


    //  ====================
    //  ||  Expressions:  ||
    //  ====================

    class IntExprNode : public ExprNode {
    public:
        IntExprNode(int64_t value);
    };

    class FloatExprNode : public ExprNode {
    public:
        FloatExprNode(double value);
    };

    class StringExprNode : public ExprNode {
    public:
        //  TODO: This.
        StringExprNode();
    };

    class BoolExprNode : public ExprNode {
    public:
        BoolExprNode(bool value);
    };

    class VarExprNode : public ExprNode {
    public:
        VarExprNode(const std::string& variable);
    };

    class ArrayExprNode : public ExprNode {
    public:
        ArrayExprNode(const std::vector<ExprNodePtr>& expressions);
    };

    class TupleExprNode : public ExprNode {
    public:
        TupleExprNode(const std::vector<ExprNodePtr>& expressions);
    };

    class TernaryExprNode : public ExprNode {
    public:
        TernaryExprNode(const ExprNodePtr& condition, const ExprNodePtr& positive, const ExprNodePtr& negative);
    };

    class UnOpExprNode : public ExprNode {
    public:
        //  TODO: This.
        UnOpExprNode();
    };

    class BinOpExprNode : public ExprNode {
    public:
        //  TODO: This.
        BinOpExprNode();
    };

    class CallExprNode : public ExprNode {
    public:
        CallExprNode(const ExprNodePtr& func, const std::vector<ExprNodePtr>& args);
    };

    class LambdaExprNode : public ExprNode {
    public:
        LambdaExprNode(const std::vector<BindingNodePtr>& bindings, const std::vector<StmtNodePtr>& body);
    };

    class CmdExprNode : public ExprNode {
    public:
        CmdExprNode(const CmdNodePtr& cmd);
    };


    //  ===================
    //  ||  Statements:  ||
    //  ===================

    class IfStmtNode : public StmtNode {
    public:
        IfStmtNode(const ExprNodePtr& condition, const StmtNodePtr& positive, const StmtNodePtr negative = nullptr);
    };

    class ForStmtNode : public StmtNode {
    public:
        ForStmtNode(const StmtNodePtr& initial, const StmtNodePtr& update, const ExprNodePtr& condition,
                    const std::vector<StmtNodePtr>& body);
    };

    class WhileStmtNode : public StmtNode {
    public:
        WhileStmtNode(const ExprNodePtr& condition, const std::vector<StmtNodePtr>& body);
    };

    class DoWhileStmtNode : public StmtNode {
    public:
        DoWhileStmtNode(const ASTNodePtr& condition, const std::vector<ASTNodePtr>& stmts);
    };

    class ScopeStmtNode : public StmtNode {
    public:
        ScopeStmtNode(const std::vector<ASTNodePtr>& stmts);
    };

    class CmdStmtNode : public StmtNode {
    public:
        CmdStmtNode(const ASTNodePtr& command);
    };

    class AliasStmtNode : public StmtNode {
    public:
        AliasStmtNode(const std::string& variable, const ASTNodePtr& command);
    };

}  //  namespace Parser

#endif  //  AST_NODE_HPP
