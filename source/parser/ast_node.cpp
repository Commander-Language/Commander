/**
 * @file source/parser/ast_node.cpp
 * @brief Implements the functions within the `ASTNode` class and subclasses.
 *
 */

#include "ast_node.hpp"
#include "source/type_checker/type.hpp"
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
            case SET:
                return "SET";
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

    ASTNodeType IntTypeNode::nodeType() const { return ASTNodeType::INT_TYPE; }

    ASTNodeType FloatTypeNode::nodeType() const { return ASTNodeType::FLOAT_TYPE; }

    ASTNodeType BoolTypeNode::nodeType() const { return ASTNodeType::BOOL_TYPE; }

    ASTNodeType StringTypeNode::nodeType() const { return ASTNodeType::STRING_TYPE; }

    ASTNodeType ArrayTypeNode::nodeType() const { return ASTNodeType::ARRAY_TYPE; }

    ASTNodeType TupleTypeNode::nodeType() const { return ASTNodeType::TUPLE_TYPE; }

    ASTNodeType FunctionTypeNode::nodeType() const { return ASTNodeType::FUNCTION_TYPE; }

    std::string TypeNode::getTypeString() const {
        return TypeChecker::getTypeString(type);
    }

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

    std::string ExprNode::getTypeString() const {
        return TypeChecker::getTypeString(type);
    }

    ASTNodeType CmdCmdNode::nodeType() const { return ASTNodeType::CMD; }

    ASTNodeType PipeCmdNode::nodeType() const { return ASTNodeType::PIPE_CMD; }

    ASTNodeType AsyncCmdNode::nodeType() const { return ASTNodeType::ASYNC_CMD; }

    ASTNodeType IntExprNode::nodeType() const { return ASTNodeType::INT_EXPR; }

    ASTNodeType FloatExprNode::nodeType() const { return ASTNodeType::FLOAT_EXPR; }

    ASTNodeType StringExprNode::nodeType() const { return ASTNodeType::STRING_EXPR; }

    ASTNodeType BoolExprNode::nodeType() const { return ASTNodeType::BOOL_EXPR; }

    ASTNodeType VarExprNode::nodeType() const { return ASTNodeType::VAR_EXPR; }

    ASTNodeType ArrayExprNode::nodeType() const { return ASTNodeType::ARRAY_EXPR; }

    ASTNodeType ArrayIndexExprNode::nodeType() const { return ASTNodeType::ARRAY_INDEX_EXPR; }

    ASTNodeType TupleExprNode::nodeType() const { return ASTNodeType::TUPLE_EXPR; }

    ASTNodeType TupleIndexExprNode::nodeType() const { return ASTNodeType::TUPLE_INDEX_EXPR; }

    ASTNodeType TernaryExprNode::nodeType() const { return ASTNodeType::TERNARY_EXPR; }

    ASTNodeType UnOpExprNode::nodeType() const { return ASTNodeType::UNOP_EXPR; }

    ASTNodeType BinOpExprNode::nodeType() const { return ASTNodeType::BINOP_EXPR; }

    ASTNodeType CallExprNode::nodeType() const { return ASTNodeType::CALL_EXPR; }

    ASTNodeType LambdaExprNode::nodeType() const { return ASTNodeType::LAMBDA_EXPR; }

    ASTNodeType CmdExprNode::nodeType() const { return ASTNodeType::CMD_EXPR; }

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

    ASTNodeType IfStmtNode::nodeType() const { return ASTNodeType::IF_STMT; }

    ASTNodeType ForStmtNode::nodeType() const { return ASTNodeType::FOR_STMT; }

    ASTNodeType WhileStmtNode::nodeType() const { return ASTNodeType::WHILE_STMT; }

    ASTNodeType DoWhileStmtNode::nodeType() const { return ASTNodeType::DO_WHILE_STMT; }

    ASTNodeType ReturnStmtNode::nodeType() const { return ASTNodeType::RETURN_STMT; }

    ASTNodeType ScopeStmtNode::nodeType() const { return ASTNodeType::SCOPE_STMT; }

    ASTNodeType CmdStmtNode::nodeType() const { return ASTNodeType::CMD_STMT; }

    ASTNodeType ExprStmtNode::nodeType() const { return ASTNodeType::EXPR_STMT; }

    ASTNodeType AliasStmtNode::nodeType() const { return ASTNodeType::ALIAS_STMT; }

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

    ASTNodeType IdentVariableNode::nodeType() const { return ASTNodeType::VARIABLE; }

    //  =================
    //  ||  Commands:  ||
    //  =================

    std::string CmdCmdNode::sExpression() const {
        std::stringstream builder;
        for (const ASTNodePtr& arg : arguments) { builder << " " << arg->sExpression(); }
        return "(CmdCmdNode" + builder.str() + ")";
    }

    CmdCmdNode::CmdCmdNode(std::vector<ASTNodePtr> arguments) : arguments(std::move(arguments)) {}

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

    std::string IntExprNode::sExpression() const { return "(IntExprNode " + std::to_string(value) + getTypeString() + ")"; }

    FloatExprNode::FloatExprNode(double value) : value(value) {}

    std::string FloatExprNode::sExpression() const { return "(FloatExprNode " + std::to_string(value) + getTypeString() + ")"; }

    StringExprNode::StringExprNode(Parser::StringNodePtr stringNode) : stringNode(std::move(stringNode)) {}

    std::string StringExprNode::sExpression() const {
        return "(StringExpressionNode " + stringNode->sExpression() + getTypeString() + ")";
    }

    BoolExprNode::BoolExprNode(bool value) : value(value) {}

    std::string BoolExprNode::sExpression() const {
        const std::string val = value ? "true" : "false";
        return "(BoolExprNode " + val + getTypeString() + ")";
    }

    VarExprNode::VarExprNode(Parser::VariableNodePtr variable) : variable(std::move(variable)) {}

    std::string VarExprNode::sExpression() const { return "(VarExprNode " + variable->sExpression() + getTypeString() + ")"; }

    ArrayExprNode::ArrayExprNode(const std::vector<ExprNodePtr>& expressions) : expressions(expressions) {}

    std::string ArrayExprNode::sExpression() const {
        std::stringstream builder;
        builder << "(ArrayExprNode";
        for (const ExprNodePtr& expr : expressions) { builder << " " << expr->sExpression(); }
        return builder.str() + getTypeString() + ")";
    }

    ArrayIndexExprNode::ArrayIndexExprNode(Parser::ExprNodePtr array, const std::vector<ExprNodePtr>& indexExprs)
        : array(std::move(array)), indexExprs(indexExprs) {}

    std::string ArrayIndexExprNode::sExpression() const {
        std::stringstream builder;
        builder << "(ArrayIndexExprNode " << array->sExpression();
        for (const ExprNodePtr& indexExpr : indexExprs) { builder << " " << indexExpr->sExpression(); }
        builder << getTypeString() + ")";
        return builder.str();
    }

    TupleExprNode::TupleExprNode(const std::vector<ExprNodePtr>& expressions) : expressions(expressions) {}

    std::string TupleExprNode::sExpression() const {
        std::stringstream builder;
        builder << "(TupleExprNode";
        for (const ExprNodePtr& expr : expressions) { builder << " " << expr->sExpression(); }
        return builder.str() + getTypeString() + ")";
    }

    TupleIndexExprNode::TupleIndexExprNode(Parser::ExprNodePtr tuple, Parser::ExprNodePtr index)
        : tuple(std::move(tuple)), index(std::move(index)) {}

    std::string TupleIndexExprNode::sExpression() const {
        return "(TupleIndexExprNode " + tuple->sExpression() + " " + index->sExpression() + getTypeString() + ")";
    }

    TernaryExprNode::TernaryExprNode(Parser::ExprNodePtr condition, Parser::ExprNodePtr trueExpr,
                                     Parser::ExprNodePtr falseExpr)
        : condition(std::move(condition)), trueExpr(std::move(trueExpr)), falseExpr(std::move(falseExpr)) {}

    std::string TernaryExprNode::sExpression() const {
        return "(TernaryExprNode " + condition->sExpression() + " ? " + trueExpr->sExpression() + " : "
             + falseExpr->sExpression() + getTypeString() + ")";
    }

    UnOpExprNode::UnOpExprNode(Parser::UnOpType opType, Parser::ExprNodePtr expr)
        : opType(opType), expr(std::move(expr)) {}

    std::string UnOpExprNode::sExpression() const {
        return "(UnOpExprNode " + unOpToString(opType) + " " + expr->sExpression() + getTypeString() + ")";
    }

    BinOpExprNode::BinOpExprNode(Parser::ExprNodePtr leftExpr, Parser::BinOpType opType, Parser::ExprNodePtr rightExpr)
        : leftExpr(std::move(leftExpr)), opType(opType), rightExpr(std::move(rightExpr)) {}

    BinOpExprNode::BinOpExprNode(Parser::VariableNodePtr leftVariable, Parser::BinOpType opType,
                                 Parser::ExprNodePtr rightExpr)
        : leftVariable(std::move(leftVariable)), opType(opType), rightExpr(std::move(rightExpr)) {}

    std::string BinOpExprNode::sExpression() const {
        return "(BinOpExprNode "
             + (leftExpr == nullptr ? (ASTNodePtr)leftVariable : (ASTNodePtr)leftExpr)->sExpression() + " "
             + binOpToString(opType) + " " + rightExpr->sExpression() + getTypeString() + ")";
    }

    CallExprNode::CallExprNode(Parser::ExprNodePtr func, const std::vector<ExprNodePtr>& args)
        : func(std::move(func)), args(args) {}

    std::string CallExprNode::sExpression() const {
        std::stringstream builder;
        for (const ExprNodePtr& arg : args) { builder << " " << arg->sExpression(); }
        return "(CallExprNode  " + func->sExpression() + builder.str() + getTypeString() + ")";
    }

    LambdaExprNode::LambdaExprNode(const std::vector<BindingNodePtr>& bindings, Parser::ExprNodePtr body,
                                   Parser::TypeNodePtr returnType)
        : bindings(bindings), body(std::make_shared<ReturnStmtNode>(std::move(body))), returnType(std::move(returnType)) {}

    LambdaExprNode::LambdaExprNode(const std::vector<BindingNodePtr>& bindings, Parser::StmtNodePtr body,
                                   Parser::TypeNodePtr returnType)
        : bindings(bindings), body(std::move(body)), returnType(std::move(returnType)) {}

    std::string LambdaExprNode::sExpression() const {
        std::stringstream bindingsBuilder;
        bool first = true;
        for (const BindingNodePtr& binding : bindings) {
            bindingsBuilder << (first ? "" : " ") << binding->sExpression();
            first = false;
        }
        return "(LambdaExprNode (" + bindingsBuilder.str() + ") " + returnType->sExpression() + " " + body->sExpression() + getTypeString()
             + ")";
    }

    CmdExprNode::CmdExprNode(Parser::CmdNodePtr cmd) : cmd(std::move(cmd)) {}

    std::string CmdExprNode::sExpression() const { return "(CmdExprNode " + cmd->sExpression() + getTypeString() + ")"; }


    //  ===================
    //  ||  Statements:  ||
    //  ===================

    IfStmtNode::IfStmtNode(const std::vector<ExprNodePtr>& conditions, std::vector<StmtNodePtr> trueStmts,
                           Parser::StmtNodePtr falseStmt)
        : conditions(conditions), trueStmts(std::move(trueStmts)), falseStmt(std::move(falseStmt)) {}

    std::string IfStmtNode::sExpression() const {
        std::stringstream builder;
        for (int i = 0; i < (int)conditions.size(); i++) {
            builder << " " << conditions[i]->sExpression();
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

    std::string IntTypeNode::sExpression() const { return "(IntTypeNode" + getTypeString() + ")"; }

    std::string FloatTypeNode::sExpression() const { return "(FloatTypeNode" + getTypeString() + ")"; }

    std::string BoolTypeNode::sExpression() const { return "(BoolTypeNode" + getTypeString() + ")"; }

    std::string StringTypeNode::sExpression() const { return "(StringTypeNode" + getTypeString() + ")"; }

    ArrayTypeNode::ArrayTypeNode(Parser::TypeNodePtr subtype) : subtype(std::move(subtype)) {}

    std::string ArrayTypeNode::sExpression() const { return "(ArrayTypeNode " + subtype->sExpression() + getTypeString() + ")"; }

    TupleTypeNode::TupleTypeNode(const std::vector<TypeNodePtr>& subtypes) : subtypes(subtypes) {}

    std::string TupleTypeNode::sExpression() const {
        std::stringstream builder;
        for (const TypeNodePtr& subType : subtypes) { builder << " " << subType->sExpression(); }
        return "(TupleTypeNode" + builder.str() + getTypeString() + ")";
    }

    FunctionTypeNode::FunctionTypeNode(const std::vector<TypeNodePtr>& params) : params(params) {}

    std::string FunctionTypeNode::sExpression() const {
        std::stringstream builder;
        for (const TypeNodePtr& param : params) { builder << " " << param->sExpression(); }
        return "(FunctionTypeNode" + builder.str() + getTypeString() + ")";
    }

    IdentVariableNode::IdentVariableNode(std::string varName) : varName(std::move(varName)) {}

    std::string IdentVariableNode::sExpression() const { return "(IdentVariableNode " + varName + ")"; }

}  //  namespace Parser
