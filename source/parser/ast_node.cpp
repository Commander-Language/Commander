/**
 * @file source/parser/ast_node.cpp
 * @brief Implements the functions within the `ASTNode` class and subclasses.
 *
 */

#include "ast_node.hpp"

#include <utility>

namespace Parser {

    std::string unOpToString(UnOpType unop) {
        switch (unop) {
            case NEGATE:
                return "NEGATE";
            case NOT:
                return "NOT";
            case PRE_INCREMENT:
                return "PRE_INCREMENT";
            case POST_INCREMENT:
                return "POST_INCREMENT";
            case PRE_DECREMENT:
                return "PRE_DECREMENT";
            case POST_DECREMENT:
                return "POST_DECREMENT";
            default:
                return "UNKNOWN";
        }
    }

    std::string binOpToString(BinOpType binop) {
        switch (binop) {
            case LESSER:
                return "LESSER";
            case GREATER:
                return "GREATER";
            case LESSER_EQUAL:
                return "LESSER_EQUAL";
            case GREATER_EQUAL:
                return "GREATER_EQUAL";
            case MODULO:
                return "MODULO";
            case DIVIDE:
                return "DIVIDE";
            case MULTIPLY:
                return "MULTIPLY";
            case SUBTRACT:
                return "SUBTRACT";
            case ADD:
                return "ADD";
            case EXPONENTIATE:
                return "EXPONENTIATE";
            case AND:
                return "AND";
            case OR:
                return "OR";
            case EQUAL:
                return "EQUAL";
            case NOT_EQUAL:
                return "NOT_EQUAL";
            case ADD_EQUAL:
                return "ADD_EQUAL";
            case SUBTRACT_EQUAL:
                return "SUBTRACT_EQUAL";
            case MULTIPLY_EQUAL:
                return "MULTIPLY_EQUAL";
            case DIVIDE_EQUAL:
                return "DIVIDE_EQUAL";
            case MODULO_EQUAL:
                return "MODULO_EQUAL";
            case EXPONENTIATE_EQUAL:
                return "EXPONENTIATE_EQUAL";
            default:
                return "UNKNOWN";
        }
    }

    /**
     * @brief Copies the given list of AST node pointers. Then, adds the given node to the end of the list.
     * @details This function is not in the header, because it's intended to be used only in this CPP file.
     *
     * @tparam NodeType The type of AST nodes in the list.
     * @param nodes A list of AST node pointers.
     * @param node An AST node pointer to add to the end of the list.
     * @return A concatenation of `nodes` and `node`.
     */
    template<typename NodeType>
    std::vector<std::shared_ptr<NodeType>> concat(const std::vector<std::shared_ptr<NodeType>>& nodes,
                                                  std::shared_ptr<NodeType> node) {
        std::vector<std::shared_ptr<NodeType>> result = nodes;
        result.push_back(node);
        return result;
    }


    //  ==========================
    //  ||  Overarching types:  ||
    //  ==========================

    ASTNodeType TypeNode::nodeType() const { return ASTNodeType::TYPE; }

    BindingNode::BindingNode(std::string variable, Parser::TypeNodePtr type)
        : variable(std::move(variable)), type(std::move(type)) {}

    ASTNodeType BindingNode::nodeType() const { return ASTNodeType::BINDING; }

    std::string BindingNode::sExpression() const {
        return "(BindingNode " + variable + " " + type->sExpression() + ")";
    }

    ASTNodeType BindingsNode::nodeType() const { return ASTNodeType::BINDINGS; }

    std::string BindingsNode::sExpression() const {
        std::stringstream result;
        result << "(BindingsNode";
        for (const auto& binding : bindings) { result << " " << binding->sExpression(); }
        result << ")";
        return result.str();
    }

    BindingsNode::BindingsNode(BindingNodePtr binding) : bindings({std::move(binding)}) {}

    BindingsNode::BindingsNode(const std::vector<BindingNodePtr>& bindings, BindingNodePtr binding)
        : bindings(concat(bindings, std::move(binding))) {}

    ASTNodeType CmdNode::nodeType() const { return ASTNodeType::CMD; }

    ASTNodeType ExprNode::nodeType() const { return ASTNodeType::EXPR; }

    ExprsNode::ExprsNode(ExprNodePtr expr) : exprs({std::move(expr)}) {}

    ExprsNode::ExprsNode(const std::vector<ExprNodePtr>& exprs, ExprNodePtr expr)
        : exprs(concat(exprs, std::move(expr))) {}

    ASTNodeType ExprsNode::nodeType() const { return ASTNodeType::EXPRS; }

