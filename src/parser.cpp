#include "../include/parser.h"
#include <iostream>

Token Parser::currentToken() {
    if (pos < tokens.size())
        return tokens[pos];
    return {TokenType::END, ""};
}

void Parser::consume(TokenType expected, const std::string &expectedLexeme) {
    Token token = currentToken();
    if (token.type != expected || (!expectedLexeme.empty() && token.lexeme != expectedLexeme)) {
        throw std::runtime_error("语法错误: 期待 " + expectedLexeme + "，但得到 " + token.lexeme);
    }
    pos++;
}

bool Parser::match(TokenType type, const std::string &lexeme) {
    Token token = currentToken();
    if (token.type == type && (lexeme.empty() || token.lexeme == lexeme)) {
        pos++;
        return true;
    }
    return false;
}

// 解析程序：既可能包含全局函数定义，也可能包含全局声明或语句
std::unique_ptr<ProgramNode> Parser::parseProgram() {
    auto program = std::make_unique<ProgramNode>();

    // 如果程序以 { 开始，则认为整个程序被块包围
    if (currentToken().type == TokenType::DELIMITER && currentToken().lexeme == "{") {
        consume(TokenType::DELIMITER, "{");
        while (!(currentToken().type == TokenType::DELIMITER && currentToken().lexeme == "}")) {
            if (currentToken().type == TokenType::KEYWORD &&
               (currentToken().lexeme == "int" || currentToken().lexeme == "bool")) {
                // 判断是函数定义还是全局变量声明
                if (pos + 1 < tokens.size() && tokens[pos+1].type == TokenType::IDENTIFIER &&
                    pos + 2 < tokens.size() && tokens[pos+2].type == TokenType::DELIMITER &&
                    tokens[pos+2].lexeme == "(") {
                    program->functions.push_back(parseFuncDef());
                } else {
                    program->decls.push_back(parseDecl());
                }
            } else {
                program->stmts.push_back(parseStmt());
            }
        }
        consume(TokenType::DELIMITER, "}");
    }
    // 否则，不带外层块，直接解析到文件结束
    else {
        while (currentToken().type != TokenType::END) {
            if (currentToken().type == TokenType::KEYWORD &&
               (currentToken().lexeme == "int" || currentToken().lexeme == "bool")) {
                if (pos + 1 < tokens.size() && tokens[pos+1].type == TokenType::IDENTIFIER &&
                    pos + 2 < tokens.size() && tokens[pos+2].type == TokenType::DELIMITER &&
                    tokens[pos+2].lexeme == "(") {
                    program->functions.push_back(parseFuncDef());
                } else {
                    program->decls.push_back(parseDecl());
                }
            } else {
                program->stmts.push_back(parseStmt());
            }
        }
    }
    return program;
}


// 新增：解析函数定义，形如：
// returnType IDENTIFIER ( [参数列表] ) { 函数体 }
// 参数列表中各参数形如： type IDENTIFIER [ = literal ]
std::unique_ptr<FuncDefNode> Parser::parseFuncDef() {
    auto func = std::make_unique<FuncDefNode>();
    // 返回类型
    func->returnType = currentToken().lexeme;
    consume(TokenType::KEYWORD, func->returnType);
    // 函数名
    Token id = currentToken();
    if (id.type != TokenType::IDENTIFIER)
        throw std::runtime_error("语法错误: 函数定义期望标识符");
    func->name = id.lexeme;
    consume(TokenType::IDENTIFIER);
    // 参数列表
    consume(TokenType::DELIMITER, "(");
    while (!(currentToken().type == TokenType::DELIMITER && currentToken().lexeme == ")")) {
        Parameter param;
        // 参数类型
        if (currentToken().type != TokenType::KEYWORD ||
            (currentToken().lexeme != "int" && currentToken().lexeme != "bool"))
            throw std::runtime_error("语法错误: 参数类型应为 int 或 bool");
        param.type = currentToken().lexeme;
        consume(TokenType::KEYWORD, param.type);
        // 参数名
        if (currentToken().type != TokenType::IDENTIFIER)
            throw std::runtime_error("语法错误: 参数期望标识符");
        param.name = currentToken().lexeme;
        consume(TokenType::IDENTIFIER);
        // 可选的默认值
        if (currentToken().type == TokenType::OPERATOR && currentToken().lexeme == "=") {
            consume(TokenType::OPERATOR, "=");
            // 默认值要求为整数或浮点字面量
            if (currentToken().type != TokenType::INTEGER && currentToken().type != TokenType::FLOAT)
                throw std::runtime_error("语法错误: 参数默认值应为整数或浮点数");
            param.defaultVal = currentToken().lexeme;
            consume(currentToken().type);
        }
        func->params.push_back(param);
        // 参数之间使用分号分隔（测试案例中使用分号）
        if (currentToken().type == TokenType::DELIMITER && currentToken().lexeme == ";") {
            consume(TokenType::DELIMITER, ";");
        }
        else {
            break; // 若不是分号，则参数列表结束或后续有语法错误
        }
    }
    consume(TokenType::DELIMITER, ")");
    // 函数体必须为块语句
    auto block = parseBlock();
    // 动态转换为 BlockStmtNode 类型
    func->body = std::unique_ptr<BlockStmtNode>(dynamic_cast<BlockStmtNode*>(block.release()));
    if (!func->body)
        throw std::runtime_error("语法错误: 函数体必须为块语句");
    return func;
}

