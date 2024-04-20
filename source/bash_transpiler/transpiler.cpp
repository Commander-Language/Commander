/**
 * @file transpiler.cpp
 * @brief Implementation for transpiler.hpp
 */

#include "transpiler.hpp"

namespace BashTranspiler {

    BashTranspiler::BashTranspiler(const TypeChecker::VariableTablePtr& symbolTable) { _table = symbolTable; }

    std::string BashTranspiler::transpile(const Parser::ASTNodeList& nodes) {
        _buffer << "#!/bin/bash";
        _writeLine();
        _writeLine();
        _scopes.push(std::map<std::string, std::string> {});
        for (const Parser::ASTNodePtr& node : nodes) { _transpile(node); }
        _writeLine();
        for (const std::string& alias : _aliases) { _lines.push_back("unalias " + alias); }
        _aliases.clear();
        for (const std::string& line : _lines) { _buffer << line << '\n'; }
        _lines.clear();
        std::string output = _buffer.str();
        _buffer.str("");
        _scopes.pop();
        return output;
    }

    void BashTranspiler::_transpile(const Parser::ASTNodePtr& astNode) {
        switch (astNode->nodeType()) {
            case Parser::BASIC_CMD: {
                for (const Parser::ASTNodePtr& astNodePtr :
                     std::static_pointer_cast<Parser::BasicCmdNode>(astNode)->arguments) {
                    _transpile(astNodePtr);
                    _buffer << " ";
                }
                return;
            }
            case Parser::PIPE_CMD: {
                Parser::PipeCmdNodePtr pipeCmdPtr = std::static_pointer_cast<Parser::PipeCmdNode>(astNode);
                _transpile(pipeCmdPtr->leftCmd);
                _buffer << "| ";
                _transpile(pipeCmdPtr->rightCmd);
                return;
            }
            case Parser::ASYNC_CMD: {
                _transpile(std::static_pointer_cast<Parser::AsyncCmdNode>(astNode)->cmd);
                return;
            }
            case Parser::INT_EXPR: {
                _buffer << "$((" << std::static_pointer_cast<Parser::IntExprNode>(astNode)->value << "))";
                return;
            }
            case Parser::FLOAT_EXPR: {
                _buffer << "$(echo " << std::static_pointer_cast<Parser::FloatExprNode>(astNode)->value << ")";
                return;
            }
            case Parser::STRING_EXPR: {
                _buffer << "$(echo \"";
                _transpile(std::static_pointer_cast<Parser::StringExprNode>(astNode)->stringNode);
                _buffer << "\")";
                return;
            }
            case Parser::BOOL_EXPR: {
                _buffer << "$((" << (std::static_pointer_cast<Parser::BoolExprNode>(astNode)->value ? 1 : 0) << "))";
                return;
            }
            case Parser::VAR_LVALUE:
            case Parser::VAR_EXPR: {
                std::string varName = astNode->nodeType() == Parser::VAR_LVALUE
                                            ? std::static_pointer_cast<Parser::VarLValueNode>(astNode)->variable
                                            : std::static_pointer_cast<Parser::VarExprNode>(astNode)->variable;
                if (_scopes.top().find(varName) != _scopes.top().end()) { varName = _scopes.top()[varName]; }
                _buffer << "$";
                _buffer << varName;
                return;
            }
            case Parser::LVALUE_EXPR: {
                _transpile(std::static_pointer_cast<Parser::LValueExprNode>(astNode)->expr);
                return;
            }
            case Parser::ARRAY_EXPR: {
                // TODO: Implement
                return;
            }
            case Parser::INDEX_LVALUE:
            case Parser::INDEX_EXPR: {
                // TODO: Implement
                return;
            }
            case Parser::TUPLE_EXPR: {
                // TODO: Implement
                return;
            }
            case Parser::TERNARY_EXPR: {
                const Parser::TernaryExprNodePtr ternaryExpr = std::static_pointer_cast<Parser::TernaryExprNode>(
                        astNode);
                _buffer << "$(if [ \"";
                _transpile(ternaryExpr->condition);
                _buffer << "\" -eq 1 ]; then echo ";
                _transpile(ternaryExpr->trueExpr);
                _buffer << "; else echo ";
                _transpile(ternaryExpr->falseExpr);
                _buffer << "; fi)";
                return;
            }
            case Parser::UNOP_EXPR: {
                Parser::UnOpExprNodePtr unOpExpr = std::static_pointer_cast<Parser::UnOpExprNode>(astNode);
                switch (unOpExpr->opType) {
                    case Parser::NEGATE:
                        _buffer << "$(echo \"-";
                        _transpile(unOpExpr->node);
                        _buffer << "\")";
                        break;
                    case Parser::NOT:
                        _buffer << "$(( ! ";
                        _transpile(unOpExpr->node);
                        _buffer << " ))";
                        break;
                    case Parser::PRE_INCREMENT: {
                        Parser::BinOpExprNodePtr newBinop = std::make_shared<Parser::BinOpExprNode>(
                                unOpExpr->node, Parser::ADD_SET,
                                std::make_shared<Parser::IntExprNode>(unOpExpr->position, 1));
                        newBinop->type = unOpExpr->type;
                        _transpile(newBinop);
                        break;
                    }
                    case Parser::POST_INCREMENT: {
                        _buffer << "$(echo \"";
                        Parser::BinOpExprNodePtr newBinop = std::make_shared<Parser::BinOpExprNode>(
                                unOpExpr->node, Parser::ADD_SET,
                                std::make_shared<Parser::IntExprNode>(unOpExpr->position, 1));
                        newBinop->type = unOpExpr->type;
                        _transpile(newBinop);
                        _buffer << " - 1\" | bc)";
                        break;
                    }
                    case Parser::PRE_DECREMENT: {
                        Parser::BinOpExprNodePtr newBinop = std::make_shared<Parser::BinOpExprNode>(
                                unOpExpr->node, Parser::SUBTRACT_SET,
                                std::make_shared<Parser::IntExprNode>(unOpExpr->position, 1));
                        newBinop->type = unOpExpr->type;
                        _transpile(newBinop);
                        break;
                    }
                    case Parser::POST_DECREMENT: {
                        _buffer << "$(echo \"";
                        Parser::BinOpExprNodePtr newBinop = std::make_shared<Parser::BinOpExprNode>(
                                unOpExpr->node, Parser::SUBTRACT_SET,
                                std::make_shared<Parser::IntExprNode>(unOpExpr->position, 1));
                        newBinop->type = unOpExpr->type;
                        _transpile(newBinop);
                        _buffer << " + 1\" | bc)";
                        break;
                    }
                }
                return;
            }
            case Parser::BINOP_EXPR: {
                Parser::BinOpExprNodePtr binOpExpr = std::static_pointer_cast<Parser::BinOpExprNode>(astNode);
                bool isFloat = binOpExpr->type && binOpExpr->type->getType() == TypeChecker::FLOAT;
                switch (binOpExpr->opType) {
                    case Parser::LESSER:
                        // TODO: Handle strings
                        _createBCBinopExpression(binOpExpr, "<", false);
                        break;
                    case Parser::GREATER:
                        // TODO: Handle strings
                        _createBCBinopExpression(binOpExpr, ">", false);
                        break;
                    case Parser::LESSER_EQUAL:
                        // TODO: Handle strings
                        _createBCBinopExpression(binOpExpr, "<=", false);
                        break;
                    case Parser::GREATER_EQUAL:
                        // TODO: Handle strings
                        _createBCBinopExpression(binOpExpr, ">=", false);
                        break;
                    case Parser::EQUAL:
                    case Parser::NOT_EQUAL: {
                        TypeChecker::TyPtr leftTy;
                        switch (binOpExpr->left->nodeType()) {
                            case Parser::VAR_LVALUE:
                                leftTy = _table->getVariable(
                                                       std::static_pointer_cast<Parser::VarLValueNode>(binOpExpr->left)
                                                               ->variable)
                                                 ->types[0];
                                break;
                            case Parser::BINDING:
                                leftTy = _table->getVariable(
                                                       std::static_pointer_cast<Parser::BindingNode>(binOpExpr->left)
                                                               ->variable)
                                                 ->types[0];
                                break;
                            case Parser::INDEX_LVALUE:
                                leftTy = std::static_pointer_cast<Parser::IndexLValueNode>(binOpExpr->left)->type;
                                break;
                            default:
                                leftTy = std::static_pointer_cast<Parser::ExprNode>(binOpExpr->left)->type;
                                break;
                        }
                        const TypeChecker::TyPtr rightTy = binOpExpr->right->type;
                        if (!leftTy && !rightTy) {
                            // Return true if unknown type
                            _buffer << "$((1))";
                            break;
                        }
                        const TypeChecker::TyPtr type = leftTy ? leftTy : rightTy;
                        std::string opString = binOpExpr->opType == Parser::EQUAL ? "==" : "!=";
                        switch (type->getType()) {
                            case TypeChecker::INT:
                            case TypeChecker::FLOAT:
                            case TypeChecker::BOOL:
                                _createBCBinopExpression(binOpExpr, opString, false);
                                break;
                            case TypeChecker::STRING:
                                _buffer << "$([ ";
                                _transpile(binOpExpr->left);
                                _buffer << " " << opString << " ";
                                _transpile(binOpExpr->right);
                                _buffer << " ] && echo $?)";
                                break;
                            case TypeChecker::TUPLE:
                                // TODO
                                break;
                            case TypeChecker::ARRAY:
                                // TODO
                                break;
                            case TypeChecker::FUNCTION:
                                // TODO
                                break;
                            default:
                                _buffer << "$([ \"";
                                _transpile(binOpExpr->left);
                                _buffer << "\" " << opString << " \"";
                                _transpile(binOpExpr->right);
                                _buffer << "\" ] && echo $?)";
                                break;
                        }
                        break;
                    }
                    case Parser::AND:
                    case Parser::OR:
                        _buffer << "$((";
                        _transpile(binOpExpr->left);
                        _buffer << (binOpExpr->opType == Parser::AND ? " && " : " || ");
                        _transpile(binOpExpr->right);
                        _buffer << "))";
                        break;
                    case Parser::EXPONENTIATE:
                        if (!isFloat) { _buffer << "$(echo \"scale=0; ("; }
                        _buffer << "$(echo \"e(l(";
                        _transpile(binOpExpr->left);
                        _buffer << ") * ";
                        _transpile(binOpExpr->right);
                        _buffer << ")\" | bc -l)";
                        if (!isFloat) { _buffer << " + 0.5) / 1\" | bc -l)"; }
                        break;
                    case Parser::MULTIPLY:
                        _createBCBinopExpression(binOpExpr, "*", true);
                        break;
                    case Parser::DIVIDE:
                        _createBCBinopExpression(binOpExpr, "/", isFloat);
                        break;
                    case Parser::MODULO:
                        _createBCBinopExpression(binOpExpr, "%", false);
                        break;
                    case Parser::ADD: {
                        if (!binOpExpr->type || binOpExpr->type->getType() != TypeChecker::STRING) {
                            _createBCBinopExpression(binOpExpr, "+", false);
                            return;
                        }
                        _buffer << "$(echo \"";
                        _transpile(binOpExpr->left);
                        _buffer << "\"\"";
                        _transpile(binOpExpr->right);
                        _buffer << "\")";
                        return;
                    }
                    case Parser::SUBTRACT:
                        _createBCBinopExpression(binOpExpr, "-", false);
                        break;
                    case Parser::EXPONENTIATE_SET:
                    case Parser::MULTIPLY_SET:
                    case Parser::DIVIDE_SET:
                    case Parser::MODULO_SET:
                    case Parser::ADD_SET:
                    case Parser::SUBTRACT_SET:
                        _transpileOpSetNode(binOpExpr);
                        break;
                    case Parser::SET: {
                        if (binOpExpr->left->nodeType() == Parser::VAR_LVALUE
                            || binOpExpr->left->nodeType() == Parser::BINDING) {
                            Parser::VarLValueNodePtr lvalue;
                            if (binOpExpr->left->nodeType() == Parser::VAR_LVALUE) {
                                lvalue = std::static_pointer_cast<Parser::VarLValueNode>(binOpExpr->left);
                            } else {
                                lvalue = std::make_shared<Parser::VarLValueNode>(
                                        binOpExpr->left->position,
                                        std::static_pointer_cast<Parser::BindingNode>(binOpExpr->left)->variable);
                            }
                            std::string varName = lvalue->variable;
                            if (_scopes.top().find(varName) != _scopes.top().end()) {
                                varName = _scopes.top()[varName];
                            }
                            std::string backupBuffer = _buffer.str();
                            _resetLine();
                            _buffer << varName;
                            _buffer << "=";
                            _transpile(binOpExpr->right);
                            _writeLine();
                            _buffer.str("");
                            _buffer << backupBuffer;
                            _transpile(lvalue);
                        } else {
                            // TODO: Handle the index lvalue case
                        }
                        break;
                    }
                }
                return;
            }
            case Parser::CALL_EXPR: {
                Parser::CallExprNodePtr callExpr = std::static_pointer_cast<Parser::CallExprNode>(astNode);
                bool isVariable = callExpr->func->nodeType() == Parser::LVALUE_EXPR
                               && std::static_pointer_cast<Parser::LValueExprNode>(callExpr->func)->expr->nodeType()
                                          == Parser::VAR_EXPR;
                if (isVariable) {
                    std::string name = std::static_pointer_cast<Parser::VarExprNode>(
                                               std::static_pointer_cast<Parser::LValueExprNode>(callExpr->func)->expr)
                                               ->variable;
                    if (_scopes.top().find(name) != _scopes.top().end()) { name = _scopes.top()[name]; }
                    std::vector<Parser::ExprNodePtr> args = callExpr->args->exprs;
                    if (name == "parseInt" || name == "parseFloat" || name == "parseBool" || name == "toString") {
                        if (args[0]->type && args[0]->type->getType() == TypeChecker::FLOAT && name == "parseInt") {
                            _buffer << "$(echo \"scale=0; ";
                            _transpile(args[0]);
                            _buffer << " / 1\" | bc -l)";
                            return;
                        }
                        if (name == "parseBool") {
                            // TODO: Handle parsing strings to bools
                        }
                        _transpile(args[0]);
                        return;
                    }
                    if (name == "sqrt") {
                        _createBCFunctionExpression(args[0], "sqrt");
                        return;
                    }
                    if (name == "ln") {
                        _createBCFunctionExpression(args[0], "l");
                        return;
                    }
                    if (name == "log") {
                        _buffer << "$(echo \"l(";
                        _transpile(args[0]);
                        _buffer << ") / l(10)\" | bc -l)";
                        return;
                    }
                    if (name == "abs") {
                        _buffer << "$(echo \"";
                        _transpile(args[0]);
                        _buffer << " * (if (";
                        _transpile(args[0]);
                        _buffer << " < 0) -1 else 1)\" | bc -l";
                        return;
                    }
                    if (name == "floor") {
                        _buffer << "$(echo \"scale=0; ";
                        _transpile(args[0]);
                        _buffer << " / 1\" | bc -l)";
                        return;
                    }
                    if (name == "ceil") {
                        _buffer << "$(echo \"scale=0; (";
                        _transpile(args[0]);
                        _buffer << " + 1) / 1\" | bc -l)";
                        return;
                    }
                    if (name == "round") {
                        _buffer << "$(echo \"scale=0; (";
                        _transpile(args[0]);
                        _buffer << " + 0.5) / 1\" | bc -l)";
                        return;
                    }
                    if (name == "sin") {
                        _createBCFunctionExpression(args[0], "s");
                        return;
                    }
                    if (name == "cos") {
                        _createBCFunctionExpression(args[0], "c");
                        return;
                    }
                    if (name == "tan") {
                        _buffer << "$(echo \"s(";
                        _transpile(args[0]);
                        _buffer << ") / c(";
                        _transpile(args[0]);
                        _buffer << ")\" | bc -l)";
                        return;
                    }
                    if (name == "csc") {
                        _buffer << "$(echo \"1 / s(";
                        _transpile(args[0]);
                        _buffer << ")\" | bc -l)";
                        return;
                    }
                    if (name == "sec") {
                        _buffer << "$(echo \"1 / c(";
                        _transpile(args[0]);
                        _buffer << ")\" | bc -l)";
                        return;
                    }
                    if (name == "cot") {
                        _buffer << "$(echo \"c(";
                        _transpile(args[0]);
                        _buffer << ") / s(";
                        _transpile(args[0]);
                        _buffer << ")\" | bc -l)";
                        return;
                    }
                    if (name == "sinh") {
                        _buffer << "$(echo \"(e(";
                        _transpile(args[0]);
                        _buffer << ") - e(-";
                        _transpile(args[0]);
                        _buffer << ")) / 2\" | bc -l)";
                        return;
                    }
                    if (name == "cosh") {
                        _buffer << "$(echo \"(e(";
                        _transpile(args[0]);
                        _buffer << ") + e(-";
                        _transpile(args[0]);
                        _buffer << ")) / 2\" | bc -l)";
                        return;
                    }
                    if (name == "tanh") {
                        _buffer << "$(echo \"(e(";
                        _transpile(args[0]);
                        _buffer << ") - e(-";
                        _transpile(args[0]);
                        _buffer << ")) / (e(";
                        _transpile(args[0]);
                        _buffer << ") + e(-";
                        _transpile(args[0]);
                        _buffer << ")\" | bc -l)";
                        return;
                    }
                    if (name == "csch") {
                        _buffer << "$(echo \"2 / (e(";
                        _transpile(args[0]);
                        _buffer << ") - e(-";
                        _transpile(args[0]);
                        _buffer << "))\" | bc -l)";
                        return;
                    }
                    if (name == "sech") {
                        _buffer << "$(echo \"2 / (e(";
                        _transpile(args[0]);
                        _buffer << ") + e(-";
                        _transpile(args[0]);
                        _buffer << "))\" | bc -l)";
                        return;
                    }
                    if (name == "coth") {
                        _buffer << "$(echo \"(e(";
                        _transpile(args[0]);
                        _buffer << ") + e(-";
                        _transpile(args[0]);
                        _buffer << ")) / (e(";
                        _transpile(args[0]);
                        _buffer << ") - e(-";
                        _transpile(args[0]);
                        _buffer << ")\" | bc -l)";
                        return;
                    }
                    if (name == "arcsin") {
                        _buffer << "$(echo \"a(";
                        _transpile(args[0]);
                        _buffer << " / sqrt(1 - e(2 * l(";
                        _transpile(args[0]);
                        _buffer << "))))\" | bc -l)";
                        return;
                    }
                    if (name == "arccos") {
                        _buffer << "$(echo \"if (";
                        _transpile(args[0]);
                        _buffer << " == 0) " << Util::PI << " / 2 else a(sqrt(1 - e(2 * l(";
                        _transpile(args[0]);
                        _buffer << "))) / ";
                        _transpile(args[0]);
                        _buffer << ") + (if (";
                        _transpile(args[0]);
                        _buffer << " < 0) " << Util::PI << " else 0)\" | bc -l)";
                        return;
                    }
                    if (name == "arctan") {
                        _createBCFunctionExpression(args[0], "a");
                        return;
                    }
                    if (name == "arccsc") {
                        _buffer << "$(echo \"a(1 / (";
                        _transpile(args[0]);
                        _buffer << " * sqrt(1 - e(2 * l(1 / ";
                        _transpile(args[0]);
                        _buffer << "))))\" | bc -l)";
                        return;
                    }
                    if (name == "arcsec") {
                        _buffer << "$(echo \"a(sqrt(1 - e(2 * l(1 / ";
                        _transpile(args[0]);
                        _buffer << "))) * ";
                        _transpile(args[0]);
                        _buffer << ") + (if (";
                        _transpile(args[0]);
                        _buffer << " < 0) " << Util::PI << " else 0)\" | bc -l)";
                        return;
                    }
                    if (name == "arccot") {
                        _buffer << "$(echo \"" << Util::PI << " / 2 - a(";
                        _transpile(args[0]);
                        _buffer << ")\" | bc -l)";
                        return;
                    }
                    if (name == "arcsinh") {
                        _buffer << "$(echo \"l(";
                        _transpile(args[0]);
                        _buffer << " + sqrt(1 + e(2 * l(";
                        _transpile(args[0]);
                        _buffer << "))))\" | bc -l)";
                        return;
                    }
                    if (name == "arccosh") {
                        _buffer << "$(echo \"l(";
                        _transpile(args[0]);
                        _buffer << " + sqrt(-1 + e(2 * l(";
                        _transpile(args[0]);
                        _buffer << "))))\" | bc -l)";
                        return;
                    }
                    if (name == "arctanh") {
                        _buffer << "$(echo \"l((1 + ";
                        _transpile(args[0]);
                        _buffer << ") / (1 - ";
                        _transpile(args[0]);
                        _buffer << ")) / 2\" | bc -l)";
                        return;
                    }
                    if (name == "arccsch") {
                        _buffer << "$(echo \"l((1 / ";
                        _transpile(args[0]);
                        _buffer << ") + sqrt((1 / e(2 * l(";
                        _transpile(args[0]);
                        _buffer << "))) + 1))\" | bc -l)";
                        return;
                    }
                    if (name == "arcsech") {
                        _buffer << "$(echo \"l((1 / ";
                        _transpile(args[0]);
                        _buffer << ") + sqrt((1 / e(2 * l(";
                        _transpile(args[0]);
                        _buffer << "))) - 1))\" | bc -l)";
                        return;
                    }
                    if (name == "arccoth") {
                        _buffer << "$(echo \"l((1 + ";
                        _transpile(args[0]);
                        _buffer << ") / (-1 - ";
                        _transpile(args[0]);
                        _buffer << ")) / 2\" | bc -l)";
                        return;
                    }
                    if (name == "random") {
                        _buffer << "$(echo \"$RANDOM / 32767\" | bc -l)";
                        return;
                    }
                    if (name == "time") {
                        _buffer << "$(date +%s%3N)";
                        return;
                    }
                    if (name == "date") {
                        // TODO
                    }
                    if (name == "sleep") {
                        _buffer << "$(sleep $(echo \"scale=3; ";
                        _transpile(args[0]);
                        _buffer << " / 1000\" | bc -l))";
                        return;
                    }
                    if (name == "charAt") {
                        _buffer << "$(echo ";
                        _transpile(args[0]);
                        _buffer << " | cut -c";
                        Parser::BinOpExprNodePtr bin = std::make_shared<Parser::BinOpExprNode>(
                                args[1], Parser::SUBTRACT, std::make_shared<Parser::IntExprNode>(args[1]->position, 1));
                        bin->type = TypeChecker::INT_TY;
                        _transpile(bin);
                        _buffer << ")";
                        return;
                    }
                    if (name == "startsWith") {
                        _buffer << "$([ ";
                        _transpile(args[0]);
                        _buffer << " == ";
                        _transpile(args[1]);
                        _buffer << "* ] && echo $?)";
                        return;
                    }
                    if (name == "endsWith") {
                        _buffer << "$([ ";
                        _transpile(args[0]);
                        _buffer << " == *";
                        _transpile(args[1]);
                        _buffer << " ] && echo $?)";
                        return;
                    }
                    if (name == "includes") {
                        switch (args[0]->type->getType()) {
                            case TypeChecker::TUPLE:
                                // TODO
                                break;
                            case TypeChecker::ARRAY:
                                // TODO
                                break;
                            case TypeChecker::STRING:
                                _buffer << "$([ ";
                                _transpile(args[0]);
                                _buffer << " == *";
                                _transpile(args[1]);
                                _buffer << "* ] && echo $?)";
                                break;
                            default:
                                break;
                        }
                        return;
                    }
                    if (name == "indexOf") {
                        switch (args[0]->type->getType()) {
                            case TypeChecker::TUPLE:
                                // TODO
                                break;
                            case TypeChecker::ARRAY:
                                // TODO
                                break;
                            case TypeChecker::STRING:
                                // TODO: Refactor so it doesn't use variables
                                _buffer << "$(awk -v _1=";
                                _transpile(args[0]);
                                _buffer << " -v _2=";
                                _transpile(args[1]);
                                _buffer << " 'BEGIN{print index(_1, _2)}')";
                                break;
                            default:
                                break;
                        }
                        return;
                    }
                    if (name == "length") {
                        switch (args[0]->type->getType()) {
                            case TypeChecker::TUPLE:
                                // TODO
                                break;
                            case TypeChecker::ARRAY:
                                // TODO
                                break;
                            case TypeChecker::STRING:
                                _buffer << "$(echo ";
                                _transpile(args[0]);
                                _buffer << " | wc -c)";
                                break;
                            default:
                                break;
                        }
                        return;
                    }
                    if (name == "replace") {
                        _buffer << "$(echo ";
                        _transpile(args[0]);
                        _buffer << " | sed 's/";
                        _transpile(args[1]);
                        _buffer << "/";
                        _transpile(args[2]);
                        _buffer << "/')";
                        return;
                    }
                    if (name == "replaceAll") {
                        _buffer << "$(echo ";
                        _transpile(args[0]);
                        _buffer << " | sed 's/";
                        _transpile(args[1]);
                        _buffer << "/";
                        _transpile(args[2]);
                        _buffer << "/g')";
                        return;
                    }
                    if (name == "substring") {
                        if (args.size() == 2) {
                            _buffer << "$(echo ";
                            _transpile(args[0]);
                            _buffer << " | cut -c";
                            Parser::BinOpExprNodePtr bin = std::make_shared<Parser::BinOpExprNode>(
                                    args[1], Parser::SUBTRACT,
                                    std::make_shared<Parser::IntExprNode>(args[1]->position, 1));
                            bin->type = TypeChecker::INT_TY;
                            _transpile(bin);
                            _buffer << "-)";
                        } else {
                            _buffer << "$(echo ";
                            _transpile(args[0]);
                            _buffer << " | cut -c";
                            Parser::BinOpExprNodePtr bin1 = std::make_shared<Parser::BinOpExprNode>(
                                    args[1], Parser::SUBTRACT,
                                    std::make_shared<Parser::IntExprNode>(args[1]->position, 1));
                            bin1->type = TypeChecker::INT_TY;
                            _transpile(bin1);
                            _buffer << "-";
                            Parser::BinOpExprNodePtr bin2 = std::make_shared<Parser::BinOpExprNode>(
                                    args[2], Parser::SUBTRACT,
                                    std::make_shared<Parser::IntExprNode>(args[2]->position, 2));
                            bin2->type = TypeChecker::INT_TY;
                            _transpile(bin2);
                            _buffer << ")";
                        }
                    }
                    if (name == "trim") {
                        _buffer << "$(echo ";
                        _transpile(args[0]);
                        _buffer << " | sed -e 's/^[[:space:]]*//' -e 's/[[:space:]]*$//')";
                    }
                    if (name == "lower") {
                        _buffer << "$(echo \"";
                        _transpile(args[0]);
                        _buffer << "\" | tr '[:upper:]' '[:lower:]')";
                        return;
                    }
                    if (name == "upper") {
                        _buffer << "$(echo \"";
                        _transpile(args[0]);
                        _buffer << "\" | tr '[:lower:]' '[:upper:]')";
                        return;
                    }
                    if (name == "split") {
                        // TODO
                    }
                    if (name == "sort") {
                        // TODO
                    }
                    if (name == "filter") {
                        // TODO
                    }
                    if (name == "map") {
                        // TODO
                    }
                    if (name == "foreach") {
                        // TODO
                    }
                    if (name == "append") {
                        // TODO
                    }
                    if (name == "remove") {
                        // TODO
                    }
                }
                _buffer << "$(";
                if (isVariable) {
                    std::string name = std::static_pointer_cast<Parser::VarExprNode>(
                                               std::static_pointer_cast<Parser::LValueExprNode>(callExpr->func)->expr)
                                               ->variable;
                    _buffer << name;
                } else {
                    _transpile(callExpr->func);
                }
                _transpile(callExpr->args);
                _buffer << ")";
                return;
            }
            case Parser::API_CALL_EXPR: {
                Parser::ApiCallExprNodePtr apiCallExpr = std::static_pointer_cast<Parser::ApiCallExprNode>(astNode);
                _transpile(std::make_shared<Parser::CallExprNode>(
                        std::make_shared<Parser::LValueExprNode>(
                                std::make_shared<Parser::VarLValueNode>(apiCallExpr->funcPosition, apiCallExpr->func)),
                        apiCallExpr->args));
                return;
            }
            case Parser::LAMBDA_EXPR: {
                // TODO: Implement
                return;
            }
            case Parser::CMD_EXPR: {
                // TODO: Implement
                return;
            }
            case Parser::SCAN_EXPR: {
                _buffer << "$(read -p \"";
                _transpile(std::static_pointer_cast<Parser::ScanExprNode>(astNode)->prompt);
                _buffer << "\" && echo \"$REPLY\")";
                return;
            }
            case Parser::READ_EXPR: {
                _buffer << "$(cat \"" << std::static_pointer_cast<Parser::ReadExprNode>(astNode)->filePath << "\")";
                return;
            }
            case Parser::EXPRS: {
                _buffer << " ";
                for (const Parser::ExprNodePtr& exprPtr : std::static_pointer_cast<Parser::ExprsNode>(astNode)->exprs) {
                    _transpile(exprPtr);
                    _buffer << " ";
                }
                return;
            }
            case Parser::PRGM: {
                _transpile(std::static_pointer_cast<Parser::PrgmNode>(astNode)->stmts);
                return;
            }
            case Parser::IF_STMT: {
                const Parser::IfStmtNodePtr ifStmtNode = std::static_pointer_cast<Parser::IfStmtNode>(astNode);
                _buffer << "if [ \"";
                _transpile(ifStmtNode->condition);
                _buffer << " \" -eq 1 ]; then";
                _incrementIndent();
                _writeLine();
                _transpile(ifStmtNode->trueStmt);
                _decrementIndent();
                _resetLine();
                if (ifStmtNode->falseStmt) {
                    _buffer << "else";
                    _incrementIndent();
                    _writeLine();
                    _transpile(ifStmtNode->falseStmt);
                    _decrementIndent();
                    _resetLine();
                }
                _buffer << "fi";
                _writeLine();
                return;
            }
            case Parser::FOR_STMT: {
                const Parser::ForStmtNodePtr forStmtNode = std::static_pointer_cast<Parser::ForStmtNode>(astNode);
                _transpile(std::make_shared<Parser::ExprStmtNode>(forStmtNode->initial));
                Parser::ScopeStmtNodePtr body = std::make_shared<Parser::ScopeStmtNode>(forStmtNode->body->position);
                if (forStmtNode->body->nodeType() == Parser::SCOPE_STMT) {
                    Parser::ScopeStmtNodePtr forBody = std::static_pointer_cast<Parser::ScopeStmtNode>(
                            forStmtNode->body);
                    body->stmts->stmts.insert(body->stmts->stmts.end(), forBody->stmts->stmts.begin(),
                                              forBody->stmts->stmts.end());
                } else {
                    body->stmts->stmts.push_back(forStmtNode->body);
                }
                body->stmts->stmts.push_back(std::make_shared<Parser::ExprStmtNode>(forStmtNode->update));
                _transpile(
                        std::make_shared<Parser::WhileStmtNode>(forStmtNode->position, forStmtNode->condition, body));
                return;
            }
            case Parser::WHILE_STMT: {
                const Parser::WhileStmtNodePtr whileStmtNode = std::static_pointer_cast<Parser::WhileStmtNode>(astNode);
                _buffer << "while [ \"";
                _transpile(whileStmtNode->condition);
                _buffer << "\" -eq 1 ]; do";
                _incrementIndent();
                _writeLine();
                _transpile(whileStmtNode->body);
                _decrementIndent();
                _resetLine();
                _buffer << "done";
                _writeLine();
                return;
            }
            case Parser::DO_WHILE_STMT: {
                const Parser::DoWhileStmtNodePtr doWhileStmtNode = std::static_pointer_cast<Parser::DoWhileStmtNode>(
                        astNode);
                _buffer << "while true; do";
                _incrementIndent();
                _writeLine();
                _transpile(doWhileStmtNode->body);
                _buffer << "[ \"";
                _transpile(doWhileStmtNode->condition);
                _buffer << "\" -eq 1 ] || break";
                _decrementIndent();
                _writeLine();
                return;
            }
            case Parser::RETURN_STMT: {
                _buffer << "echo ";
                _transpile(std::static_pointer_cast<Parser::ReturnStmtNode>(astNode)->retExpr);
                _writeLine();
                return;
            }
            case Parser::BREAK_STMT: {
                _buffer << "break";
                _writeLine();
                return;
            }
            case Parser::CONTINUE_STMT: {
                _buffer << "continue";
                _writeLine();
                return;
            }
            case Parser::SCOPE_STMT: {
                _transpile(std::static_pointer_cast<Parser::ScopeStmtNode>(astNode)->stmts);
                return;
            }
            case Parser::CMD_STMT: {
                _transpile(std::static_pointer_cast<Parser::CmdStmtNode>(astNode)->command);
                _writeLine();
                return;
            }
            case Parser::EXPR_STMT: {
                _buffer << "echo ";
                _transpile(std::static_pointer_cast<Parser::ExprStmtNode>(astNode)->expression);
                _buffer << " > /dev/null";
                _writeLine();
                return;
            }
            case Parser::ALIAS_STMT: {
                Parser::AliasStmtNodePtr aliasStmt = std::static_pointer_cast<Parser::AliasStmtNode>(astNode);
                _aliases.insert(aliasStmt->alias);
                _buffer << "alias " << aliasStmt->alias << "='";
                _transpile(aliasStmt->command);
                _buffer << "'";
                _writeLine();
                return;
            }
            case Parser::IMPORT_STMT: {
                for (const auto& node : std::static_pointer_cast<Parser::ImportStmtNode>(astNode)->prgm) {
                    _transpile(node);
                }
                return;
            }
            case Parser::PRINT_STMT: {
                _buffer << "echo -n ";
                _transpile(std::static_pointer_cast<Parser::PrintStmtNode>(astNode)->expression);
                _writeLine();
                return;
            }
            case Parser::PRINTLN_STMT: {
                _buffer << "echo ";
                _transpile(std::static_pointer_cast<Parser::PrintlnStmtNode>(astNode)->expression);
                _writeLine();
                return;
            }
            case Parser::WRITE_STMT: {
                const Parser::WriteStmtNodePtr writeStmtNode = std::static_pointer_cast<Parser::WriteStmtNode>(astNode);
                _buffer << "echo ";
                _transpile(writeStmtNode->fileData);
                _buffer << " > ";
                _transpile(writeStmtNode->filePath);
                _writeLine();
                return;
            }
            case Parser::FUNCTION_STMT: {
                const Parser::FunctionStmtNodePtr functionStmt = std::static_pointer_cast<Parser::FunctionStmtNode>(
                        astNode);
                _buffer << functionStmt->name << "() {";
                _incrementIndent();
                _writeLine();
                std::map<std::string, std::string> newScope;
                int i = 1;
                for (Parser::BindingNodePtr bindingNode : functionStmt->bindings->bindings) {
                    newScope[bindingNode->variable] = std::to_string(i++);
                }
                _pushScope(newScope, functionStmt->table);
                _transpile(functionStmt->body);
                _decrementIndent();
                _resetLine();
                _buffer << "}";
                _writeLine();
                _popScope();
                return;
            }
            case Parser::TIMEOUT_STMT: {
                Parser::TimeoutStmtNodePtr timeoutStmt = std::static_pointer_cast<Parser::TimeoutStmtNode>(astNode);
                _buffer << "(timeout ";
                _buffer << (((double)timeoutStmt->timeout) / 1000.0);
                _buffer << "s";
                _incrementIndent();
                _writeLine();
                _transpile(timeoutStmt->stmt);
                _decrementIndent();
                _resetLine();
                _buffer << ")";
                _writeLine();
                _buffer << "if [ $? -eq 124 ]; then";
                _incrementIndent();
                _writeLine();
                _buffer << "echo ";
                _transpile(timeoutStmt->message);
                _writeLine();
                _decrementIndent();
                _resetLine();
                _buffer << "fi";
                _writeLine();
                return;
            }
            case Parser::ASSERT_STMT: {
                const Parser::AssertStmtNodePtr assertStmtNode = std::static_pointer_cast<Parser::AssertStmtNode>(
                        astNode);
                _buffer << "if [ \"";
                _transpile(assertStmtNode->expr);
                _buffer << "\" -eq 0 ]; then";
                _incrementIndent();
                _writeLine();
                _buffer << "echo ";
                _transpile(assertStmtNode->message);
                _writeLine();
                _buffer << "exit 1";
                _writeLine();
                _decrementIndent();
                _resetLine();
                _buffer << "fi";
                _writeLine();
                return;
            }
            case Parser::STMTS: {
                for (const Parser::StmtNodePtr& stmtPtr : std::static_pointer_cast<Parser::StmtsNode>(astNode)->stmts) {
                    _transpile(stmtPtr);
                }
                return;
            }
            case Parser::STRING: {
                Parser::StringNodePtr stringPtr = std::static_pointer_cast<Parser::StringNode>(astNode);
                if (!stringPtr->isLiteral()) {
                    _transpile(stringPtr->expressions);
                    return;
                }
                if (stringPtr->literal.empty()) { return; }
                if (_aliases.find(stringPtr->literal) != _aliases.end()) {
                    _buffer << stringPtr->literal;
                    return;
                }
                _buffer << "$(echo \"" << stringPtr->literal << "\")";
                return;
            }
            case Parser::STRING_EXPRS: {
                for (const Parser::ExprNodePtr& exprPtr :
                     std::static_pointer_cast<Parser::StringExprsNode>(astNode)->expressions) {
                    _transpile(exprPtr);
                }
                return;
            }
            default:
                return;
        }
    }

