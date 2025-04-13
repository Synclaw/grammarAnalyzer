#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "lexer.h"
#include <vector>
#include <memory>
#include <stdexcept>

class Parser {
public:
    Parser(const std::vector<Token> &tokens) : tokens(tokens), pos(0) {}
    // 解析整个程序，返回 ProgramNode 指针
    std::unique_ptr<ProgramNode> parseProgram();

private:
    std::vector<Token> tokens;
    size_t pos;

    Token currentToken();
    void consume(TokenType expected, const std::string &expectedLexeme = "");
    bool match(TokenType type, const std::string &lexeme = "");

    // 新增：函数定义解析
    std::unique_ptr<FuncDefNode> parseFuncDef();

    // 解析全局声明（变量声明）
    std::unique_ptr<DeclNode> parseDecl();

    // 语句解析
    std::unique_ptr<StmtNode> parseStmt();
    std::unique_ptr<StmtNode> parseBlock();

    // 表达式解析
    std::unique_ptr<ExprNode> parseExpr();
    std::unique_ptr<ExprNode> parseTerm();
    std::unique_ptr<ExprNode> parseFactor();
    std::unique_ptr<ExprNode> parsePrimary();
};

#endif // PARSER_H
