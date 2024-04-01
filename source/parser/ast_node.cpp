/**
 * @file source/parser/ast_node.cpp
 * @brief Implements the functions within the `ASTNode` class and subclasses.
 *
 */

#include "ast_node.hpp"

#include <sstream>
#include <utility>

namespace Parser {

    std::string nodeTypeToString(ASTNodeType nodeType) {
        switch (nodeType) {
            case BINDING:
                return "BINDING";
            case BINDINGS:
                return "BINDINGS";
            case CMD:
                return "CMD";
            case BASIC_CMD:
                return "BASIC_CMD";
            case PIPE_CMD:
                return "PIPE_CMD";
            case ASYNC_CMD:
                return "ASYNC_CMD";
            case LVALUE:
                return "LVALUE";
            case VAR_LVALUE:
                return "VAR_LVALUE";
            case INDEX_LVALUE:
                return "INDEX_LVALUE";
            case INT_EXPR:
                return "INT_EXPR";
            case FLOAT_EXPR:
                return "FLOAT_EXPR";
            case STRING_EXPR:
                return "STRING_EXPR";
            case BOOL_EXPR:
                return "BOOL_EXPR";
            case VAR_EXPR:
                return "VAR_EXPR";
            case LVALUE_EXPR:
                return "LVALUE_EXPR";
            case ARRAY_EXPR:
                return "ARRAY_EXPR";
            case INDEX_EXPR:
                return "INDEX_EXPR";
            case TUPLE_EXPR:
                return "TUPLE_EXPR";
            case TERNARY_EXPR:
                return "TERNARY_EXPR";
            case UNOP_EXPR:
                return "UNOP_EXPR";
            case BINOP_EXPR:
                return "BINOP_EXPR";
            case CALL_EXPR:
                return "CALL_EXPR";
            case API_CALL_EXPR:
                return "API_CALL_EXPR";
            case LAMBDA_EXPR:
                return "LAMBDA_EXPR";
            case CMD_EXPR:
                return "CMD_EXPR";
            case SCAN_EXPR:
                return "SCAN_EXPR";
            case READ_EXPR:
                return "READ_EXPR";
            case EXPR:
                return "EXPR";
            case EXPRS:
                return "EXPRS";
            case PRGM:
                return "PRGM";
            case IF_STMT:
                return "IF_STMT";
            case FOR_STMT:
                return "FOR_STMT";
            case WHILE_STMT:
                return "WHILE_STMT";
            case DO_WHILE_STMT:
                return "DO_WHILE_STMT";
            case RETURN_STMT:
                return "RETURN_STMT";
            case BREAK_STMT:
                return "BREAK_STMT";
            case CONTINUE_STMT:
                return "CONTINUE_STMT";
            case SCOPE_STMT:
                return "SCOPE_STMT";
            case CMD_STMT:
                return "CMD_STMT";
            case EXPR_STMT:
                return "EXPR_STMT";
            case ALIAS_STMT:
                return "ALIAS_STMT";
            case IMPORT_STMT:
                return "IMPORT_STMT";
            case PRINT_STMT:
                return "PRINT_STMT";
            case PRINTLN_STMT:
                return "PRINTLN_STMT";
            case WRITE_STMT:
                return "WRITE_STMT";
            case TYPE_STMT:
                return "TYPE_STMT";
            case FUNCTION_STMT:
                return "FUNCTION_STMT";
            case TIMEOUT_STMT:
                return "TIMEOUT_STMT";
            case ASSERT_STMT:
                return "ASSERT_STMT";
            case STMT:
                return "STMT";
            case STMTS:
                return "STMTS";
            case STRING:
                return "STRING";
            case STRING_EXPRS:
                return "STRING_EXPRS";
            case INT_TYPE:
                return "INT_TYPE";
            case FLOAT_TYPE:
                return "FLOAT_TYPE";
            case BOOL_TYPE:
                return "BOOL_TYPE";
            case STRING_TYPE:
                return "STRING_TYPE";
            case ARRAY_TYPE:
                return "ARRAY_TYPE";
            case TUPLE_TYPE:
                return "TUPLE_TYPE";
            case FUNCTION_TYPE:
                return "FUNCTION_TYPE";
            case VARIABLE_TYPE:
                return "VARIABLE_TYPE";
            case TYPE:
                return "TYPE";
            case TYPES:
                return "TYPES";
            default:
                return "UNKNOWN";
        }
    }

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
            case EQUAL:
                return "EQUAL";
            case NOT_EQUAL:
                return "NOT_EQUAL";
            case AND:
                return "AND";
            case OR:
                return "OR";
            case EXPONENTIATE:
                return "EXPONENTIATE";
            case MULTIPLY:
                return "MULTIPLY";
            case DIVIDE:
                return "DIVIDE";
            case MODULO:
                return "MODULO";
            case ADD:
                return "ADD";
            case SUBTRACT:
                return "SUBTRACT";
            case EXPONENTIATE_SET:
                return "EXPONENTIATE_SET";
            case MULTIPLY_SET:
                return "MULTIPLY_SET";
            case DIVIDE_SET:
                return "DIVIDE_SET";
            case MODULO_SET:
                return "MODULO_SET";
            case ADD_SET:
                return "ADD_SET";
            case SUBTRACT_SET:
                return "SUBTRACT_SET";
            case SET:
                return "SET";
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

