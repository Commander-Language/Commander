#include "transpiler.hpp"
#include "source/parser/parser.hpp"
#include "source/type_checker/type.hpp"
#include "source/util/commander_exception.hpp"

#include <utility>
namespace PowerShellTranspiler {
    PowerShellTranspiler::PowerShellTranspiler(Parser::ASTNodeList nodes) : _ast(std::move(nodes)) {}
    std::string PowerShellTranspiler::transpile() {
        for (auto& node : _ast) { _node(node); }
        return _output;
    }
    void PowerShellTranspiler::_node(const Parser::ASTNodePtr& node) {
        switch (Parser::getAbstractNodeType(node->nodeType())) {
            case Parser::BINDING: {
                _binding(std::static_pointer_cast<Parser::BindingNode>(node));
                break;
            }
            case Parser::BINDINGS: {
                _bindings(std::static_pointer_cast<Parser::BindingsNode>(node));
                break;
            }
            case Parser::EXPRS: {
                _exprs(std::static_pointer_cast<Parser::ExprsNode>(node));
                break;
            }
            case Parser::PRGM: {
                _prgm(std::static_pointer_cast<Parser::PrgmNode>(node));
                break;
            }
            case Parser::STMTS: {
                _stmts(std::static_pointer_cast<Parser::StmtsNode>(node));
                break;
            }
            case Parser::STRING: {
                _string(std::static_pointer_cast<Parser::StringNode>(node));
                break;
            }
            case Parser::STRING_EXPRS: {
                _string_exprs(std::static_pointer_cast<Parser::StringExprsNode>(node));
                break;
            }
            case Parser::TYPES: {
                _types(std::static_pointer_cast<Parser::TypesNode>(node));
                break;
            }
            case Parser::ASYNC_CMD:
            case Parser::BASIC_CMD:
            case Parser::CMD:
            case Parser::PIPE_CMD: {
                _cmd(std::static_pointer_cast<Parser::CmdNode>(node));
                break;
            }
            case Parser::LVALUE: {
                _lvalue(std::static_pointer_cast<Parser::LValueNode>(node));
                break;
            }
            case Parser::EXPR: {
                _expr(std::static_pointer_cast<Parser::ExprNode>(node));
                break;
            }
            case Parser::STMT: {
                _stmt(std::static_pointer_cast<Parser::StmtNode>(node));
                break;
            }
            case Parser::TYPE: {
                _type(std::static_pointer_cast<Parser::TypeNode>(node));
                break;
            }
            default: {
                throw Util::CommanderException("Unknown node type found while transpiling "
                                               + Parser::nodeTypeToString(node->nodeType()));
            }
        }
    }

