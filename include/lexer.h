#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

enum class TokenType
{
    KEYWORD,
    IDENTIFIER,
    INTEGER,
    FLOAT,
    OPERATOR,
    DELIMITER,
    END,    // 文件结束标记
    ERROR
};

struct Token
{
    TokenType type;
    string lexeme;
};

class Lexer
{
public:
    vector<Token> tokenize(const string &source);

private:
    void handleComment(size_t &pos, const string &source);
    bool isKeyword(const string &lexeme);
    bool isDelimiter(char c);
    bool isOperatorChar(char c);
    Token handleIdentifier(size_t &pos, const string &source);
    Token handleNumber(size_t &pos, const string &source);
    Token handleOperator(size_t &pos, const string &source);
    Token handleDelimiter(size_t &pos, const string &source);
    Token handleError(size_t &pos, const string &source);
};

#endif // LEXER_H