    ASTNode::ASTNode(Lexer::FilePosition position) : position(std::move(position)) {}

    TypeNode::TypeNode(Lexer::FilePosition position) : ASTNode(std::move(position)) {}

    CmdNode::CmdNode(Lexer::FilePosition position) : ASTNode(std::move(position)) {}

    LValueNode::LValueNode(Lexer::FilePosition position) : ASTNode(std::move(position)) {}

    ExprNode::ExprNode(Lexer::FilePosition position) : ASTNode(std::move(position)) {}

    StmtNode::StmtNode(Lexer::FilePosition position) : ASTNode(std::move(position)) {}

    ASTNodeType IntTypeNode::nodeType() const { return ASTNodeType::INT_TYPE; }

    ASTNodeType FloatTypeNode::nodeType() const { return ASTNodeType::FLOAT_TYPE; }

    ASTNodeType BoolTypeNode::nodeType() const { return ASTNodeType::BOOL_TYPE; }

    ASTNodeType StringTypeNode::nodeType() const { return ASTNodeType::STRING_TYPE; }

    ASTNodeType ArrayTypeNode::nodeType() const { return ASTNodeType::ARRAY_TYPE; }

    ASTNodeType TupleTypeNode::nodeType() const { return ASTNodeType::TUPLE_TYPE; }

    ASTNodeType FunctionTypeNode::nodeType() const { return ASTNodeType::FUNCTION_TYPE; }

    ASTNodeType VariableTypeNode::nodeType() const { return ASTNodeType::VARIABLE_TYPE; }

    std::string TypeNode::getTypeString() const { return TypeChecker::getTypeString(type); }

    TypesNode::TypesNode(Lexer::FilePosition position) : ASTNode(std::move(position)) {}

    TypesNode::TypesNode(const TypeNodePtr& type) : types({type}), ASTNode(type->position) {}

    TypesNode::TypesNode(const std::shared_ptr<TypesNode>& types, TypeNodePtr type)
        : types(concat(types->types, std::move(type))), ASTNode(types->position) {}

    ASTNodeType TypesNode::nodeType() const { return ASTNodeType::TYPES; }

    std::string TypesNode::sExpression() const {
        std::stringstream result;
        result << "(TypesNode";
        for (const auto& type : types) { result << " " << type->sExpression(); }
        result << ")";
        return result.str();
    }

    BindingNode::BindingNode(Lexer::FilePosition position, std::string variable, bool constant, TypeNodePtr type)
        : variable(std::move(variable)), constant(constant), type(std::move(type)), ASTNode(std::move(position)) {}

    ASTNodeType BindingNode::nodeType() const { return ASTNodeType::BINDING; }

    std::string BindingNode::sExpression() const {
        return "(BindingNode " + std::string(constant ? "const " : "") + variable
             + (type ? " " + type->sExpression() : "") + ")";
    }

    ASTNodeType BindingsNode::nodeType() const { return ASTNodeType::BINDINGS; }

    std::string BindingsNode::sExpression() const {
        std::stringstream result;
        result << "(BindingsNode";
        for (const auto& binding : bindings) { result << " " << binding->sExpression(); }
        result << ")";
        return result.str();
    }

    BindingsNode::BindingsNode(Lexer::FilePosition position) : ASTNode(std::move(position)) {}

    BindingsNode::BindingsNode(BindingNodePtr binding) : bindings({binding}), ASTNode(binding->position) {}

    BindingsNode::BindingsNode(const std::shared_ptr<BindingsNode>& bindings, BindingNodePtr binding)
        : bindings(concat(bindings->bindings, std::move(binding))), ASTNode(bindings->position) {}

    std::string ExprNode::getTypeString() const { return TypeChecker::getTypeString(type); }

    std::string LValueNode::getTypeString() const { return TypeChecker::getTypeString(type); }

    ASTNodeType BasicCmdNode::nodeType() const { return ASTNodeType::BASIC_CMD; }

    ASTNodeType PipeCmdNode::nodeType() const { return ASTNodeType::PIPE_CMD; }

    ASTNodeType AsyncCmdNode::nodeType() const { return ASTNodeType::ASYNC_CMD; }

    ASTNodeType VarLValueNode::nodeType() const { return ASTNodeType::VAR_LVALUE; }

    ASTNodeType IndexLValueNode::nodeType() const { return ASTNodeType::INDEX_LVALUE; }

