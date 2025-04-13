#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <memory>
#include <iostream>

using namespace std;

inline void printIndent(ostream &out, int indent) {
    for (int i = 0; i < indent; i++)
        out << "  ";
}

// 基类：抽象语法树节点
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print(ostream &out, int indent = 0) const = 0;
};

//==========================
// 表达式节点及其派生类
//==========================

class ExprNode : public ASTNode {};

// 字面量表达式节点（数字常量）
class LiteralExprNode : public ExprNode {
public:
    string value;
    LiteralExprNode(const string &val) : value(val) {}
    void print(ostream &out, int indent = 0) const override {
        printIndent(out, indent);
        out << "Literal: " << value << "\n";
    }
};

// 标识符表达式节点（变量引用）
class IdentifierExprNode : public ExprNode {
public:
    string name;
    IdentifierExprNode(const string &n) : name(n) {}
    void print(ostream &out, int indent = 0) const override {
        printIndent(out, indent);
        out << "Identifier: " << name << "\n";
    }
};

// 二元表达式节点（例如加法、赋值等）
class BinaryExprNode : public ExprNode {
public:
    string op;
    unique_ptr<ExprNode> left;
    unique_ptr<ExprNode> right;
    BinaryExprNode(string op, unique_ptr<ExprNode> left, unique_ptr<ExprNode> right)
        : op(op), left(move(left)), right(move(right)) {}
    void print(ostream &out, int indent = 0) const override {
        printIndent(out, indent);
        out << "BinaryExpr: " << op << "\n";
        if (left)  left->print(out, indent + 1);
        if (right) right->print(out, indent + 1);
    }
};

//==========================
// 语句节点及其派生类
//==========================

class StmtNode : public ASTNode {};

// 表达式语句节点
class ExprStmtNode : public StmtNode {
public:
    unique_ptr<ExprNode> expr;
    ExprStmtNode(unique_ptr<ExprNode> e) : expr(move(e)) {}
    void print(ostream &out, int indent = 0) const override {
        printIndent(out, indent);
        out << "ExprStmt:\n";
        if (expr)
            expr->print(out, indent + 1);
    }
};

// if 语句节点
class IfStmtNode : public StmtNode {
public:
    unique_ptr<ExprNode> condition;
    unique_ptr<StmtNode> thenStmt;
    unique_ptr<StmtNode> elseStmt; // 可选
    void print(ostream &out, int indent = 0) const override {
        printIndent(out, indent);
        out << "IfStmt:\n";
        printIndent(out, indent + 1);
        out << "Condition:\n";
        if (condition)
            condition->print(out, indent + 2);
        printIndent(out, indent + 1);
        out << "Then:\n";
        if (thenStmt)
            thenStmt->print(out, indent + 2);
        if (elseStmt) {
            printIndent(out, indent + 1);
            out << "Else:\n";
            elseStmt->print(out, indent + 2);
        }
    }
};

// while 语句节点
class WhileStmtNode : public StmtNode {
public:
    unique_ptr<ExprNode> condition;
    unique_ptr<StmtNode> body;
    void print(ostream &out, int indent = 0) const override {
        printIndent(out, indent);
        out << "WhileStmt:\n";
        printIndent(out, indent + 1);
        out << "Condition:\n";
        if (condition)
            condition->print(out, indent + 2);
        printIndent(out, indent + 1);
        out << "Body:\n";
        if (body)
            body->print(out, indent + 2);
    }
};

// 复合语句（块语句）节点
class BlockStmtNode : public StmtNode {
public:
    vector<unique_ptr<StmtNode>> stmts;
    void print(ostream &out, int indent = 0) const override {
        printIndent(out, indent);
        out << "BlockStmt:\n";
        for (const auto &stmt : stmts) {
            stmt->print(out, indent + 1);
        }
    }
};

// read 语句节点
class ReadStmtNode : public StmtNode {
public:
    string varName;
    ReadStmtNode(const string &name) : varName(name) {}
    void print(ostream &out, int indent = 0) const override {
        printIndent(out, indent);
        out << "ReadStmt: " << varName << "\n";
    }
};

// write 语句节点
class WriteStmtNode : public StmtNode {
public:
    string varName;
    WriteStmtNode(const string &name) : varName(name) {}
    void print(ostream &out, int indent = 0) const override {
        printIndent(out, indent);
        out << "WriteStmt: " << varName << "\n";
    }
};

//==========================
// 其他节点
//==========================

// 声明节点：例如 int a, b; 或 bool flag;
class DeclNode : public ASTNode {
public:
    string type;              // "int" 或 "bool"
    vector<string> names;     // 变量名列表
    void print(ostream &out, int indent = 0) const override {
        printIndent(out, indent);
        out << "Decl: " << type << " ";
        for (const auto &name : names) {
            out << name << " ";
        }
        out << "\n";
    }
};

// 用于表示函数参数
struct Parameter {
    string type;        // 参数类型，如 "int" 或 "bool"
    string name;        // 参数名
    string defaultVal;  // 默认值（如果有），否则为空
};

// 函数定义节点
class FuncDefNode : public ASTNode {
public:
    string returnType;                // 返回类型，如 "int" 或 "bool"
    string name;                      // 函数名
    vector<Parameter> params;         // 参数列表
    unique_ptr<BlockStmtNode> body;   // 函数体（块语句）
    void print(ostream &out, int indent = 0) const override {
        printIndent(out, indent);
        out << "FuncDef: " << returnType << " " << name << "\n";
        printIndent(out, indent + 1);
        out << "Parameters:\n";
        for (const auto &param : params) {
            printIndent(out, indent + 2);
            out << param.type << " " << param.name;
            if (!param.defaultVal.empty())
                out << " = " << param.defaultVal;
            out << "\n";
        }
        printIndent(out, indent + 1);
        out << "Body:\n";
        if (body)
            body->print(out, indent + 2);
    }
};

// 程序节点：包括函数定义、声明和语句
class ProgramNode : public ASTNode {
public:
    vector<unique_ptr<FuncDefNode>> functions; // 函数定义
    vector<unique_ptr<ASTNode>> decls;           // 全局声明（可选）
    vector<unique_ptr<ASTNode>> stmts;           // 全局执行语句（可选）
    void print(ostream &out, int indent = 0) const override {
        printIndent(out, indent);
        out << "Program\n";
        if (!functions.empty()) {
            printIndent(out, indent + 1);
            out << "Functions:\n";
            for (const auto &func : functions) {
                func->print(out, indent + 2);
            }
        }
        if (!decls.empty()) {
            printIndent(out, indent + 1);
            out << "Declarations:\n";
            for (const auto &decl : decls) {
                decl->print(out, indent + 2);
            }
        }
        if (!stmts.empty()) {
            printIndent(out, indent + 1);
            out << "Statements:\n";
            for (const auto &stmt : stmts) {
                stmt->print(out, indent + 2);
            }
        }
    }
};

#endif // AST_H