    std::string ExprsNode::sExpression() const {
        std::stringstream result;
        result << "(ExprsNode";
        for (const auto& expr : exprs) { result << " " << expr->sExpression(); }
        result << ")";
        return result.str();
    }

    ASTNodeType StmtNode::nodeType() const { return ASTNodeType::STMT; }

    StmtsNode::StmtsNode(Parser::StmtNodePtr stmt) : stmts({std::move(stmt)}) {}

    StmtsNode::StmtsNode(const std::vector<StmtNodePtr>& stmts, Parser::StmtNodePtr stmt)
        : stmts(concat(stmts, std::move(stmt))) {}

    ASTNodeType StmtsNode::nodeType() const { return ASTNodeType::STMTS; }

    std::string StmtsNode::sExpression() const {
        std::stringstream result;
        result << "(StmtsNode";
        for (const auto& stmt : stmts) { result << " " << stmt->sExpression(); }
        result << ")";
        return result.str();
    }

    ASTNodeType StringNode::nodeType() const { return ASTNodeType::STRING; }

    std::string StringNode::sExpression() const {
        std::stringstream builder;
        for (int i = 0; i < (int)literals.size(); i++) {
            builder << " " << literals[i];
            if (i < (int)expressions.size()) { builder << " " << expressions[i]->sExpression(); }
        }
        return "(StringNode" + builder.str() + ")";
    }

    PrgmNode::PrgmNode(const std::vector<StmtNodePtr>& stmts) : stmts(stmts) {}

    ASTNodeType PrgmNode::nodeType() const { return PRGM; }

    std::string PrgmNode::sExpression() const {
        std::stringstream builder;
        for (const StmtNodePtr& stmt : stmts) { builder << " " << stmt->sExpression(); }
        return "(PrgmNode" + builder.str() + ")";
    }

    ASTNodeType VariableNode::nodeType() const { return ASTNodeType::VARIABLE; }

    //  =================
    //  ||  Commands:  ||
    //  =================

    std::string CmdCmdNode::sExpression() const {
        std::stringstream builder;
        for (const ASTNode& arg : arguments) { builder << " " << arg.sExpression(); }
        return "(CmdCmdNode" + builder.str() + ")";
    }

    AsyncCmdNode::AsyncCmdNode(Parser::CmdNodePtr cmd) : cmd(std::move(cmd)) {}

    std::string AsyncCmdNode::sExpression() const { return "(AsyncCmdNode" + cmd->sExpression() + ")"; }

    PipeCmdNode::PipeCmdNode(Parser::CmdNodePtr leftCmd, Parser::CmdNodePtr rightCmd)
        : leftCmd(std::move(leftCmd)), rightCmd(std::move(rightCmd)) {}

    std::string PipeCmdNode::sExpression() const {
        return "(PipeCmdNode " + leftCmd->sExpression() + " | " + rightCmd->sExpression() + ")";
    }


    //  ====================
    //  ||  Expressions:  ||
    //  ====================

    IntExprNode::IntExprNode(int64_t value) : value(value) {}

    std::string IntExprNode::sExpression() const { return "(IntExprNode " + std::to_string(value) + ")"; }

    FloatExprNode::FloatExprNode(double value) : value(value) {}

    std::string FloatExprNode::sExpression() const { return "(FloatExprNode " + std::to_string(value) + ")"; }

    StringExprNode::StringExprNode(Parser::StringNodePtr stringNode) : stringNode(std::move(stringNode)) {}

    std::string StringExprNode::sExpression() const {
        return "(StringExpressionNode " + stringNode->sExpression() + ")";
    }

    BoolExprNode::BoolExprNode(bool value) : value(value) {}

    std::string BoolExprNode::sExpression() const {
        const std::string val = value ? "true" : "false";
        return "(BoolExprNode " + val + ")";
    }

    VarExprNode::VarExprNode(Parser::VariableNodePtr variable) : variable(std::move(variable)) {}

    std::string VarExprNode::sExpression() const { return "(VarExprNode " + variable->sExpression() + ")"; }

    ArrayExprNode::ArrayExprNode(const std::vector<ExprNodePtr>& expressions) : expressions(expressions) {}

    std::string ArrayExprNode::sExpression() const {
        std::stringstream builder;
        builder << "(ArrayExprNode";
        for (const ExprNodePtr& expr : expressions) { builder << " " << expr->sExpression(); }
        return builder.str() + ")";
    }

