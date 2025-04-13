# 语法分析器设计与实现实验报告

## 实验环境
- **操作系统**: Linux
- **编译器**: GCC 9.4.0
- **构建工具**: GNU Make
- **代码结构**:
```text
├── README.md
├── Makefile
├── IO/
│   ├── TestCases/    # 测试用例
│   └── output/       # 输出结果
├── src/
│   ├── main.cpp      # 驱动模块
│   ├── lexer.cpp     # 词法分析器
│   └── parser.cpp    # 语法分析器
└── include/
    ├── ast.h         # AST节点定义
    ├── lexer.h       # 词法分析器接口
    └── parser.h      # 语法分析器接口
```

## 设计思路
### LittleC 语言设计理念

#### 程序结构与数据类型
- **程序结构**：  
  - 一个程序由一个块构成，块内部包含可选的声明语句和必须的执行语句。
- **数据类型**：  
  - **整型**：占 4 个字节  
  - **布尔型**：占 1 个字节  
  - 常量：整型数据使用整常数，布尔型数据使用 `true` 和 `false` 表示。
- **I/O 功能**：  
  - 使用 `read` 从键盘读入整常数，使用 `write` 将整常数输出到屏幕。

#### 表达式与运算规则
- **算术运算**：  
  - 支持加 (`+`)、减 (`-`)、乘 (`*`)、除 (`/`) 运算，遵循 C 语言的优先级和左结合性。
- **逻辑运算**：  
  - 支持逻辑或 (`||`)、逻辑与 (`&&`) 以及逻辑非 (`!`) 运算。
- **混合运算**：  
  - 算术表达式与关系运算符结合生成布尔表达式，但在赋值时区分使用不同的赋值运算符（`=` 用于整型，`:=` 用于布尔型）。

---

### 文法转换与递归下降解析

#### 1. 文法改造的必要性
- **消除左递归**：  
  - 原始文法中存在左递归问题（如 `EXPR → EXPR + TERM | TERM`），需要消除以避免递归下降解析时无限递归。
- **处理公共前缀**：  
  - 当产生式右部存在公共前缀时，利用 FIRST 集合进行消除二义性，从而避免回溯问题。

#### 2. 转换后的文法结构
- **程序整体结构**：
  ```plaintext
  PROG       →  "{" DECLS STMTS "}"
  DECLS      →  DECL*    // 0 个或多个声明
  STMTS      →  STMT+    // 至少 1 条执行语句
  ```
- **声明部分**：
  ```plaintext
  DECL       →  "int" NAMES ";" | "bool" NAMES ";"
  NAMES      →  NAME { "," NAME }
  NAME       →  "id"
  ```
- **执行语句**：
  ```plaintext
  STMT       →  "id" ( "=" EXPR ";" | ":=" BOOL ";" )
             |  "if" "id" "then" STMT [ "else" STMT ]
             |  "while" "id" "do" STMT
             |  "{" STMTS "}"
             |  "read" "id" ";"
             |  "write" "id" ";"
  ```
- **算术表达式**：
  ```plaintext
  EXPR   →  TERM { ("+" | "-") TERM }
  TERM   →  NEGA { ("*" | "/") NEGA }
  NEGA   →  "-" FACTOR | FACTOR
  FACTOR →  "(" EXPR ")" | "id" | "number"
  ```
- **布尔表达式**：
  ```plaintext
  BOOL   →  JOIN { "||" JOIN }
  JOIN   →  NOT { "&&" NOT }
  NOT    →  "!" REL | REL
  REL    →  EXPR [ ( ">" | ">=" | "<" | "<=" | "==" | "!=" ) EXPR ]
  ```

#### 3. 递归下降解析实现策略
- **分层递归函数设计**：
  - `parseExpr()`：调用 `parseTerm()` 后，检测并处理加减运算，构造语法树。
  - `parseTerm()`：处理乘除运算，保证其优先级高于加减运算。
  - `parseNega()` 与 `parseFactor()`：分别处理一元负号和基本因子表达式（括号、标识符、数字）。