    ASTNodeType IntExprNode::nodeType() const { return ASTNodeType::INT_EXPR; }

    ASTNodeType FloatExprNode::nodeType() const { return ASTNodeType::FLOAT_EXPR; }

    ASTNodeType StringExprNode::nodeType() const { return ASTNodeType::STRING_EXPR; }

    ASTNodeType BoolExprNode::nodeType() const { return ASTNodeType::BOOL_EXPR; }

    ASTNodeType VarExprNode::nodeType() const { return ASTNodeType::VAR_EXPR; }

    ASTNodeType LValueExprNode::nodeType() const { return ASTNodeType::LVALUE_EXPR; }

    ASTNodeType ArrayExprNode::nodeType() const { return ASTNodeType::ARRAY_EXPR; }

    ASTNodeType IndexExprNode::nodeType() const { return ASTNodeType::INDEX_EXPR; }

    ASTNodeType TupleExprNode::nodeType() const { return ASTNodeType::TUPLE_EXPR; }

    ASTNodeType TernaryExprNode::nodeType() const { return ASTNodeType::TERNARY_EXPR; }

    ASTNodeType UnOpExprNode::nodeType() const { return ASTNodeType::UNOP_EXPR; }

    ASTNodeType BinOpExprNode::nodeType() const { return ASTNodeType::BINOP_EXPR; }

    ASTNodeType CallExprNode::nodeType() const { return ASTNodeType::CALL_EXPR; }

    ASTNodeType ApiCallExprNode::nodeType() const { return ASTNodeType::API_CALL_EXPR; }

    ASTNodeType LambdaExprNode::nodeType() const { return ASTNodeType::LAMBDA_EXPR; }

    ASTNodeType CmdExprNode::nodeType() const { return ASTNodeType::CMD_EXPR; }

    ASTNodeType ScanExprNode::nodeType() const { return ASTNodeType::SCAN_EXPR; }

    ASTNodeType ReadExprNode::nodeType() const { return ASTNodeType::READ_EXPR; }

    ExprsNode::ExprsNode(Lexer::FilePosition position) : ASTNode(std::move(position)) {}

    ExprsNode::ExprsNode(const ExprNodePtr& expr) : exprs({expr}), ASTNode(expr->position) {}

    ExprsNode::ExprsNode(const std::shared_ptr<ExprsNode>& exprs, ExprNodePtr expr)
        : exprs(concat(exprs->exprs, std::move(expr))), ASTNode(exprs->position) {}

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

    ASTNodeType BreakStmtNode::nodeType() const { return ASTNodeType::BREAK_STMT; }

    ASTNodeType ContinueStmtNode::nodeType() const { return ASTNodeType::CONTINUE_STMT; }

    ASTNodeType ScopeStmtNode::nodeType() const { return ASTNodeType::SCOPE_STMT; }

    ASTNodeType CmdStmtNode::nodeType() const { return ASTNodeType::CMD_STMT; }

    ASTNodeType ExprStmtNode::nodeType() const { return ASTNodeType::EXPR_STMT; }

    ASTNodeType AliasStmtNode::nodeType() const { return ASTNodeType::ALIAS_STMT; }

    ASTNodeType ImportStmtNode::nodeType() const { return ASTNodeType::IMPORT_STMT; }

    ASTNodeType PrintStmtNode::nodeType() const { return ASTNodeType::PRINT_STMT; }

    ASTNodeType PrintlnStmtNode::nodeType() const { return ASTNodeType::PRINTLN_STMT; }

    ASTNodeType WriteStmtNode::nodeType() const { return ASTNodeType::WRITE_STMT; }

    ASTNodeType TypeStmtNode::nodeType() const { return ASTNodeType::TYPE_STMT; }

    ASTNodeType FunctionStmtNode::nodeType() const { return ASTNodeType::FUNCTION_STMT; }

    ASTNodeType TimeoutStmtNode::nodeType() const { return ASTNodeType::TIMEOUT_STMT; }

    ASTNodeType AssertStmtNode::nodeType() const { return ASTNodeType::ASSERT_STMT; }

    StmtsNode::StmtsNode(Lexer::FilePosition position) : ASTNode(std::move(position)) {}

    StmtsNode::StmtsNode(const StmtNodePtr& stmt) : stmts({stmt}), ASTNode(stmt->position) {}

    StmtsNode::StmtsNode(const std::shared_ptr<StmtsNode>& stmts, StmtNodePtr stmt)
        : stmts(concat(stmts->stmts, std::move(stmt))), ASTNode(stmts->position) {}

    ASTNodeType StmtsNode::nodeType() const { return ASTNodeType::STMTS; }

    std::string StmtsNode::sExpression() const {
        std::stringstream result;
        result << "(StmtsNode";
        for (const auto& stmt : stmts) { result << "\n\t" << stmt->sExpression(); }
        result << ")";
        return result.str();
    }