    ArrayIndexExprNode::ArrayIndexExprNode(Parser::ExprNodePtr array, const std::vector<ExprNodePtr>& indexExprs)
        : array(std::move(array)), indexExprs(indexExprs) {}

    std::string ArrayIndexExprNode::sExpression() const {
        std::stringstream builder;
        builder << "(ArrayIndexExprNode " << array->sExpression();
        for (const ExprNodePtr& indexExpr : indexExprs) { builder << " " << indexExpr->sExpression(); }
        builder << ")";
        return builder.str();
    }

    TupleExprNode::TupleExprNode(const std::vector<ExprNodePtr>& expressions) : expressions(expressions) {}

    std::string TupleExprNode::sExpression() const {
        std::stringstream builder;
        builder << "(TupleExprNode";
        for (const ExprNodePtr& expr : expressions) { builder << " " << expr->sExpression(); }
        return builder.str() + ")";
    }

    TupleIndexExprNode::TupleIndexExprNode(Parser::ExprNodePtr tuple, Parser::ExprNodePtr index)
        : tuple(std::move(tuple)), index(std::move(index)) {}

    std::string TupleIndexExprNode::sExpression() const {
        return "(TupleIndexExprNode " + tuple->sExpression() + " " + index->sExpression() + ")";
    }

    TernaryExprNode::TernaryExprNode(Parser::ExprNodePtr condition, Parser::ExprNodePtr trueExpr,
                                     Parser::ExprNodePtr falseExpr)
        : condition(std::move(condition)), trueExpr(std::move(trueExpr)), falseExpr(std::move(falseExpr)) {}

    std::string TernaryExprNode::sExpression() const {
        return "(TernaryExprNode " + condition->sExpression() + " ? " + trueExpr->sExpression() + " : "
             + falseExpr->sExpression() + ")";
    }

    UnOpExprNode::UnOpExprNode(Parser::UnOpType opType, Parser::ExprNodePtr expr)
        : opType(opType), expr(std::move(expr)) {}

    UnOpExprNode::UnOpExprNode(Parser::UnOpType opType, Parser::VariableNodePtr var)
        : opType(opType), var(std::move(var)) {}

    std::string UnOpExprNode::sExpression() const {
        return "(UnOpExprNode " + unOpToString(opType) + " " + expr->sExpression() + ")";
    }

    BinOpExprNode::BinOpExprNode(Parser::ExprNodePtr leftExpr, Parser::BinOpType opType, Parser::ExprNodePtr rightExpr)
        : leftExpr(std::move(leftExpr)), opType(opType), rightExpr(std::move(rightExpr)) {}

    BinOpExprNode::BinOpExprNode(Parser::VariableNodePtr leftVariable, Parser::BinOpType opType,
                                 Parser::ExprNodePtr rightExpr)
        : leftVariable(std::move(leftVariable)), opType(opType), rightExpr(std::move(rightExpr)) {}

    std::string BinOpExprNode::sExpression() const {
        return "(BinOpExprNode "
             + (leftExpr == nullptr ? (ASTNodePtr)leftVariable : (ASTNodePtr)leftExpr)->sExpression() + " "
             + binOpToString(opType) + " " + rightExpr->sExpression() + ")";
    }

    CallExprNode::CallExprNode(Parser::ExprNodePtr func, const std::vector<ExprNodePtr>& args)
        : func(std::move(func)), args(args) {}

    std::string CallExprNode::sExpression() const {
        std::stringstream builder;
        for (const ExprNodePtr& arg : args) { builder << " " << arg->sExpression(); }
        return "(CallExprNode " + func->sExpression() + builder.str() + ")";
    }

    LambdaExprNode::LambdaExprNode(const std::vector<BindingNodePtr>& bindings, Parser::ExprNodePtr body,
                                   Parser::TypeNodePtr type)
        : bindings(bindings), body(std::make_shared<ReturnStmtNode>(std::move(body))), type(std::move(type)) {}

    LambdaExprNode::LambdaExprNode(const std::vector<BindingNodePtr>& bindings, Parser::StmtNodePtr body,
                                   Parser::TypeNodePtr type)
        : bindings(bindings), body(std::move(body)), type(std::move(type)) {}

    std::string LambdaExprNode::sExpression() const {
        std::stringstream bindingsBuilder;
        bool first = true;
        for (const BindingNodePtr& binding : bindings) {
            bindingsBuilder << (first ? "" : " ") << binding->sExpression();
            first = false;
        }
        return "(LambdaExprNode (" + bindingsBuilder.str() + ") " + type->sExpression() + " " + body->sExpression()
             + ")";
    }