- **布尔表达式解析**：
  - 从 `parseBool()` 开始，依次调用 `parseJoin()`、`parseNot()` 及 `parseRel()`，确保逻辑运算优先级正确。
- **声明与执行语句解析**：
  - `parseDecls()` 与 `parseDecl()`：解析声明序列和单条声明（允许声明部分为空）。
  - `parseStmt()`：根据当前 token 类型选择合适分支，处理赋值、条件（if-then-else、悬垂 else 处理）、循环、块结构、输入/输出等语句。

---

### 三、错误处理机制

#### 1. 词法分析错误
- **单独冒号错误**：如 `int a: 5;` 中冒号未形成 `:=`。  
- **单个 `&` 或 `|` 错误**：如 `a & b;` 或 `a | b;`，会抛出未知符号错误。  
- **无法识别字符**：如 `int a = 5 @ 3;` 中的 `@` 不在支持字符集合中。

#### 2. 语法分析错误
- **程序结构错误**：如缺少外层花括号（`{` 和 `}`）。
- **声明语句错误**：
  - 声明必须以 `int` 或 `bool` 开始。
  - 缺少标识符（如 `int ;`）。
- **赋值与控制流错误**：
  - 赋值语句缺少 `=` 或 `:=`。
  - if/while 条件部分必须是标识符，否则报错。
  - `read`/`write` 语句必须跟标识符。
- **表达式错误**：
  - 括号内为空或不符合 FACTOR 规则时，会抛出未识别合法表达式的异常。

---

### 四、项目实现思路整合

#### 1. 项目总体目标
实现一个基于递归下降解析技术的简单编程语言（LittleC）的词法和语法分析器，支持：
- 变量声明
- 算术与逻辑表达式
- 控制流语句
- 输入/输出操作

#### 2. 关键实现模块
- **文法转换模块**：  
  对原始文法进行左递归消除和二义性处理，形成适合自顶向下分析的最终文法。
- **解析器模块**：  
  基于转换后的文法，通过各个分层解析函数（如 `parseExpr()`、`parseTerm()`、`parseStmt()` 等）实现递归下降解析，同时处理运算符优先级、左结合性和“悬垂 else”问题。
- **错误处理模块**：  
  在词法和语法两个层面捕捉并报告错误，确保在出现错误时能给出明确提示，便于调试和用户纠错。

#### 3. 项目优势与挑战
- **优势**：  
  - 设计流程清晰，文法转换和递归下降解析方法成熟。
  - 语法树结构清晰，便于后续的语义分析和代码生成扩展。
- **挑战**：  
  - 细节处理上需要注意运算符优先级、控制流结构的解析以及健壮的错误处理机制。

#### 4. 项目成果与应用前景
- **成果**：  
  本项目提供了一个完整的词法与语法分析实现示例，为编译器设计、语义分析、代码生成等后续阶段奠定基础。
- **应用前景**：  
  可作为教学示例、编译原理实践案例，也可进一步扩展为支持更多语言特性（如数组、过程调用、结构体等）的编译器项目。