    StringExprsNode::StringExprsNode(const Lexer::FilePosition& position, std::string literal) : ASTNode(position) {
        expressions.push_back(std::make_shared<StringExprNode>(
                std::make_shared<StringNode>(StringNode(position, std::move(literal)))));
    }

    StringExprsNode::StringExprsNode(Lexer::FilePosition position, std::string literal, ExprNodePtr expr,
                                     const std::shared_ptr<StringExprsNode>& exprs)
        : ASTNode(exprs->position) {
        expressions.insert(expressions.end(), exprs->expressions.begin(), exprs->expressions.end());
        expressions.push_back(std::move(expr));
        expressions.push_back(std::make_shared<StringExprNode>(
                std::make_shared<StringNode>(StringNode(std::move(position), std::move(literal)))));
    }

    ASTNodeType StringExprsNode::nodeType() const { return ASTNodeType::STRING_EXPRS; }

    std::string StringExprsNode::sExpression() const {
        std::stringstream builder;
        builder << "(StringExprsNode";
        for (const ExprNodePtr& expr : expressions) { builder << " " << expr->sExpression(); }
        builder << ")";
        return builder.str();
    }

    StringNode::StringNode(Lexer::FilePosition position, std::string literal)
        : literal(std::move(literal)), ASTNode(std::move(position)) {}

    StringNode::StringNode(Lexer::FilePosition position, StringExprsNodePtr exprs)
        : expressions(std::move(exprs)), ASTNode(std::move(position)) {}

    bool StringNode::isLiteral() const { return expressions == nullptr; }

    ASTNodeType StringNode::nodeType() const { return ASTNodeType::STRING; }

    std::string StringNode::sExpression() const {
        return "(StringNode " + (isLiteral() ? "'" + literal + "'" : expressions->sExpression()) + ")";
    }

    PrgmNode::PrgmNode(const StmtsNodePtr& stmts) : stmts(stmts), ASTNode(stmts->position) {}

    ASTNodeType PrgmNode::nodeType() const { return PRGM; }

    std::string PrgmNode::sExpression() const {
        std::stringstream builder;
        for (const StmtNodePtr& stmt : stmts->stmts) { builder << "\n\t" << stmt->sExpression(); }
        return "(PrgmNode" + builder.str() + ")";
    }

    //  =================
    //  ||  Commands:  ||
    //  =================

    BasicCmdNode::BasicCmdNode(const ASTNodePtr& argument) : arguments({argument}), CmdNode(argument->position) {}

    BasicCmdNode::BasicCmdNode(const BasicCmdNodePtr& first, const BasicCmdNodePtr& second) : CmdNode(first->position) {
        arguments.insert(arguments.end(), first->arguments.begin(), first->arguments.end());
        arguments.insert(arguments.end(), second->arguments.begin(), second->arguments.end());
    }

    std::string BasicCmdNode::sExpression() const {
        std::stringstream builder;
        for (const ASTNodePtr& arg : arguments) { builder << " " << arg->sExpression(); }
        return "(BasicCmdNode" + builder.str() + ")";
    }

    AsyncCmdNode::AsyncCmdNode(const CmdNodePtr& cmd) : cmd(cmd), CmdNode(cmd->position) {}

    std::string AsyncCmdNode::sExpression() const { return "(AsyncCmdNode " + cmd->sExpression() + ")"; }

    PipeCmdNode::PipeCmdNode(const CmdNodePtr& leftCmd, CmdNodePtr rightCmd)
        : leftCmd(leftCmd), rightCmd(std::move(rightCmd)), CmdNode(leftCmd->position) {}

    std::string PipeCmdNode::sExpression() const {
        return "(PipeCmdNode " + leftCmd->sExpression() + " | " + rightCmd->sExpression() + ")";
    }

    //  ================
    //  ||  Lvalues:  ||
    //  ================

    VarLValueNode::VarLValueNode(Lexer::FilePosition position, std::string variable)
        : variable(std::move(variable)), LValueNode(std::move(position)) {}

    std::string VarLValueNode::sExpression() const { return "(VarLValueNode " + variable + getTypeString() + ")"; }

    IndexLValueNode::IndexLValueNode(const Parser::LValueNodePtr& lvalue, Parser::ExprNodePtr index)
        : lvalue(lvalue), index(std::move(index)), LValueNode(lvalue->position) {}

    std::string IndexLValueNode::sExpression() const {
        return "(IndexLValueNode " + lvalue->sExpression() + " " + index->sExpression() + getTypeString() + ")";
    }

    //  ====================
    //  ||  Expressions:  ||
    //  ====================

    IntExprNode::IntExprNode(Lexer::FilePosition position, int64_t value)
        : value(value), ExprNode(std::move(position)) {}

    std::string IntExprNode::sExpression() const {
        return "(IntExprNode " + std::to_string(value) + getTypeString() + ")";
    }