    void BashTranspiler::_writeLine() {
        _lines.push_back(_buffer.str());
        _resetLine();
    }

    void BashTranspiler::_resetLine() {
        _buffer.str("");
        for (int i = 0; i < _indent; i++) { _buffer << ' '; }
    }

    void BashTranspiler::_incrementIndent() { _indent += INDENT_SIZE; }

    void BashTranspiler::_decrementIndent() { _indent -= INDENT_SIZE; }

    void BashTranspiler::_pushScope(const std::map<std::string, std::string>& scope,
                                    const TypeChecker::VariableTablePtr& table) {
        _scopes.push(scope);
        _table = table;
    }

    void BashTranspiler::_popScope() {
        _scopes.pop();
        auto parent = _table->getParent();
        if (parent) { _table = parent; }
    }

    void BashTranspiler::_transpileOpSetNode(const Parser::BinOpExprNodePtr& binop) {
        Parser::BinOpType opr;
        switch (binop->opType) {
            case Parser::ADD_SET:
                opr = Parser::ADD;
                break;
            case Parser::SUBTRACT_SET:
                opr = Parser::SUBTRACT;
                break;
            case Parser::MULTIPLY_SET:
                opr = Parser::MULTIPLY;
                break;
            case Parser::DIVIDE_SET:
                opr = Parser::DIVIDE;
                break;
            case Parser::MODULO_SET:
                opr = Parser::MODULO;
                break;
            case Parser::EXPONENTIATE_SET:
                opr = Parser::EXPONENTIATE;
                break;
            default:
                throw Util::CommanderException("Unexpected binop in _transpileOpSetNode(): "
                                               + Parser::binOpToString(binop->opType));
        }
        const Parser::BinOpExprNodePtr newBinop = std::make_shared<Parser::BinOpExprNode>(binop->left, opr,
                                                                                          binop->right);
        newBinop->type = binop->type;
        const Parser::BinOpExprNodePtr newBinopTwo = std::make_shared<Parser::BinOpExprNode>(binop->left, Parser::SET,
                                                                                             newBinop);
        newBinopTwo->type = binop->type;
        _transpile(newBinopTwo);
    }

    void BashTranspiler::_createBCBinopExpression(const Parser::BinOpExprNodePtr& binop, const std::string& op,
                                                  const bool& useLib) {
        _buffer << "$(echo \"";
        _transpile(binop->left);
        _buffer << " " << op << " ";
        _transpile(binop->right);
        _buffer << "\" | bc";
        if (useLib) { _buffer << " -l"; }
        _buffer << ")";
    }

    void BashTranspiler::_createBCFunctionExpression(const Parser::ExprNodePtr& expr, const std::string& func) {
        _buffer << "$(echo \"" << func << "(";
        _transpile(expr);
        _buffer << ")\" | bc -l)";
    }

}  // namespace BashTranspiler