std::unique_ptr<DeclNode> Parser::parseDecl() {
    auto decl = std::make_unique<DeclNode>();
    // 声明： "int" 或 "bool" 后跟标识符列表，以 ; 结尾
    Token token = currentToken();
    if (token.lexeme == "int" || token.lexeme == "bool") {
        decl->type = token.lexeme;
        consume(TokenType::KEYWORD, token.lexeme);
    } else {
        throw std::runtime_error("语法错误: 声明必须以 int 或 bool 开始");
    }
    // 至少一个标识符
    Token idToken = currentToken();
    if (idToken.type != TokenType::IDENTIFIER)
        throw std::runtime_error("语法错误: 声明缺少标识符");
    decl->names.push_back(idToken.lexeme);
    consume(TokenType::IDENTIFIER);
    // 多个标识符以逗号分隔
    while (currentToken().type == TokenType::DELIMITER && currentToken().lexeme == ",") {
        consume(TokenType::DELIMITER, ",");
        idToken = currentToken();
        if (idToken.type != TokenType::IDENTIFIER)
            throw std::runtime_error("语法错误: 声明中缺少标识符");
        decl->names.push_back(idToken.lexeme);
        consume(TokenType::IDENTIFIER);
    }
    consume(TokenType::DELIMITER, ";");
    return decl;
}

std::unique_ptr<StmtNode> Parser::parseStmt() {
    Token token = currentToken();
    if (token.type == TokenType::KEYWORD) {
        if (token.lexeme == "if") {
            consume(TokenType::KEYWORD, "if");
            Token cond = currentToken();
            if (cond.type != TokenType::IDENTIFIER)
                throw std::runtime_error("语法错误: if 条件部分期望标识符");
            auto condition = std::make_unique<IdentifierExprNode>(cond.lexeme);
            consume(TokenType::IDENTIFIER);
            consume(TokenType::KEYWORD, "then");
            auto thenStmt = parseStmt();
            std::unique_ptr<StmtNode> elseStmt = nullptr;
            if (currentToken().type == TokenType::KEYWORD && currentToken().lexeme == "else") {
                consume(TokenType::KEYWORD, "else");
                elseStmt = parseStmt();
            }
            auto ifStmt = std::make_unique<IfStmtNode>();
            ifStmt->condition = std::move(condition);
            ifStmt->thenStmt = std::move(thenStmt);
            ifStmt->elseStmt = std::move(elseStmt);
            return ifStmt;
        }
        else if (token.lexeme == "while") {
            consume(TokenType::KEYWORD, "while");
            Token cond = currentToken();
            if (cond.type != TokenType::IDENTIFIER)
                throw std::runtime_error("语法错误: while 条件部分期望标识符");
            auto condition = std::make_unique<IdentifierExprNode>(cond.lexeme);
            consume(TokenType::IDENTIFIER);
            consume(TokenType::KEYWORD, "do");
            auto body = parseStmt();
            auto whileStmt = std::make_unique<WhileStmtNode>();
            whileStmt->condition = std::move(condition);
            whileStmt->body = std::move(body);
            return whileStmt;
        }
        else if (token.lexeme == "read") {
            consume(TokenType::KEYWORD, "read");
            Token id = currentToken();
            if (id.type != TokenType::IDENTIFIER)
                throw std::runtime_error("语法错误: read 语句期望标识符");
            string varName = id.lexeme;
            consume(TokenType::IDENTIFIER);
            consume(TokenType::DELIMITER, ";");
            return std::make_unique<ReadStmtNode>(varName);
        }
        else if (token.lexeme == "write") {
            consume(TokenType::KEYWORD, "write");
            // 此处考虑写语句中可能有多个标识符，中间以逗号分隔
            vector<string> vars;
            Token id = currentToken();
            if (id.type != TokenType::IDENTIFIER)
                throw std::runtime_error("语法错误: write 语句期望标识符");
            vars.push_back(id.lexeme);
            consume(TokenType::IDENTIFIER);
            while (currentToken().type == TokenType::DELIMITER && currentToken().lexeme == ",") {
                consume(TokenType::DELIMITER, ",");
                id = currentToken();
                if (id.type != TokenType::IDENTIFIER)
                    throw std::runtime_error("语法错误: write 语句期望标识符");
                vars.push_back(id.lexeme);
                consume(TokenType::IDENTIFIER);
            }
            consume(TokenType::DELIMITER, ";");
            // 此处将写语句视为一个表达式语句，输出时只打印第一个变量（或根据需要扩展 AST）
            // 为简单起见，我们只生成一个 WriteStmtNode，并将第一个标识符传入
            return std::make_unique<WriteStmtNode>(vars.front());
        }
    }
    else if (token.type == TokenType::DELIMITER && token.lexeme == "{") {
        return parseBlock();
    }
    // 赋值语句： id = EXPR ; 或 id := EXPR ;
    if (token.type == TokenType::IDENTIFIER) {
        string varName = token.lexeme;
        consume(TokenType::IDENTIFIER);
        Token op = currentToken();
        if (op.type == TokenType::OPERATOR && op.lexeme == "=") {
            consume(TokenType::OPERATOR, "=");
            auto expr = parseExpr();
            consume(TokenType::DELIMITER, ";");
            auto assignExpr = std::make_unique<BinaryExprNode>("=", std::make_unique<IdentifierExprNode>(varName), std::move(expr));
            return std::make_unique<ExprStmtNode>(std::move(assignExpr));
        }
        else if (op.type == TokenType::OPERATOR && op.lexeme == ":=") {
            consume(TokenType::OPERATOR, ":=");
            auto expr = parseExpr();
            consume(TokenType::DELIMITER, ";");
            auto assignExpr = std::make_unique<BinaryExprNode>(":=", std::make_unique<IdentifierExprNode>(varName), std::move(expr));
            return std::make_unique<ExprStmtNode>(std::move(assignExpr));
        }
        else {
            throw std::runtime_error("语法错误: 赋值语句缺少 '=' 或 ':='");
        }
    }
    throw std::runtime_error("语法错误: 未识别的语句起始符 " + token.lexeme);
}