    FloatExprNode::FloatExprNode(Lexer::FilePosition position, double value)
        : value(value), ExprNode(std::move(position)) {}

    std::string FloatExprNode::sExpression() const {
        return "(FloatExprNode " + std::to_string(value) + getTypeString() + ")";
    }

    StringExprNode::StringExprNode(const StringNodePtr& stringNode)
        : stringNode(stringNode), ExprNode(stringNode->position) {}

    std::string StringExprNode::sExpression() const {
        return "(StringExpressionNode " + stringNode->sExpression() + getTypeString() + ")";
    }

    BoolExprNode::BoolExprNode(Lexer::FilePosition position, bool value)
        : value(value), ExprNode(std::move(position)) {}

    std::string BoolExprNode::sExpression() const {
        const std::string val = value ? "true" : "false";
        return "(BoolExprNode " + val + getTypeString() + ")";
    }

    VarExprNode::VarExprNode(Lexer::FilePosition position, std::string variable)
        : variable(std::move(variable)), ExprNode(std::move(position)) {}

    VarExprNode::VarExprNode(const VarLValueNodePtr& lvalue) : variable(lvalue->variable), ExprNode(lvalue->position) {}

    std::string VarExprNode::sExpression() const { return "(VarExprNode " + variable + getTypeString() + ")"; }

    LValueExprNode::LValueExprNode(const LValueNodePtr& lvalue) : ExprNode(lvalue->position) {
        if (lvalue->nodeType() == VAR_LVALUE) {
            expr = std::make_shared<VarExprNode>(std::static_pointer_cast<VarLValueNode>(lvalue));
        } else {
            expr = std::make_shared<IndexExprNode>(std::static_pointer_cast<IndexLValueNode>(lvalue));
        }
    }

    std::string LValueExprNode::sExpression() const {
        return "(LValueExprNode " + expr->sExpression() + getTypeString() + ")";
    }

    ArrayExprNode::ArrayExprNode(const Lexer::FilePosition& position)
        : expressions(std::make_shared<ExprsNode>(position)), ExprNode(position) {}

    ArrayExprNode::ArrayExprNode(Lexer::FilePosition position, ExprsNodePtr expressions)
        : expressions(std::move(expressions)), ExprNode(std::move(position)) {}

    std::string ArrayExprNode::sExpression() const {
        return "(ArrayExprNode " + expressions->sExpression() + getTypeString() + ")";
    }

    IndexExprNode::IndexExprNode(const ExprNodePtr& expr, ExprNodePtr index)
        : expr(expr), index(std::move(index)), ExprNode(expr->position) {}

    IndexExprNode::IndexExprNode(const IndexLValueNodePtr& indexLValue)
        : index(indexLValue->index), ExprNode(indexLValue->position) {
        if (indexLValue->lvalue->nodeType() == VAR_LVALUE) {
            expr = std::make_shared<VarExprNode>(
                    indexLValue->lvalue->position,
                    std::static_pointer_cast<VarLValueNode>(indexLValue->lvalue)->variable);
        } else {
            expr = std::make_shared<IndexExprNode>(std::static_pointer_cast<IndexLValueNode>(indexLValue->lvalue));
        }
    }

    std::string IndexExprNode::sExpression() const {
        return "(IndexExprNode " + expr->sExpression() + " " + index->sExpression() + getTypeString() + ")";
    }

    TupleExprNode::TupleExprNode(const Lexer::FilePosition& position)
        : expressions(std::make_shared<ExprsNode>(position)), ExprNode(position) {}

    TupleExprNode::TupleExprNode(Lexer::FilePosition position, ExprsNodePtr expressions)
        : expressions(std::move(expressions)), ExprNode(std::move(position)) {}

    std::string TupleExprNode::sExpression() const {
        return "(TupleExprNode " + expressions->sExpression() + getTypeString() + ")";
    }

    TernaryExprNode::TernaryExprNode(Lexer::FilePosition position, ExprNodePtr condition, ExprNodePtr trueExpr,
                                     ExprNodePtr falseExpr)
        : condition(std::move(condition)), trueExpr(std::move(trueExpr)), falseExpr(std::move(falseExpr)),
          ExprNode(std::move(position)) {}

    std::string TernaryExprNode::sExpression() const {
        return "(TernaryExprNode " + condition->sExpression() + " " + trueExpr->sExpression() + " "
             + falseExpr->sExpression() + getTypeString() + ")";
    }

    UnOpExprNode::UnOpExprNode(Lexer::FilePosition position, UnOpType opType, ASTNodePtr node)
        : opType(opType), node(std::move(node)), ExprNode(std::move(position)) {}

    std::string UnOpExprNode::sExpression() const {
        return "(UnOpExprNode " + node->sExpression() + " " + unOpToString(opType) + getTypeString() + ")";
    }