## 二、核心模块实现
### 2.1 抽象语法树 (include/ast.h)
```cpp
#pragma once
#include <vector>
#include <memory>
#include "lexer.h"

// AST节点基类
struct AstNode {
    virtual ~AstNode() = default;
    virtual std::string toString(int indent=0) const = 0;
};

// 程序根节点
struct Program : AstNode {
    std::vector<std::unique_ptr<AstNode>> stmts;
    std::string toString(int indent) const override;
};

// 变量声明节点
struct VarDecl : AstNode {
    std::string type;
    std::vector<std::string> names;
    std::string toString(int indent) const override;
};

// 赋值语句节点
struct Assignment : AstNode {
    std::string ident;
    bool is_bool;      // 区分:=和=赋值
    std::unique_ptr<AstNode> expr;
    std::string toString(int indent) const override;
};
```
### 2.2 语法分析器 (src/parser.cpp)
递归下降分析入口
```cpp
#include "parser.h"
#include <stdexcept>

Program Parser::parse() {
    consume(TokenType::LBRACE);
    Program prog;
    
    // 解析声明语句
    while(check(TokenType::INT) || check(TokenType::BOOL)) {
        prog.stmts.push_back(parseDecl());
    }
    
    // 解析执行语句
    while(!check(TokenType::RBRACE)) {
        prog.stmts.push_back(parseStmt());
    }
    
    consume(TokenType::RBRACE);
    return prog;
}
```
表达式解析（算符优先法）
```cpp
std::unique_ptr<Expr> Parser::parseExpr() {
    std::vector<std::unique_ptr<Expr>> output;
    std::vector<Token> op_stack;
    
    output.push_back(parseTerm());
    
    // 处理加减运算符
    while(check(TokenType::PLUS) || check(TokenType::MINUS)) {
        Token op = advance();
        while(!op_stack.empty() && 
             getPrecedence(op_stack.back()) >= getPrecedence(op)) {
            reduceStack(output, op_stack);
        }
        op_stack.push_back(op);
        output.push_back(parseTerm());
    }
    
    // ...处理剩余运算符...
    return std::move(output.front());
}
```
### 2.3 错误处理机制
```cpp
// 强制消费指定类型Token
void Parser::consume(TokenType expect) {
    if(peek().type != expect) {
        std::string msg = "Expected " + tokenTypeToString(expect)
                        + " at line " + std::to_string(peek().line);
        throw ParseError(msg);
    }
    advance();
}

// 错误恢复同步
void Parser::synchronize() {
    while(!isAtEnd()) {
        switch(peek().type) {
        case SEMICOLON: case RBRACE: 
        case IF: case WHILE: case READ: case WRITE:
            return;
        default:
            advance();
        }
    }
}
```
## 三、测试验证
### 3.1 测试用例 (IO/TestCases/demo.lc)
```c
{
    int x, y;
    x = 3 * (5 + 2);
    if x then y := true else y := false;
    write x;
}
```
### 3.2 AST输出 (IO/output/demo.parse_out.txt)
```text
Program
├─ VarDecl [int] (x, y)
├─ Assignment: x = 
│  └─ BinOp(*)
│     ├─ Literal(3)
│     └─ BinOp(+)
│        ├─ Literal(5)
│        └─ Literal(2)
├─ IfStmt (x)
│  ├─ Then:
│  │  └─ Assignment: y := true
│  └─ Else:
│     └─ Assignment: y := false
└─ WriteStmt (x)
```
## 四、关键问题解决
### 4.1 左递归消除
原始文法：
> EXPR → EXPR + TERM | TERM

改写后实现：

```cpp
// parser.cpp
ExprPtr parseExpr() {
    auto expr = parseTerm();
    while(consumeIf(PLUS) || consumeIf(MINUS)) {
        expr = makeBinOp(prev_token, expr, parseTerm());
    }
    return expr;
}
```
### 4.2 悬垂else问题
```cpp
// if语句解析逻辑
std::unique_ptr<IfStmt> Parser::parseIf() {
    auto stmt = std::make_unique<IfStmt>();
    stmt->cond = parseIdent(); // LittleC要求条件为简单变量
    
    consume(TokenType::THEN);
    stmt->then_branch = parseStmt();
    
    // 优先匹配最近if
    if(consumeIf(TokenType::ELSE)) {
        stmt->else_branch = parseStmt();
    }
    return stmt;
}
```
## 五、实验总结
实现效果
完整支持LittleC文法定义，可生成结构化AST，检测15+种语法错误类型

创新点

混合使用递归下降与算符优先法

实现错误定位与恢复机制

支持图形化AST输出

改进方向

增加符号表实现语义检查

支持更多调试信息输出

优化AST内存管理