    CmdExprNode::CmdExprNode(Parser::CmdNodePtr cmd) : cmd(std::move(cmd)) {}

    std::string CmdExprNode::sExpression() const { return "(CmdExprNode " + cmd->sExpression() + ")"; }


    //  ===================
    //  ||  Statements:  ||
    //  ===================

    IfStmtNode::IfStmtNode(const std::vector<ExprNodePtr>& conds, std::vector<StmtNodePtr> trueStmts,
                           Parser::StmtNodePtr falseStmt)
        : conds(conds), trueStmts(std::move(trueStmts)), falseStmt(std::move(falseStmt)) {}

    std::string IfStmtNode::sExpression() const {
        std::stringstream builder;
        for (int i = 0; i < (int)conds.size(); i++) {
            builder << " " << conds[i]->sExpression();
            builder << " " << trueStmts[i]->sExpression();
        }
        return "(IfStmtNode" + builder.str() + " " + falseStmt->sExpression() + ")";
    }

    ForStmtNode::ForStmtNode(Parser::StmtNodePtr initial, Parser::ExprNodePtr condition, Parser::StmtNodePtr update,
                             Parser::StmtNodePtr body)
        : initial(std::move(initial)), condition(std::move(condition)), update(std::move(update)),
          body(std::move(body)) {}

    std::string ForStmtNode::sExpression() const {
        return "(ForStmtNode " + initial->sExpression() + " " + condition->sExpression() + " " + update->sExpression()
             + " " + body->sExpression() + ")";
    }

    WhileStmtNode::WhileStmtNode(Parser::ExprNodePtr condition, Parser::StmtNodePtr body)
        : condition(std::move(condition)), body(std::move(body)) {}

    std::string WhileStmtNode::sExpression() const {
        return "(WhileStmtNode " + condition->sExpression() + " " + body->sExpression() + ")";
    }

    DoWhileStmtNode::DoWhileStmtNode(Parser::ExprNodePtr condition, Parser::StmtNodePtr body)
        : condition(std::move(condition)), body(std::move(body)) {}

    std::string DoWhileStmtNode::sExpression() const {
        return "(DoWhileStmtNode " + condition->sExpression() + " " + body->sExpression() + ")";
    }

    ReturnStmtNode::ReturnStmtNode(Parser::ExprNodePtr retExpr) : retExpr(std::move(retExpr)) {}

    std::string ReturnStmtNode::sExpression() const { return "(ReturnStmtNode " + retExpr->sExpression() + ")"; }

    ScopeStmtNode::ScopeStmtNode(const std::vector<StmtNodePtr>& stmts) : stmts(stmts) {}

    std::string ScopeStmtNode::sExpression() const {
        std::stringstream builder;
        for (const StmtNodePtr& stmt : stmts) { builder << "\n\t" << stmt->sExpression(); }
        return "(ScopeStmtNode " + builder.str() + ")";
    }

    CmdStmtNode::CmdStmtNode(Parser::CmdNodePtr command) : command(std::move(command)) {}

    std::string CmdStmtNode::sExpression() const { return "(CmdStmtNode " + command->sExpression() + ")"; }

    ExprStmtNode::ExprStmtNode(Parser::ExprNodePtr expression) : expression(std::move(expression)) {}

    std::string ExprStmtNode::sExpression() const { return "(ExprStmtNode " + expression->sExpression() + ")"; }

    AliasStmtNode::AliasStmtNode(std::string alias, Parser::CmdNodePtr command)
        : alias(std::move(alias)), command(std::move(command)) {}

    std::string AliasStmtNode::sExpression() const {
        return "(AliasStmtNode " + alias + " " + command->sExpression() + ")";
    }

    std::string IntTypeNode::sExpression() const { return "(IntTypeNode)"; }

    std::string FloatTypeNode::sExpression() const { return "(FloatTypeNode)"; }

    ArrayTypeNode::ArrayTypeNode(Parser::TypeNodePtr subtype) : subtype(std::move(subtype)) {}

    std::string ArrayTypeNode::sExpression() const { return "(ArrayTypeNode " + subtype->sExpression() + ")"; }

    TupleTypeNode::TupleTypeNode(const std::vector<TypeNodePtr>& subtypes) : subtypes(subtypes) {}

    std::string TupleTypeNode::sExpression() const { return "(TupleTypeNode)"; }

    IdentVariableNode::IdentVariableNode(std::string varName) : varName(std::move(varName)) {}

    std::string IdentVariableNode::sExpression() const { return "(IdentVariableNode " + varName + ")"; }

}  //  namespace Parser