    BinOpExprNode::BinOpExprNode(const ASTNodePtr& left, BinOpType opType, ExprNodePtr right)
        : opType(opType), left(left), right(std::move(right)), ExprNode(left->position) {}

    std::string BinOpExprNode::sExpression() const {
        return "(BinOpExprNode " + left->sExpression() + " " + binOpToString(opType) + " " + right->sExpression()
             + getTypeString() + ")";
    }

    CallExprNode::CallExprNode(const ExprNodePtr& func)
        : func(func), args(std::make_shared<ExprsNode>(func->position)), ExprNode(func->position) {}

    CallExprNode::CallExprNode(const ExprNodePtr& func, ExprsNodePtr args)
        : func(func), args(std::move(args)), ExprNode(func->position) {}

    std::string CallExprNode::sExpression() const {
        return "(CallExprNode " + func->sExpression() + " " + args->sExpression() + getTypeString() + ")";
    }

    ApiCallExprNode::ApiCallExprNode(const ExprNodePtr& expression, std::string func)
        : expression(expression), func(std::move(func)), args(std::make_shared<ExprsNode>(expression->position)),
          ExprNode(expression->position) {}

    ApiCallExprNode::ApiCallExprNode(const ExprNodePtr& expression, std::string func, ExprsNodePtr args)
        : expression(expression), func(std::move(func)), args(std::move(args)), ExprNode(expression->position) {}

    std::string ApiCallExprNode::sExpression() const {
        return "(ApiCallExprNode " + expression->sExpression() + " " + func + " " + args->sExpression()
             + getTypeString() + ")";
    }

    LambdaExprNode::LambdaExprNode(Lexer::FilePosition position, ExprNodePtr body, TypeNodePtr returnType)
        : bindings(std::make_shared<BindingsNode>(position)),
          body(std::make_shared<ReturnStmtNode>(body->position, body)), returnType(std::move(returnType)),
          ExprNode(position) {}

    LambdaExprNode::LambdaExprNode(const Lexer::FilePosition& position, StmtNodePtr body, TypeNodePtr returnType)
        : bindings(std::make_shared<BindingsNode>(position)), body(std::move(body)), returnType(std::move(returnType)),
          ExprNode(position) {}

    LambdaExprNode::LambdaExprNode(Lexer::FilePosition position, BindingsNodePtr bindings, const ExprNodePtr& body,
                                   TypeNodePtr returnType)
        : bindings(std::move(bindings)), body(std::make_shared<ReturnStmtNode>(body->position, body)),
          returnType(std::move(returnType)), ExprNode(std::move(position)) {}

    LambdaExprNode::LambdaExprNode(Lexer::FilePosition position, BindingsNodePtr bindings, StmtNodePtr body,
                                   TypeNodePtr returnType)
        : bindings(std::move(bindings)), body(std::move(body)), returnType(std::move(returnType)),
          ExprNode(std::move(position)) {}

    std::string LambdaExprNode::sExpression() const {
        return "(LambdaExprNode " + bindings->sExpression() + (returnType ? " " + returnType->sExpression() : "") + " "
             + body->sExpression() + getTypeString() + ")";
    }

    CmdExprNode::CmdExprNode(Lexer::FilePosition position, CmdNodePtr cmd)
        : cmd(std::move(cmd)), ExprNode(std::move(position)) {}

    std::string CmdExprNode::sExpression() const {
        return "(CmdExprNode " + cmd->sExpression() + getTypeString() + ")";
    }

    ScanExprNode::ScanExprNode(Lexer::FilePosition position, ExprNodePtr prompt)
        : prompt(std::move(prompt)), ExprNode(std::move(position)) {}

    std::string ScanExprNode::sExpression() const {
        return "(ScanExprNode " + prompt->sExpression() + getTypeString() + ")";
    }

    ReadExprNode::ReadExprNode(Lexer::FilePosition position, ExprNodePtr filePath)
        : filePath(std::move(filePath)), ExprNode(std::move(position)) {}

    std::string ReadExprNode::sExpression() const {
        return "(ReadExprNode " + filePath->sExpression() + getTypeString() + ")";
    }


    //  ===================
    //  ||  Statements:  ||
    //  ===================

    IfStmtNode::IfStmtNode(Lexer::FilePosition position, ExprNodePtr condition, StmtNodePtr trueStmt,
                           StmtNodePtr falseStmt)
        : condition(std::move(condition)), trueStmt(std::move(trueStmt)), falseStmt(std::move(falseStmt)),
          StmtNode(std::move(position)) {}

    std::string IfStmtNode::sExpression() const {
        return "(IfStmtNode " + condition->sExpression() + " " + trueStmt->sExpression()
             + (falseStmt ? " " + falseStmt->sExpression() : "") + ")";
    }

    ForStmtNode::ForStmtNode(Lexer::FilePosition position, StmtNodePtr initial, ExprNodePtr condition,
                             StmtNodePtr update, StmtNodePtr body)
        : initial(std::move(initial)), condition(std::move(condition)), update(std::move(update)),
          body(std::move(body)), StmtNode(std::move(position)) {}