    void PowerShellTranspiler::_binding(const Parser::BindingNodePtr node) { _write("$" + node->variable + " "); }
    void PowerShellTranspiler::_bindings(const Parser::BindingsNodePtr node) {
        for (auto& binding : node->bindings) { _binding(binding); }
    }
    void PowerShellTranspiler::_exprs(const Parser::ExprsNodePtr node) {
        for (auto& expr : node->exprs) { _expr(expr); }
    }
    void PowerShellTranspiler::_prgm(const Parser::PrgmNodePtr node) { _stmts(node->stmts); }
    void PowerShellTranspiler::_stmts(const Parser::StmtsNodePtr node) {
        for (auto& stmt : node->stmts) { _stmt(stmt); }
    }
    void PowerShellTranspiler::_string(const Parser::StringNodePtr node) {
        if (node->isLiteral()) {
            _write("\"" + node->literal + "\"");
        } else {
            _string_exprs(node->expressions);
        }
    }
    void PowerShellTranspiler::_string_exprs(const Parser::StringExprsNodePtr node) {
        for (auto& expr : node->expressions) _expr(expr);
    }
    void PowerShellTranspiler::_types(const Parser::TypesNodePtr node) {
        for (auto& type : node->types) { _type(type); }
    }
    void PowerShellTranspiler::_cmd(const Parser::CmdNodePtr node) {
        switch (node->nodeType()) {
            case Parser::BASIC_CMD: {
                auto basicCommand = std::static_pointer_cast<Parser::BasicCmdNode>(node);
                for (auto& arg : basicCommand->arguments) { _node(arg); }
                break;
            }
            case Parser::ASYNC_CMD: {
                _write("Start-Job -ScriptBlock { ");
                auto asyncCommand = std::static_pointer_cast<Parser::AsyncCmdNode>(node);
                auto cmd = std::static_pointer_cast<Parser::BasicCmdNode>(asyncCommand->cmd);
                for (auto& arg : cmd->arguments) { _node(arg); }
                _write(" }");
                break;
            }
            case Parser::PIPE_CMD: {
                auto pipeCommand = std::static_pointer_cast<Parser::PipeCmdNode>(node);
                auto left = std::static_pointer_cast<Parser::BasicCmdNode>(pipeCommand->leftCmd);
                for (auto& arg : left->arguments) { _node(arg); }
                _write(" | ");
                _cmd(pipeCommand->rightCmd);
                break;
            }
            default: {
                // TODO: Error
            }
        }
    }
    void PowerShellTranspiler::_lvalue(const Parser::LValueNodePtr node) {
        switch (node->nodeType()) {
                //            case Parser::VAR_LVALUE: {
                //                auto var = std::static_pointer_cast<Parser::VarLValueNode>(node);
                //                _write("$" + var->variable);
                //                break;
                //            }
                //            case Parser::INDEX_LVALUE: {
                //                auto index = std::static_pointer_cast<Parser::IndexLValueNode>(node);
                //                break;
                //            }
                //            case Parser::LVALUE: {
                //                break;
                //            }
            default: {
                break;
            }
        }
    }
    void PowerShellTranspiler::_expr(const Parser::ExprNodePtr node) {
        switch (node->nodeType()) {
            case Parser::INT_EXPR: {
                auto expr = std::static_pointer_cast<Parser::IntExprNode>(node);
                _write(std::to_string(expr->value));
                break;
            }
            case Parser::FLOAT_EXPR: {
                auto expr = std::static_pointer_cast<Parser::FloatExprNode>(node);
                _write(std::to_string(expr->value));
                break;
            }
            case Parser::STRING_EXPR: {
                auto expr = std::static_pointer_cast<Parser::StringExprNode>(node);
                _node(expr->stringNode);
                break;
            }
            case Parser::BOOL_EXPR: {
                auto expr = std::static_pointer_cast<Parser::BoolExprNode>(node);
                if (expr->value) _write("1");
                else
                    _write("0");
                break;
            }
            case Parser::VAR_EXPR: {
                auto expr = std::static_pointer_cast<Parser::VarExprNode>(node);
                _write("$" + expr->variable);
                break;
            }
            case Parser::LVALUE_EXPR: {
                auto expr = std::static_pointer_cast<Parser::LValueExprNode>(node);
                _expr(expr->expr);
                break;
            }
            case Parser::ARRAY_EXPR: {
                auto expr = std::static_pointer_cast<Parser::ArrayExprNode>(node);
                auto exprs = std::static_pointer_cast<Parser::ExprsNode>(expr->expressions);
                auto array = exprs->exprs;
                if (array.size() == 0) _write("@()");
                else if (array.size() == 1) {
                    _write(",");
                    _expr(array[0]);
                } else {
                    for (int i = 0; i < array.size(); i++) {
                        _expr(array[i]);
                        if (i != array.size() - 1) _write(",");
                    }
                }
                break;
            }
            case Parser::INDEX_EXPR: {
                // TODO: Update when tuples are actually implemented
                auto expr = std::static_pointer_cast<Parser::IndexExprNode>(node);
                _expr(expr->expr);
                _write("[");
                _expr(expr->index);
                _write("]");
                break;
            }
            case Parser::TUPLE_EXPR: {
                // TODO: Use actual powershell tuple
                // This impelmentation just set it as an array
                auto expr = std::static_pointer_cast<Parser::TupleExprNode>(node);
                auto exprs = std::static_pointer_cast<Parser::ExprsNode>(expr->expressions);
                auto array = exprs->exprs;
                if (array.size() == 0) _write("@()");
                else if (array.size() == 1) {
                    _write(",");
                    _expr(array[0]);
                } else {
                    for (int i = 0; i < array.size(); i++) {
                        _expr(array[i]);
                        if (i != array.size() - 1) _write(",");
                    }
                }
                break;
            }
            case Parser::TERNARY_EXPR: {
                // Only for powershell v7.0+
                auto expr = std::static_pointer_cast<Parser::TernaryExprNode>(node);
                _write("( ");
                _expr(expr->condition);
                _write(") ?");
                _expr(expr->trueExpr);
                _write(" : ");
                _expr(expr->falseExpr);
                break;
            }
            case Parser::UNOP_EXPR: {
                auto unaryOperation = std::static_pointer_cast<Parser::UnOpExprNode>(node);
                break;
            }
            case Parser::BINOP_EXPR: {
                auto binaryOperation = std::static_pointer_cast<Parser::BinOpExprNode>(node);
                break;
            }
            case Parser::LAMBDA_EXPR: {
                auto expr = std::static_pointer_cast<Parser::LambdaExprNode>(node);
                _write("{ ");
                auto bindings = expr->bindings->bindings;
                if (bindings.size() > 0) {
                    _write("Param(");
                    for (int i = 0; i < bindings.size(); i++) {
                        if (i > 0) { _write("[,"); }
                        auto binding = bindings[i];
                        if (binding->type) {
                            _write("[");
                            _type(binding->type);
                            _write("]");
                        }
                        _write("$" + binding->variable);
                        if (i > 0) { _write("]"); }
                    }
                    _write(")");
                }
                _stmt(expr->body);
                _write("}");
                break;
            }
            case Parser::CMD_EXPR: {
                auto expr = std::static_pointer_cast<Parser::CmdExprNode>(node);
                _cmd(expr->cmd);
                break;
            }
            case Parser::CALL_EXPR: {
                auto expr = std::static_pointer_cast<Parser::CallExprNode>(node);
                // TODO: Implement
                break;
            }
            case Parser::API_CALL_EXPR: {
                auto expr = std::static_pointer_cast<Parser::ApiCallExprNode>(node);
                // TODO: Implement
                break;
            }
            case Parser::SCAN_EXPR: {
                auto expr = std::static_pointer_cast<Parser::ScanExprNode>(node);
                // TODO: Implement
                break;
            }
            case Parser::READ_EXPR: {
                auto expr = std::static_pointer_cast<Parser::ReadExprNode>(node);
                // TODO: Implement
                break;
            }
            default: {
                break;
            }
        }
    }
    void PowerShellTranspiler::_stmt(const Parser::StmtNodePtr node) {
        switch (node->nodeType()) {
            case Parser::IF_STMT: {
                auto stmt = std::static_pointer_cast<Parser::IfStmtNode>(node);
                _write("if(");
                _expr(stmt->condition);
                _writeLine(") {");
                _increaseIndent();

                _stmt(stmt->trueStmt);
                if (stmt->falseStmt) {
                    _decreaseIndent();
                    _writeLine("} else {");

                    _increaseIndent();
                    _stmt(stmt->falseStmt);
                    _decreaseIndent();
                    _writeLine("}");
                } else {
                    _decreaseIndent();
                    _writeLine("}");
                }
                break;
            }
            case Parser::FOR_STMT: {
                auto stmt = std::static_pointer_cast<Parser::ForStmtNode>(node);
                _write("for(");
                _expr(stmt->initial);
                _write(";");
                _expr(stmt->condition);
                _write(";");
                _expr(stmt->update);
                _writeLine(") {");
                _increaseIndent();
                _stmt(stmt->body);
                _decreaseIndent();
                _writeLine("}");
                break;
            }
            case Parser::WHILE_STMT: {
                auto stmt = std::static_pointer_cast<Parser::WhileStmtNode>(node);
                _write("while(");
                _expr(stmt->condition);
                _writeLine("){");
                _increaseIndent();
                _stmt(stmt->body);
                _decreaseIndent();
                _writeLine("}");
                break;
            }
            case Parser::DO_WHILE_STMT: {
                auto stmt = std::static_pointer_cast<Parser::DoWhileStmtNode>(node);
                _writeLine("do{");
                _increaseIndent();
                _stmt(stmt->body);
                _decreaseIndent();
                _write("} while(");
                _expr(stmt->condition);
                _writeLine(")");
                break;
            }
            case Parser::RETURN_STMT: {
                auto stmt = std::static_pointer_cast<Parser::ReturnStmtNode>(node);
                _write("return ");
                _expr(stmt->retExpr);
                _writeLine("");
                break;
            }
            case Parser::SCOPE_STMT: {
                // TODO: implement user-defined scopes
                auto stmt = std::static_pointer_cast<Parser::ScopeStmtNode>(node);
                _writeLine("{");
                _increaseIndent();
                _stmts(stmt->stmts);
                _decreaseIndent();
                _writeLine("}");
                break;
            }
            case Parser::CMD_STMT: {
                auto stmt = std::static_pointer_cast<Parser::CmdStmtNode>(node);
                _cmd(stmt->command);
                _writeLine("");
                break;
            }
            case Parser::EXPR_STMT: {
                auto stmt = std::static_pointer_cast<Parser::ExprStmtNode>(node);
                _expr(stmt->expression);
                _writeLine("");
                break;
            }
            case Parser::ALIAS_STMT: {
                auto stmt = std::static_pointer_cast<Parser::AliasStmtNode>(node);
                _write("New-Alias -Name " + stmt->alias + "-Value ");
                _cmd(stmt->command);
                _writeLine("");
                break;
            }
            case Parser::IMPORT_STMT: {
                auto stmt = std::static_pointer_cast<Parser::ImportStmtNode>(node);
                // TODO: Implement
                break;
            }
            case Parser::PRINT_STMT: {
                auto stmt = std::static_pointer_cast<Parser::PrintStmtNode>(node);
                _write("Write-Output ");
                _expr(stmt->expression);
                _writeLine("");
                break;
            }
            case Parser::PRINTLN_STMT: {
                auto stmt = std::static_pointer_cast<Parser::PrintlnStmtNode>(node);
                _write("Write-Output ");
                _expr(stmt->expression);
                _writeLine("");
                break;
            }
            case Parser::WRITE_STMT: {
                auto stmt = std::static_pointer_cast<Parser::WriteStmtNode>(node);
                _write("Write-Output ");
                _expr(stmt->fileData);
                _write(" | Out-File -FilePath ");
                _expr(stmt->filePath);
                _writeLine("");

                break;
            }
            case Parser::TYPE_STMT: {
                auto stmt = std::static_pointer_cast<Parser::TypeStmtNode>(node);
                // TODO: Implement
                break;
            }
            case Parser::BREAK_STMT: {
                auto stmt = std::static_pointer_cast<Parser::BreakStmtNode>(node);
                _writeLine("break");
                break;
            }
            case Parser::CONTINUE_STMT: {
                auto stmt = std::static_pointer_cast<Parser::ContinueStmtNode>(node);
                _writeLine("continue");
                break;
            }
            case Parser::TIMEOUT_STMT: {
                auto stmt = std::static_pointer_cast<Parser::TimeoutStmtNode>(node);
                std::string var1 = "$timeout" + std::to_string(_timeoutCount);
                std::string var2 = "$wait" + std::to_string(_timeoutCount);
                _writeLine(var1 + " = {");
                _stmt(stmt->stmt);
                _writeLine("}");
                _writeLine(var2 + " = Start-Job -ScriptBlock " + var1);
                _write("if(!(Wait-Job " + var2 + "-Timeout " + std::to_string(stmt->timeout / 1000) + ")){");
                _write("Write-Output ");
                _string(stmt->message);
                _writeLine("}");
                _timeoutCount++;
                break;
            }
            case Parser::ASSERT_STMT: {
                auto stmt = std::static_pointer_cast<Parser::AssertStmtNode>(node);
                _write("if(!(");
                _expr(stmt->expr);
                _writeLine(")) {");
                _write("Write-Output ");
                _string(stmt->message);
                _writeLine("}");

                break;
            }
            case Parser::FUNCTION_STMT: {
                auto stmt = std::static_pointer_cast<Parser::FunctionStmtNode>(node);
                _writeLine("function " + stmt->name + "{");
                _increaseIndent();
                auto bindings = stmt->bindings->bindings;
                if (bindings.size() > 0) {
                    _write("param(");
                    for (int i = 0; i < bindings.size(); i++) {
                        auto binding = bindings[i];
                        if (binding->type) {
                            _write("[");
                            _type(binding->type);
                            _write("]");
                        }
                        _write("$" + binding->variable);
                        if (i < bindings.size() - 1) { _write(", "); }
                    }
                    _writeLine(")");
                }
                _stmt(stmt->body);
                _decreaseIndent();
                _write("}");
                break;
            }
            default: {
            }
        }
    }
    void PowerShellTranspiler::_type(const Parser::TypeNodePtr node) {
        switch (node->type->getType()) {
            case TypeChecker::INT: {
                _write("int");
                break;
            }
            case TypeChecker::FLOAT: {
                _write("double");
                break;
            }
            case TypeChecker::BOOL: {
                _write("bool");
                break;
            }
            case TypeChecker::TUPLE: {
                _write("Array");  // TODO: Fix when tuples are implemented
                break;
            }
            case TypeChecker::ARRAY: {
                _write("Array");
                break;
            }
            case TypeChecker::FUNCTION: {
                _write("scriptblock");
                break;
            }
            case TypeChecker::STRING: {
                _write("string");
                break;
            }
            case TypeChecker::COMMAND: {
                _write("cmdlet");  // This is probably wrong
                break;
            }
            default: {
            }
        }
    }

    void PowerShellTranspiler::_increaseIndent() { _indentLevel++; }
    void PowerShellTranspiler::_decreaseIndent() {
        _indentLevel--;
        if (_indentLevel < 0) { _indentLevel = 0; }
    }
    void PowerShellTranspiler::_writeLine(const std::string& str) {
        _output.append(std::string(_indentLevel * _indentSize, ' '));
        _output.append(str);
        _output.append("\n");
    }
    void PowerShellTranspiler::_write(const std::string& str) {
        _output.append(std::string(_indentLevel * _indentSize, ' '));
        _output.append(str);
    }
}  // namespace PowerShellTranspiler
