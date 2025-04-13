#include "../include/lexer.h"
#include <unordered_set>
#include <cctype>

namespace {
    // 关键字集合，新增 "function"
    const unordered_set<string> KEYWORDS = {"if", "else", "while", "int", "bool", "read", "write", "then", "do", "function"};
    // 操作符集合
    const unordered_set<string> OPERATORS = {"+", "-", "*", "/", "=", ":=", "==", "!=", "<", "<=", ">", ">=", "&&", "||", "!"};
    // 分隔符集合
    const unordered_set<char> DELIMITERS = {';', ',', '(', ')', '{', '}', ':'};
}


void Lexer::handleComment(size_t &pos, const string &source) {
    if (pos + 1 >= source.size()) return;
    if (source[pos + 1] == '/') { // 行注释
        pos += 2;
        while (pos < source.size() && source[pos] != '\n')
            pos++;
    } else if (source[pos + 1] == '*') { // 块注释
        pos += 2;
        while (pos < source.size()) {
            if (source[pos] == '*' && pos + 1 < source.size() && source[pos + 1] == '/'){
                pos += 2;
                return;
            }
            pos++;
        }
    }
}

bool Lexer::isKeyword(const string &lexeme) {
    return KEYWORDS.count(lexeme) > 0;
}

bool Lexer::isDelimiter(char c) {
    return DELIMITERS.count(c) > 0;
}

bool Lexer::isOperatorChar(char c) {
    string op_chars = "+-*/=!<>&|:";
    return op_chars.find(c) != string::npos;
}

Token Lexer::handleIdentifier(size_t &pos, const string &source) {
    size_t start = pos;
    while (pos < source.size() && (isalnum(source[pos]) || source[pos] == '_'))
        pos++;
    string lexeme = source.substr(start, pos - start);
    if (isKeyword(lexeme))
        return {TokenType::KEYWORD, lexeme};
    else
        return {TokenType::IDENTIFIER, lexeme};
}

Token Lexer::handleNumber(size_t &pos, const string &source) {
    size_t start = pos;
    bool hasDot = false;
    while (pos < source.size() && (isdigit(source[pos]) || source[pos] == '.')) {
        if (source[pos] == '.') {
            if (hasDot) break;
            hasDot = true;
        }
        pos++;
    }
    string lexeme = source.substr(start, pos - start);
    if (hasDot)
        return {TokenType::FLOAT, lexeme};
    else
        return {TokenType::INTEGER, lexeme};
}

Token Lexer::handleOperator(size_t &pos, const string &source) {
    string op;
    op.push_back(source[pos]);
    pos++;
    // 尝试识别两字符操作符
    if (pos < source.size()) {
        string op2 = op;
        op2.push_back(source[pos]);
        if (OPERATORS.count(op2)) {
            pos++;
            return {TokenType::OPERATOR, op2};
        }
    }
    if (OPERATORS.count(op))
        return {TokenType::OPERATOR, op};
    return {TokenType::ERROR, op};
}


Token Lexer::handleDelimiter(size_t &pos, const string &source) {
    char c = source[pos];
    pos++;
    return {TokenType::DELIMITER, string(1, c)};
}

Token Lexer::handleError(size_t &pos, const string &source) {
    string err(1, source[pos]);
    pos++;
    return {TokenType::ERROR, err};
}

vector<Token> Lexer::tokenize(const string &source) {
    vector<Token> tokens;
    size_t pos = 0;
    while (pos < source.size()) {
        if (isspace(source[pos])) {
            pos++;
            continue;
        }
        if (source[pos] == '/') {
            if (pos + 1 < source.size() && (source[pos + 1] == '/' || source[pos + 1] == '*')) {
                handleComment(pos, source);
                continue;
            }
        }
        if (isalpha(source[pos]) || source[pos] == '_') {
            tokens.push_back(handleIdentifier(pos, source));
            continue;
        }
        if (isdigit(source[pos])) {
            tokens.push_back(handleNumber(pos, source));
            continue;
        }
        if (isDelimiter(source[pos])) {
            tokens.push_back(handleDelimiter(pos, source));
            continue;
        }
        if (isOperatorChar(source[pos])) {
            tokens.push_back(handleOperator(pos, source));
            continue;
        }
        tokens.push_back(handleError(pos, source));
    }
    tokens.push_back({TokenType::END, ""});
    return tokens;
}
