#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
#include "lexer.h"
#include "parser.h"

#define inputDir "./IO/testCases/" // 请修改为实际的源代码目录
#define outputDir "./IO/output/"   // 请修改为实际的输出目录

using namespace std;
namespace fs = std::filesystem;

vector<string> FileQueue() {
    vector<string> file_list;
    try {
        for (const auto &entry : fs::directory_iterator(inputDir)) {
            if (entry.is_regular_file())
                file_list.push_back(entry.path().filename().string());
        }
    } catch (const fs::filesystem_error &e) {
        throw runtime_error("目录访问失败: " + string(e.what()));
    }
    return file_list;
}

bool writeASTToFile(const ASTNode &ast, const string &filename) {
    ofstream out(filename);
    if (!out) {
        cout << "无法打开输出文件：" << filename << endl;
        return false;
    }
    ast.print(out);
    return true;
}

int main() {
    try {
        vector<string> fileList = FileQueue();
        for (const auto &file : fileList) {
            string currentFileName = string(inputDir) + file;
            string currentOutput = string(outputDir) + file; // 输出文件名与输入文件相同
            cout << "当前文件: " << file << endl;
            ifstream in(currentFileName);
            if (!in) {
                cout << "无法找到输入文件: " << file << endl;
                continue;
            }
            stringstream buffer;
            buffer << in.rdbuf();
            string source = buffer.str();

            // 词法分析
            cout << "正在词法分析..." << endl;
            Lexer lexer;
            auto tokens = lexer.tokenize(source);
            cout << "词法分析完成, 开始语法分析..." << endl;

            // 语法分析
            Parser parser(tokens);
            unique_ptr<ProgramNode> ast;
            try {
                ast = parser.parseProgram();
            } catch (const exception &e) {
                // 将错误信息写入输出文件
                ofstream errOut(currentOutput);
                if (errOut) {
                    errOut << "语法分析错误: " << e.what() << "\n";
                }
                cout << "语法分析错误: " << e.what() << endl;
                continue;
            }
            cout << "语法分析完成" << endl;
            // 输出 AST 到文件
            if (writeASTToFile(*ast, currentOutput))
                cout << "结果已写入: " << currentOutput << "\n\n";
            else
                cout << "写入文件失败" << endl;
        }
        cout << "所有文件处理完成，请到输出文件夹查看结果" << endl;
    } catch (const exception &e) {
        cerr << "错误: " << e.what() << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
