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

    ASTNodeType IntTypeNode::nodeType() const { return ASTNodeType::INT_TYPE; }

    ASTNodeType FloatTypeNode::nodeType() const { return ASTNodeType::FLOAT_TYPE; }

    ASTNodeType BoolTypeNode::nodeType() const { return ASTNodeType::BOOL_TYPE; }

    ASTNodeType StringTypeNode::nodeType() const { return ASTNodeType::STRING_TYPE; }

    ASTNodeType ArrayTypeNode::nodeType() const { return ASTNodeType::ARRAY_TYPE; }

    ASTNodeType TupleTypeNode::nodeType() const { return ASTNodeType::TUPLE_TYPE; }

    ASTNodeType FunctionTypeNode::nodeType() const { return ASTNodeType::FUNCTION_TYPE; }

    ASTNodeType VariableTypeNode::nodeType() const { return ASTNodeType::VARIABLE_TYPE; }

    std::string TypeNode::getTypeString() const { return TypeChecker::getTypeString(type); }

    TypesNode::TypesNode(TypeNodePtr type) : types({std::move(type)}) {}

    TypesNode::TypesNode(const std::shared_ptr<TypesNode>& types, TypeNodePtr type)
        : types(concat(types->types, std::move(type))) {}

    ASTNodeType TypesNode::nodeType() const { return ASTNodeType::TYPES; }

    std::string TypesNode::sExpression() const {
        std::stringstream result;
        result << "(TypesNode";
        for (const auto& type : types) { result << " " << type->sExpression(); }
        result << ")";
        return result.str();
    }

    BindingNode::BindingNode(std::string variable, bool constant, TypeNodePtr type)
        : variable(std::move(variable)), constant(constant), type(std::move(type)) {}

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

    BindingsNode::BindingsNode(BindingNodePtr binding) : bindings({std::move(binding)}) {}

    BindingsNode::BindingsNode(const std::shared_ptr<BindingsNode>& bindings, BindingNodePtr binding)
        : bindings(concat(bindings->bindings, std::move(binding))) {}

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

    ExprsNode::ExprsNode(ExprNodePtr expr) : exprs({std::move(expr)}) {}

    ExprsNode::ExprsNode(const std::shared_ptr<ExprsNode>& exprs, ExprNodePtr expr)
        : exprs(concat(exprs->exprs, std::move(expr))) {}

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

    StmtsNode::StmtsNode(StmtNodePtr stmt) : stmts({std::move(stmt)}) {}

    StmtsNode::StmtsNode(const std::shared_ptr<StmtsNode>& stmts, StmtNodePtr stmt)
        : stmts(concat(stmts->stmts, std::move(stmt))) {}

    ASTNodeType StmtsNode::nodeType() const { return ASTNodeType::STMTS; }

    std::string StmtsNode::sExpression() const {
        std::stringstream result;
        result << "(StmtsNode";
        for (const auto& stmt : stmts) { result << "\n\t" << stmt->sExpression(); }
        result << ")";
        return result.str();
    }

    StringExprsNode::StringExprsNode(std::string literal) {
        expressions.push_back(
                std::make_shared<StringExprNode>(std::make_shared<StringNode>(StringNode(std::move(literal)))));
    }

    StringExprsNode::StringExprsNode(std::string literal, ExprNodePtr expr,
                                     const std::shared_ptr<StringExprsNode>& exprs) {
        expressions.insert(expressions.end(), exprs->expressions.begin(), exprs->expressions.end());
        expressions.push_back(std::move(expr));
        expressions.push_back(
                std::make_shared<StringExprNode>(std::make_shared<StringNode>(StringNode(std::move(literal)))));
    }

    ASTNodeType StringExprsNode::nodeType() const { return ASTNodeType::STRING_EXPRS; }

    std::string StringExprsNode::sExpression() const {
        std::stringstream builder;
        builder << "(StringExprsNode";
        for (const ExprNodePtr& expr : expressions) { builder << " " << expr->sExpression(); }
        builder << ")";
        return builder.str();
    }

    StringNode::StringNode() : literal("") {}

    StringNode::StringNode(std::string literal) : literal(std::move(literal)) {}

    StringNode::StringNode(StringExprsNodePtr exprs) : expressions(std::move(exprs)) {}

    bool StringNode::isLiteral() const { return expressions == nullptr; }

    ASTNodeType StringNode::nodeType() const { return ASTNodeType::STRING; }

    std::string StringNode::sExpression() const {
        return "(StringNode " + (isLiteral() ? "'" + literal + "'" : expressions->sExpression()) + ")";
    }

    PrgmNode::PrgmNode() : stmts(std::make_shared<StmtsNode>()) {}

    PrgmNode::PrgmNode(StmtsNodePtr stmts) : stmts(std::move(stmts)) {}

    ASTNodeType PrgmNode::nodeType() const { return PRGM; }

    std::string PrgmNode::sExpression() const {
        std::stringstream builder;
        for (const StmtNodePtr& stmt : stmts->stmts) { builder << "\n\t" << stmt->sExpression(); }
        return "(PrgmNode" + builder.str() + ")";
    }

    //  =================
    //  ||  Commands:  ||
    //  =================

    BasicCmdNode::BasicCmdNode(ASTNodePtr argument) : arguments({std::move(argument)}) {}

    BasicCmdNode::BasicCmdNode(const BasicCmdNodePtr& first, const BasicCmdNodePtr& second) {
        arguments.insert(arguments.end(), first->arguments.begin(), first->arguments.end());
        arguments.insert(arguments.end(), second->arguments.begin(), second->arguments.end());
    }

    std::string BasicCmdNode::sExpression() const {
        std::stringstream builder;
        for (const ASTNodePtr& arg : arguments) { builder << " " << arg->sExpression(); }
        return "(BasicCmdNode" + builder.str() + ")";
    }

    AsyncCmdNode::AsyncCmdNode(CmdNodePtr cmd) : cmd(std::move(cmd)) {}

    std::string AsyncCmdNode::sExpression() const { return "(AsyncCmdNode " + cmd->sExpression() + ")"; }

    PipeCmdNode::PipeCmdNode(CmdNodePtr leftCmd, CmdNodePtr rightCmd)
        : leftCmd(std::move(leftCmd)), rightCmd(std::move(rightCmd)) {}

    std::string PipeCmdNode::sExpression() const {
        return "(PipeCmdNode " + leftCmd->sExpression() + " | " + rightCmd->sExpression() + ")";
    }

    //  ================
    //  ||  Lvalues:  ||
    //  ================

    VarLValueNode::VarLValueNode(std::string variable) : variable(std::move(variable)) {}

    std::string VarLValueNode::sExpression() const { return "(VarLValueNode " + variable + getTypeString() + ")"; }

    IndexLValueNode::IndexLValueNode(Parser::LValueNodePtr lvalue, Parser::ExprNodePtr index)
        : lvalue(std::move(lvalue)), index(std::move(index)) {}

    std::string IndexLValueNode::sExpression() const {
        return "(IndexLValueNode " + lvalue->sExpression() + " " + index->sExpression() + getTypeString() + ")";
    }

    //  ====================
    //  ||  Expressions:  ||
    //  ====================

    IntExprNode::IntExprNode(int64_t value) : value(value) {}

    std::string IntExprNode::sExpression() const {
        return "(IntExprNode " + std::to_string(value) + getTypeString() + ")";
    }

    FloatExprNode::FloatExprNode(double value) : value(value) {}

    std::string FloatExprNode::sExpression() const {
        return "(FloatExprNode " + std::to_string(value) + getTypeString() + ")";
    }

    StringExprNode::StringExprNode(StringNodePtr stringNode) : stringNode(std::move(stringNode)) {}

    std::string StringExprNode::sExpression() const {
        return "(StringExpressionNode " + stringNode->sExpression() + getTypeString() + ")";
    }

    BoolExprNode::BoolExprNode(bool value) : value(value) {}

    std::string BoolExprNode::sExpression() const {
        const std::string val = value ? "true" : "false";
        return "(BoolExprNode " + val + getTypeString() + ")";
    }

    VarExprNode::VarExprNode(std::string variable) : variable(std::move(variable)) {}

    VarExprNode::VarExprNode(const VarLValueNodePtr& lvalue) : variable(lvalue->variable) {}

    std::string VarExprNode::sExpression() const { return "(VarExprNode " + variable + getTypeString() + ")"; }

    LValueExprNode::LValueExprNode(const LValueNodePtr& lvalue) {
        if (lvalue->nodeType() == VAR_LVALUE) {
            expr = std::make_shared<VarExprNode>(std::static_pointer_cast<VarLValueNode>(lvalue));
        } else {
            expr = std::make_shared<IndexExprNode>(std::static_pointer_cast<IndexLValueNode>(lvalue));
        }
    }

    std::string LValueExprNode::sExpression() const {
        return "(LValueExprNode " + expr->sExpression() + getTypeString() + ")";
    }

    ArrayExprNode::ArrayExprNode() : expressions(std::make_shared<ExprsNode>()) {}

    ArrayExprNode::ArrayExprNode(ExprsNodePtr expressions) : expressions(std::move(expressions)) {}

    std::string ArrayExprNode::sExpression() const {
        return "(ArrayExprNode " + expressions->sExpression() + getTypeString() + ")";
    }

    IndexExprNode::IndexExprNode(ExprNodePtr expr, ExprNodePtr index)
        : expr(std::move(expr)), index(std::move(index)) {}

    IndexExprNode::IndexExprNode(IndexLValueNodePtr indexLValue) : index(indexLValue->index) {
        if (indexLValue->lvalue->nodeType() == VAR_LVALUE) {
            expr = std::make_shared<VarExprNode>(
                    std::static_pointer_cast<VarLValueNode>(indexLValue->lvalue)->variable);
        } else {
            expr = std::make_shared<IndexExprNode>(std::static_pointer_cast<IndexLValueNode>(indexLValue->lvalue));
        }
    }

    std::string IndexExprNode::sExpression() const {
        return "(IndexExprNode " + expr->sExpression() + " " + index->sExpression() + getTypeString() + ")";
    }

    TupleExprNode::TupleExprNode() : expressions(std::make_shared<ExprsNode>()) {}

    TupleExprNode::TupleExprNode(ExprsNodePtr expressions) : expressions(std::move(expressions)) {}

    std::string TupleExprNode::sExpression() const {
        return "(TupleExprNode " + expressions->sExpression() + getTypeString() + ")";
    }

    TernaryExprNode::TernaryExprNode(ExprNodePtr condition, ExprNodePtr trueExpr, ExprNodePtr falseExpr)
        : condition(std::move(condition)), trueExpr(std::move(trueExpr)), falseExpr(std::move(falseExpr)) {}

    std::string TernaryExprNode::sExpression() const {
        return "(TernaryExprNode " + condition->sExpression() + " " + trueExpr->sExpression() + " "
             + falseExpr->sExpression() + getTypeString() + ")";
    }

    UnOpExprNode::UnOpExprNode(UnOpType opType, ASTNodePtr node) : opType(opType), node(std::move(node)) {}

    std::string UnOpExprNode::sExpression() const {
        return "(UnOpExprNode " + node->sExpression() + " " + unOpToString(opType) + getTypeString() + ")";
    }

    BinOpExprNode::BinOpExprNode(ASTNodePtr left, BinOpType opType, ExprNodePtr right)
        : opType(opType), left(std::move(left)), right(std::move(right)) {}

    std::string BinOpExprNode::sExpression() const {
        return "(BinOpExprNode " + left->sExpression() + " " + binOpToString(opType) + " " + right->sExpression()
             + getTypeString() + ")";
    }

    CallExprNode::CallExprNode(ExprNodePtr func) : func(std::move(func)), args(std::make_shared<ExprsNode>()) {}

    CallExprNode::CallExprNode(ExprNodePtr func, ExprsNodePtr args) : func(std::move(func)), args(std::move(args)) {}

    std::string CallExprNode::sExpression() const {
        return "(CallExprNode " + func->sExpression() + " " + args->sExpression() + getTypeString() + ")";
    }

    ApiCallExprNode::ApiCallExprNode(ExprNodePtr expression, std::string func)
        : expression(std::move(expression)), func(std::move(func)), args(std::make_shared<ExprsNode>()) {}

    ApiCallExprNode::ApiCallExprNode(ExprNodePtr expression, std::string func, ExprsNodePtr args)
        : expression(std::move(expression)), func(std::move(func)), args(std::move(args)) {}

    std::string ApiCallExprNode::sExpression() const {
        return "(ApiCallExprNode " + expression->sExpression() + " " + func + " " + args->sExpression()
             + getTypeString() + ")";
    }

    LambdaExprNode::LambdaExprNode(ExprNodePtr body, TypeNodePtr returnType)
        : bindings(std::make_shared<BindingsNode>()), body(std::make_shared<ReturnStmtNode>(std::move(body))),
          returnType(std::move(returnType)) {}

    LambdaExprNode::LambdaExprNode(StmtNodePtr body, TypeNodePtr returnType)
        : bindings(std::make_shared<BindingsNode>()), body(std::move(body)), returnType(std::move(returnType)) {}

    LambdaExprNode::LambdaExprNode(BindingsNodePtr bindings, ExprNodePtr body, TypeNodePtr returnType)
        : bindings(std::move(bindings)), body(std::make_shared<ReturnStmtNode>(std::move(body))),
          returnType(std::move(returnType)) {}

    LambdaExprNode::LambdaExprNode(BindingsNodePtr bindings, StmtNodePtr body, TypeNodePtr returnType)
        : bindings(std::move(bindings)), body(std::move(body)), returnType(std::move(returnType)) {}

    std::string LambdaExprNode::sExpression() const {
        return "(LambdaExprNode " + bindings->sExpression() + (returnType ? " " + returnType->sExpression() : "") + " "
             + body->sExpression() + getTypeString() + ")";
    }

    CmdExprNode::CmdExprNode(CmdNodePtr cmd) : cmd(std::move(cmd)) {}

    std::string CmdExprNode::sExpression() const {
        return "(CmdExprNode " + cmd->sExpression() + getTypeString() + ")";
    }

    ScanExprNode::ScanExprNode(ExprNodePtr prompt) : prompt(std::move(prompt)) {}

    std::string ScanExprNode::sExpression() const {
        return "(ScanExprNode " + prompt->sExpression() + getTypeString() + ")";
    }

    ReadExprNode::ReadExprNode(ExprNodePtr filePath) : filePath(std::move(filePath)) {}

    std::string ReadExprNode::sExpression() const {
        return "(ReadExprNode " + filePath->sExpression() + getTypeString() + ")";
    }


    //  ===================
    //  ||  Statements:  ||
    //  ===================

    IfStmtNode::IfStmtNode(ExprNodePtr condition, StmtNodePtr trueStmt, StmtNodePtr falseStmt)
        : condition(std::move(condition)), trueStmt(std::move(trueStmt)), falseStmt(std::move(falseStmt)) {}

    std::string IfStmtNode::sExpression() const {
        return "(IfStmtNode " + condition->sExpression() + " " + trueStmt->sExpression()
             + (falseStmt ? " " + falseStmt->sExpression() : "") + ")";
    }

    ForStmtNode::ForStmtNode(StmtNodePtr initial, ExprNodePtr condition, StmtNodePtr update, StmtNodePtr body)
        : initial(std::move(initial)), condition(std::move(condition)), update(std::move(update)),
          body(std::move(body)) {}

    std::string ForStmtNode::sExpression() const {
        return "(ForStmtNode " + initial->sExpression() + " " + condition->sExpression() + " " + update->sExpression()
             + " " + body->sExpression() + ")";
    }

    WhileStmtNode::WhileStmtNode(ExprNodePtr condition, StmtNodePtr body)
        : condition(std::move(condition)), body(std::move(body)) {}

    std::string WhileStmtNode::sExpression() const {
        return "(WhileStmtNode " + condition->sExpression() + " " + body->sExpression() + ")";
    }

    DoWhileStmtNode::DoWhileStmtNode(ExprNodePtr condition, StmtNodePtr body)
        : condition(std::move(condition)), body(std::move(body)) {}

    std::string DoWhileStmtNode::sExpression() const {
        return "(DoWhileStmtNode " + condition->sExpression() + " " + body->sExpression() + ")";
    }

    ReturnStmtNode::ReturnStmtNode(ExprNodePtr retExpr) : retExpr(std::move(retExpr)) {}

    std::string ReturnStmtNode::sExpression() const { return "(ReturnStmtNode " + retExpr->sExpression() + ")"; }

    std::string BreakStmtNode::sExpression() const { return "(BreakStmtNode)"; }

    std::string ContinueStmtNode::sExpression() const { return "(ContinueStmtNode)"; }

    ScopeStmtNode::ScopeStmtNode() : stmts(std::make_shared<StmtsNode>()) {}

    ScopeStmtNode::ScopeStmtNode(StmtsNodePtr stmts) : stmts(std::move(stmts)) {}

    std::string ScopeStmtNode::sExpression() const { return "(ScopeStmtNode " + stmts->sExpression() + ")"; }

    CmdStmtNode::CmdStmtNode(CmdNodePtr command) : command(std::move(command)) {}

    std::string CmdStmtNode::sExpression() const { return "(CmdStmtNode " + command->sExpression() + ")"; }

    ExprStmtNode::ExprStmtNode(ExprNodePtr expression) : expression(std::move(expression)) {}

    std::string ExprStmtNode::sExpression() const { return "(ExprStmtNode " + expression->sExpression() + ")"; }

    AliasStmtNode::AliasStmtNode(std::string alias, CmdNodePtr command)
        : alias(std::move(alias)), command(std::move(command)) {}

    std::string AliasStmtNode::sExpression() const {
        return "(AliasStmtNode " + alias + " " + command->sExpression() + ")";
    }

    ImportStmtNode::ImportStmtNode(StringNodePtr filePath) : filePath(std::move(filePath)) {}

    std::string ImportStmtNode::sExpression() const {
        std::stringstream builder;
        for (ASTNodePtr node : prgm) { builder << " " + node->sExpression(); }
        return "(ImportStmtNode " + filePath->sExpression() + builder.str() + ")";
    }

    PrintStmtNode::PrintStmtNode(ExprNodePtr expression) : expression(std::move(expression)) {}

    std::string PrintStmtNode::sExpression() const { return "(PrintStmtNode " + expression->sExpression() + ")"; }

    PrintlnStmtNode::PrintlnStmtNode(ExprNodePtr expression) : expression(std::move(expression)) {}

    std::string PrintlnStmtNode::sExpression() const { return "(PrintlnStmtNode " + expression->sExpression() + ")"; }

    WriteStmtNode::WriteStmtNode(ExprNodePtr fileData, ExprNodePtr filePath)
        : fileData(std::move(fileData)), filePath(std::move(filePath)) {}

    std::string WriteStmtNode::sExpression() const {
        return "(WriteStmtNode " + fileData->sExpression() + " " + filePath->sExpression() + ")";
    }

    TypeStmtNode::TypeStmtNode(std::string alias, TypeNodePtr type) : alias(std::move(alias)), type(std::move(type)) {}

    std::string TypeStmtNode::sExpression() const { return "(TypeStmtNode " + alias + " " + type->sExpression() + ")"; }

    FunctionStmtNode::FunctionStmtNode(std::string name, StmtNodePtr body, TypeNodePtr returnType)
        : name(std::move(name)), bindings(std::make_shared<BindingsNode>()), body(std::move(body)),
          returnType(std::move(returnType)) {}

    FunctionStmtNode::FunctionStmtNode(std::string name, BindingsNodePtr bindings, StmtNodePtr body,
                                       TypeNodePtr returnType)
        : name(std::move(name)), bindings(std::move(bindings)), body(std::move(body)),
          returnType(std::move(returnType)) {}

    std::string FunctionStmtNode::sExpression() const {
        return "(FunctionStmtNode " + name + " " + bindings->sExpression() + " " + returnType->sExpression() + " "
             + body->sExpression() + ")";
    }

    TimeoutStmtNode::TimeoutStmtNode(const int64_t& timeout, StringNodePtr message, Parser::StmtNodePtr stmt)
        : timeout(timeout), message(std::move(message)), stmt(std::move(stmt)) {}

    std::string TimeoutStmtNode::sExpression() const {
        return "(TimeoutStmtNode " + std::to_string(timeout) + " " + message->sExpression() + " " + stmt->sExpression()
             + ")";
    }

    AssertStmtNode::AssertStmtNode(ExprNodePtr expr, StringNodePtr message)
        : expr(std::move(expr)), message(std::move(message)) {}

    std::string AssertStmtNode::sExpression() const {
        return "(AssertStmtNode " + expr->sExpression() + " " + message->sExpression() + ")";
    }

    std::string IntTypeNode::sExpression() const { return "(IntTypeNode" + getTypeString() + ")"; }

    std::string FloatTypeNode::sExpression() const { return "(FloatTypeNode" + getTypeString() + ")"; }

    std::string BoolTypeNode::sExpression() const { return "(BoolTypeNode" + getTypeString() + ")"; }

    std::string StringTypeNode::sExpression() const { return "(StringTypeNode" + getTypeString() + ")"; }

    ArrayTypeNode::ArrayTypeNode(TypeNodePtr subtype) : subtype(std::move(subtype)) {}

    std::string ArrayTypeNode::sExpression() const {
        return "(ArrayTypeNode " + subtype->sExpression() + getTypeString() + ")";
    }

    TupleTypeNode::TupleTypeNode() : subtypes(std::make_shared<TypesNode>()) {}

    TupleTypeNode::TupleTypeNode(TypesNodePtr subtypes) : subtypes(std::move(subtypes)) {}

    std::string TupleTypeNode::sExpression() const {
        return "(TupleTypeNode " + subtypes->sExpression() + getTypeString() + ")";
    }

    FunctionTypeNode::FunctionTypeNode(TypeNodePtr returnType)
        : params(std::make_shared<TypesNode>()), returnType(std::move(returnType)) {}

    FunctionTypeNode::FunctionTypeNode(TypesNodePtr params, TypeNodePtr returnType)
        : params(std::move(params)), returnType(std::move(returnType)) {}

    std::string FunctionTypeNode::sExpression() const {
        return "(FunctionTypeNode " + params->sExpression() + " " + returnType->sExpression() + getTypeString() + ")";
    }

    VariableTypeNode::VariableTypeNode(std::string name) : name(std::move(name)) {}

    std::string VariableTypeNode::sExpression() const { return "(VariableTypeNode " + name + getTypeString() + ")"; }

}  //  namespace Parser