    std::string ForStmtNode::sExpression() const {
        return "(ForStmtNode" + (initial ? " " + initial->sExpression() : "")
             + (condition ? " " + condition->sExpression() : "") + (update ? " " + update->sExpression() : "") + " "
             + body->sExpression() + ")";
    }

    WhileStmtNode::WhileStmtNode(Lexer::FilePosition position, ExprNodePtr condition, StmtNodePtr body)
        : condition(std::move(condition)), body(std::move(body)), StmtNode(std::move(position)) {}

    std::string WhileStmtNode::sExpression() const {
        return "(WhileStmtNode " + condition->sExpression() + " " + body->sExpression() + ")";
    }

    DoWhileStmtNode::DoWhileStmtNode(Lexer::FilePosition position, ExprNodePtr condition, StmtNodePtr body)
        : condition(std::move(condition)), body(std::move(body)), StmtNode(std::move(position)) {}

    std::string DoWhileStmtNode::sExpression() const {
        return "(DoWhileStmtNode " + condition->sExpression() + " " + body->sExpression() + ")";
    }

    ReturnStmtNode::ReturnStmtNode(Lexer::FilePosition position, ExprNodePtr retExpr)
        : retExpr(std::move(retExpr)), StmtNode(std::move(position)) {}

    std::string ReturnStmtNode::sExpression() const { return "(ReturnStmtNode " + retExpr->sExpression() + ")"; }

    BreakStmtNode::BreakStmtNode(Lexer::FilePosition position) : StmtNode(std::move(position)) {}

    std::string BreakStmtNode::sExpression() const { return "(BreakStmtNode)"; }

    ContinueStmtNode::ContinueStmtNode(Lexer::FilePosition position) : StmtNode(std::move(position)) {}

    std::string ContinueStmtNode::sExpression() const { return "(ContinueStmtNode)"; }

    ScopeStmtNode::ScopeStmtNode(const Lexer::FilePosition& position)
        : stmts(std::make_shared<StmtsNode>(position)), StmtNode(position) {}

    ScopeStmtNode::ScopeStmtNode(Lexer::FilePosition position, StmtsNodePtr stmts)
        : stmts(std::move(stmts)), StmtNode(std::move(position)) {}

    std::string ScopeStmtNode::sExpression() const { return "(ScopeStmtNode " + stmts->sExpression() + ")"; }

    CmdStmtNode::CmdStmtNode(const CmdNodePtr& command) : command(command), StmtNode(command->position) {}

    std::string CmdStmtNode::sExpression() const { return "(CmdStmtNode " + command->sExpression() + ")"; }

    ExprStmtNode::ExprStmtNode(const ExprNodePtr& expression)
        : expression(expression), StmtNode(expression->position) {}

    std::string ExprStmtNode::sExpression() const { return "(ExprStmtNode " + expression->sExpression() + ")"; }

    AliasStmtNode::AliasStmtNode(Lexer::FilePosition position, std::string alias, CmdNodePtr command)
        : alias(std::move(alias)), command(std::move(command)), StmtNode(std::move(position)) {}

    std::string AliasStmtNode::sExpression() const {
        return "(AliasStmtNode " + alias + " " + command->sExpression() + ")";
    }

    ImportStmtNode::ImportStmtNode(Lexer::FilePosition position, StringNodePtr filePath)
        : filePath(std::move(filePath)), StmtNode(std::move(position)) {}

    std::string ImportStmtNode::sExpression() const {
        std::stringstream builder;
        for (ASTNodePtr const& node : prgm) { builder << " " + node->sExpression(); }
        return "(ImportStmtNode " + filePath->sExpression() + builder.str() + ")";
    }

    PrintStmtNode::PrintStmtNode(Lexer::FilePosition position, ExprNodePtr expression)
        : expression(std::move(expression)), StmtNode(std::move(position)) {}

    std::string PrintStmtNode::sExpression() const { return "(PrintStmtNode " + expression->sExpression() + ")"; }

    PrintlnStmtNode::PrintlnStmtNode(Lexer::FilePosition position, ExprNodePtr expression)
        : expression(std::move(expression)), StmtNode(std::move(position)) {}

    std::string PrintlnStmtNode::sExpression() const { return "(PrintlnStmtNode " + expression->sExpression() + ")"; }

    WriteStmtNode::WriteStmtNode(Lexer::FilePosition position, ExprNodePtr fileData, ExprNodePtr filePath)
        : fileData(std::move(fileData)), filePath(std::move(filePath)), StmtNode(std::move(position)) {}

    std::string WriteStmtNode::sExpression() const {
        return "(WriteStmtNode " + fileData->sExpression() + " " + filePath->sExpression() + ")";
    }

