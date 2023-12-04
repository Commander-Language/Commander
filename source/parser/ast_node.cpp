/**
* @file source/parser/ast_node.cpp
* @brief Implements the functions within the `ASTNode` class and subclasses.
*
*/

#include "ast_node.hpp"

namespace Parser {

    std::string unopToString(const Unop& unop) {
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

    std::string binopToString(const Binop& binop) {
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

    std::string BindingNode::sExpression() const {
        return "(Binding " + variable + " " + type->sExpression() + ")";
    }

    std::string CmdNode::sExpression() const {
        std::ostringstream builder;
        for (const ASTNode& arg : arguments) {
            builder << " " << arg.sExpression();
        }
        return "(CmdNode" + builder.str() + ")";
    }

    std::string StringNode::sExpression() const {
        std::ostringstream builder;
        for (int i = 0; i < (int)literals.size(); i++) {
            builder << " " << literals[i];
            if (i < (int)expressions.size()) {
                builder << " " << expressions[i]->sExpression();
            }
        }
        return "(StringNode" + builder.str() + ")";
    }

    std::string PrgmNode::sExpression() const {
        std::ostringstream builder;
        for (const StmtNodePtr& stmt : stmts) {
            builder << " " << stmt->sExpression();
        }
        return "(PrgmNode" + builder.str() + ")";
    }

    std::string PipeCmdNode::sExpression() const {
        return "(PipeCmdNode " + leftCmd->sExpression() + " | " + rightCmd->sExpression() + ")";
    }

    std::string IntExprNode::sExpression() const {
        return "(IntExprNode " + std::to_string(value) + ")";
    }

    std::string FloatExprNode::sExpression() const {
        return "(FloatExprNode " + std::to_string(value) + ")";
    }

    std::string BoolExprNode::sExpression() const {
        std::string val = value ? "true" : "false";
        return "(BoolExprNode " + val + ")";
    }

    std::string VarExprNode::sExpression() const {
        return "(VarExprNode " + variable + ")";
    }

    std::string ArrayExprNode::sExpression() const {
        std::ostringstream builder;
        for (const ExprNodePtr& expr : expressions) {
            builder << " " << expr->sExpression();
        }
        return "(ArrayExprNode " + builder.str() + ")";
    }

    std::string ArrayIndexExprNode::sExpression() const {
        std::ostringstream builder;
        for (const ExprNodePtr& expr : expressions) {
            builder << " " << expr->sExpression();
        }
        return "(ArrayIndexExprNode " + expression->sExpression() + builder.str() + ")";
    }

    std::string TupleExprNode::sExpression() const {
        std::ostringstream builder;
        for (const ExprNodePtr& expr : expressions) {
            builder << " " << expr->sExpression();
        }
        return "(TupleExprNode " + builder.str() + ")";
    }

    std::string TupleIndexExprNode::sExpression() const {
        return "(TupleIndexExprNode " + expression->sExpression() + " " + indexExpression->sExpression() + ")";
    }

    std::string TernaryExprNode::sExpression() const {
        return "(TernaryExprNode " + condition->sExpression() + " ? " + trueExpr->sExpression() + " : " + falseExpr->sExpression() + ")";
    }

    std::string UnOpExprNode::sExpression() const {
        return "(UnOpExprNode " + unopToString(op) + " " + expression->sExpression() + ")";
    }

    std::string BinOpExprNode::sExpression() const {
        return "(BinOpExprNode " + binopToString(op) + " " + expression->sExpression() + ")";
    }

    std::string CallExprNode::sExpression() const {
        std::ostringstream builder;
        for (const ExprNodePtr& arg : arguments) {
            builder << " " << arg->sExpression();
        }
        return "(CallExprNode " + function->sExpression() + builder.str() + ")";
    }

    std::string LambdaExprNode::sExpression() const {
        std::ostringstream bindingsBuilder;
        bool first = true;
        for (const BindingNodePtr& binding : bindings) {
            bindingsBuilder << (first ? "" : " ") << binding->sExpression();
            first = false;
        }
        return "(LambdaExprNode (" + bindingsBuilder.str() + ") " + type->sExpression() + " " + statement->sExpression() + ")";
    }

    std::string CmdExprNode::sExpression() const {
        return "(CmdExprNode " + cmd->sExpression() + ")";
    }

    std::string IfStmtNode::sExpression() const {
        std::ostringstream builder;
        for (int i = 0; i < (int)conditions.size(); i++) {
            builder << " " << conditions[i]->sExpression();
            builder << " " << statements[i]->sExpression();
        }
        return "(IfStmtNode" + builder.str() + " " + falseStatement->sExpression() + ")";
    }

    std::string ForStmtNode::sExpression() const {
        return "(ForStmtNode " + initialization->sExpression() + " " + condition->sExpression() + " " + update->sExpression() + " " + body->sExpression() + ")";
    }

    std::string WhileStmtNode::sExpression() const {
        return "(WhileStmtNode " + condition->sExpression() + " "  + body->sExpression() + ")";
    }

    std::string DoWhileStmtNode::sExpression() const {
        return "(DoWhileStmtNode " + condition->sExpression() + " " + body->sExpression() + ")";
    }

    std::string ScopeStmtNode::sExpression() const {
        std::ostringstream builder;
        for (const StmtNodePtr& stmt : stmts) {
            builder << " " << stmt->sExpression();
        }
        return "(ScopeStmtNode " + builder.str() + ")";
    }

    std::string CmdStmtNode::sExpression() const {
        return "(CmdStmtNode " + command->sExpression() + ")";
    }

    std::string AliasStmtNode::sExpression() const {
        return "(AliasStmtNode " + alias + " " + command->sExpression() + ")";
    }

    std::string IntTypeNode::sExpression() const {
        return "(IntTypeNode)";
    }

    std::string FloatTypeNode::sExpression() const {
        return "(FloatTypeNode)";
    }

    std::string ArrayTypeNode::sExpression() const {
        return "(ArrayTypeNode)";
    }

    std::string TupleTypeNode::sExpression() const {
        return "(TupleTypeNode)";
    }
}