std::unique_ptr<StmtNode> Parser::parseBlock() {
    consume(TokenType::DELIMITER, "{");
    auto block = std::make_unique<BlockStmtNode>();
    while (!(currentToken().type == TokenType::DELIMITER && currentToken().lexeme == "}")) {
        block->stmts.push_back(parseStmt());
    }
    consume(TokenType::DELIMITER, "}");
    return block;
}

std::unique_ptr<ExprNode> Parser::parseExpr() {
    auto left = parseTerm();
    while (currentToken().type == TokenType::OPERATOR &&
           (currentToken().lexeme == "+" || currentToken().lexeme == "-")) {
        string op = currentToken().lexeme;
        consume(TokenType::OPERATOR, op);
        auto right = parseTerm();
        left = std::make_unique<BinaryExprNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<ExprNode> Parser::parseTerm() {
    auto left = parseFactor();
    while (currentToken().type == TokenType::OPERATOR &&
           (currentToken().lexeme == "*" || currentToken().lexeme == "/")) {
        string op = currentToken().lexeme;
        consume(TokenType::OPERATOR, op);
        auto right = parseFactor();
        left = std::make_unique<BinaryExprNode>(op, std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<ExprNode> Parser::parseFactor() {
    if (currentToken().type == TokenType::OPERATOR && currentToken().lexeme == "-") {
        consume(TokenType::OPERATOR, "-");
        auto factor = parseFactor();
        auto zero = std::make_unique<LiteralExprNode>("0");
        return std::make_unique<BinaryExprNode>("-", std::move(zero), std::move(factor));
    }
    return parsePrimary();
}

std::unique_ptr<ExprNode> Parser::parsePrimary() {
    Token token = currentToken();
    if (token.type == TokenType::INTEGER || token.type == TokenType::FLOAT) {
        consume(token.type);
        return std::make_unique<LiteralExprNode>(token.lexeme);
    }
    else if (token.type == TokenType::IDENTIFIER) {
        consume(TokenType::IDENTIFIER);
        return std::make_unique<IdentifierExprNode>(token.lexeme);
    }
    else if (token.type == TokenType::DELIMITER && token.lexeme == "(") {
        consume(TokenType::DELIMITER, "(");
        auto expr = parseExpr();
        consume(TokenType::DELIMITER, ")");
        return expr;
    }
    throw std::runtime_error("语法错误: 在表达式中未识别到合法的标识符、数字或 '('");
}