    TypeStmtNode::TypeStmtNode(Lexer::FilePosition position, std::string alias, TypeNodePtr type)
        : alias(std::move(alias)), type(std::move(type)), StmtNode(std::move(position)) {}

    std::string TypeStmtNode::sExpression() const { return "(TypeStmtNode " + alias + " " + type->sExpression() + ")"; }

    IntTypeNode::IntTypeNode(Lexer::FilePosition position) : TypeNode(std::move(position)) {}

    FloatTypeNode::FloatTypeNode(Lexer::FilePosition position) : TypeNode(std::move(position)) {}

    BoolTypeNode::BoolTypeNode(Lexer::FilePosition position) : TypeNode(std::move(position)) {}

    StringTypeNode::StringTypeNode(Lexer::FilePosition position) : TypeNode(std::move(position)) {}

    FunctionStmtNode::FunctionStmtNode(Lexer::FilePosition position, std::string name, StmtNodePtr body,
                                       TypeNodePtr returnType)
        : name(std::move(name)), bindings(std::make_shared<BindingsNode>(position)), body(std::move(body)),
          returnType(std::move(returnType)), StmtNode(position) {}

    FunctionStmtNode::FunctionStmtNode(Lexer::FilePosition position, std::string name, BindingsNodePtr bindings,
                                       StmtNodePtr body, TypeNodePtr returnType)
        : name(std::move(name)), bindings(std::move(bindings)), body(std::move(body)),
          returnType(std::move(returnType)), StmtNode(std::move(position)) {}

    std::string FunctionStmtNode::sExpression() const {
        return "(FunctionStmtNode " + name + " " + bindings->sExpression()
             + (returnType ? " " + returnType->sExpression() : "") + " " + body->sExpression() + ")";
    }

    TimeoutStmtNode::TimeoutStmtNode(Lexer::FilePosition position, const int64_t& timeout, StringNodePtr message,
                                     Parser::StmtNodePtr stmt)
        : timeout(timeout), message(std::move(message)), stmt(std::move(stmt)), StmtNode(std::move(position)) {}

    std::string TimeoutStmtNode::sExpression() const {
        return "(TimeoutStmtNode " + std::to_string(timeout) + " " + message->sExpression() + " " + stmt->sExpression()
             + ")";
    }

    AssertStmtNode::AssertStmtNode(Lexer::FilePosition position, ExprNodePtr expr, StringNodePtr message)
        : expr(std::move(expr)), message(std::move(message)), StmtNode(std::move(position)) {}

    std::string AssertStmtNode::sExpression() const {
        return "(AssertStmtNode " + expr->sExpression() + " " + message->sExpression() + ")";
    }

    std::string IntTypeNode::sExpression() const { return "(IntTypeNode" + getTypeString() + ")"; }

    std::string FloatTypeNode::sExpression() const { return "(FloatTypeNode" + getTypeString() + ")"; }

    std::string BoolTypeNode::sExpression() const { return "(BoolTypeNode" + getTypeString() + ")"; }

    std::string StringTypeNode::sExpression() const { return "(StringTypeNode" + getTypeString() + ")"; }

    ArrayTypeNode::ArrayTypeNode(TypeNodePtr subtype) : subtype(std::move(subtype)), TypeNode(subtype->position) {}

    std::string ArrayTypeNode::sExpression() const {
        return "(ArrayTypeNode " + subtype->sExpression() + getTypeString() + ")";
    }

    TupleTypeNode::TupleTypeNode(const Lexer::FilePosition& position)
        : subtypes(std::make_shared<TypesNode>(position)), TypeNode(position) {}

    TupleTypeNode::TupleTypeNode(Lexer::FilePosition position, TypesNodePtr subtypes)
        : subtypes(std::move(subtypes)), TypeNode(std::move(position)) {}

    std::string TupleTypeNode::sExpression() const {
        return "(TupleTypeNode " + subtypes->sExpression() + getTypeString() + ")";
    }

    FunctionTypeNode::FunctionTypeNode(Lexer::FilePosition position, TypeNodePtr returnType)
        : params(std::make_shared<TypesNode>(position)), returnType(std::move(returnType)), TypeNode(position) {}

    FunctionTypeNode::FunctionTypeNode(Lexer::FilePosition position, TypesNodePtr params, TypeNodePtr returnType)
        : params(std::move(params)), returnType(std::move(returnType)), TypeNode(std::move(position)) {}

    std::string FunctionTypeNode::sExpression() const {
        return "(FunctionTypeNode " + params->sExpression() + " " + returnType->sExpression() + getTypeString() + ")";
    }

    VariableTypeNode::VariableTypeNode(Lexer::FilePosition position, std::string name)
        : name(std::move(name)), TypeNode(std::move(position)) {}

    std::string VariableTypeNode::sExpression() const { return "(VariableTypeNode " + name + getTypeString() + ")"; }

}  //  namespace Parser
