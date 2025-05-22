#include "jsonparser.hpp"
#include <fstream>
#include <sstream>
using namespace std;

JSONNode* json = nullptr;
size_t pos = 0;

// 读取文件内容 注意路径中不能有中文
string readFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Could not open file: " + filename);
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool endwithjson(const string& str) {
    return str.length() >= 5 && str.substr(str.length() - 5) == ".json";  // 判断是否以.json结尾
}

void JSONfromFile(const string& filename) {
    string content = readFile(filename);  // 读取文件内容
    pos = 0;
    delete json;  // 释放之前的json对象
    json = JSONNode::parse(content, pos);  // 解析JSON内容
}

void JSONtoFile(const string& filename) {
    ofstream outputFile(filename);
    if (!outputFile.is_open()) {
        throw runtime_error("Could not open output file: " + filename);
    } else {
        json->print(outputFile, 0, false);
        outputFile.close();
    }
}

void skipspace(const string& str, size_t& pos) {
    while (pos < str.length() && isspace(str[pos])) ++pos;  // 跳过空格
}

int main(int argc, char* argv[]) {
    // 传入0个参数说明命令模式；一个参数(argc=2)说明预读取
    if (argc <= 2) {
        if (argc == 2) JSONfromFile(argv[1]);
        // 进入命令调用模式
        while (1) {
            try {
                string command;
                cout << "\033[36m$\033[0m ";
                getline(cin, command);  // 没有\n
                // 删除开头和结尾的空格
                command.erase(0, command.find_first_not_of(" \t"));
                command.erase(command.find_last_not_of(" \t") + 1);
                if (command.length() == 0) continue;
                if (command == "exit") break;
                if (command == "help") {
                    cout << "\033[35mCommands:\033[0m\n"
                         << "  \033[34mexit:\033[0m Exit the program\n"
                         << "  \033[34mhelp:\033[0m Show this help message\n"
                         << "  \033[34mjson[key]...:\033[0m Print that level briefly\n"
                         << "  \033[34mjson[key]... all:\033[0m Print recursively in detail\n"
                         << "  \033[34mfile.json:\033[0m Read json from file.json\n"
                         << "  \033[34mwrite file.json:\033[0m Write formatted json to file.json\n"
                         << "\n\033[35mor call the program with two arguments:\033[0m\n"
                         << "  \033[34m./jsonparser.exe input.json output.json:\033[0m Read json from input.json, format, and write to output.json\n";
                    continue;
                }
                if (command.length() > 5 && command.substr(0, 5) == "write") {
                    if (json == nullptr) throw runtime_error("No JSON data loaded. Please load a JSON file first.");
                    pos = 5;
                    skipspace(command, pos);  // 跳过中间的空格
                    if (pos >= command.length()) throw runtime_error("No file specified after 'write' command.");
                    JSONtoFile(command.substr(pos));
                    continue;
                }
                // json开头的指令。如果以.json结尾（对于json.json）则说明是写文件
                if (command.length() >= 4 && command.substr(0, 4) == "json" && !endwithjson(command)) {
                    if (json == nullptr) throw runtime_error("No JSON data loaded. Please load a JSON file first.");
                    pos = 4;
                    bool all = false;
                    if (command.length() > 4 && command.substr(command.length() - 4) == " all") {
                        all = true;
                        command.erase(command.length() - 4);  // 删除最后的" all"
                    }
                    JSONNode* p = json;
                    while (pos < command.length()) {
                        skipspace(command, pos);
                        if (command[pos] != '[') throw runtime_error("Invalid command format. Expected 'json[key]...'.");
                        ++pos;  // 跳过'['
                        skipspace(command, pos);
                        if (pos >= command.length()) throw runtime_error("Unterminated command at position " + to_string(pos) + ", expected ']'");
                        // 方括号里的可能是字符串或者数字。字符串可以省略引号，为此需要先判断当前数据结构类型
                        if (p->getType() == JSONType::OBJECT) {
                            if (command[pos] != '"') {
                                size_t start = pos;
                                while (pos < command.length() && !(isspace(command[pos]) || command[pos] == ']')) ++pos;  // 找到空格或者']'
                                string key = command.substr(start, pos - start);  // 提取键
                                cout << "\033[33mWarning: Key(" << key << ") is not wrapped with quotes. Assuming it's a string.\033[0m\n";  // 黄色输出警告信息
                                p = (*(JSONObject*)p)[key];
                            } else {
                                string key = JSONString::parseString(command, pos);  // 解析字符串
                                p = (*(JSONObject*)p)[key];
                            }
                        } else if (p->getType() == JSONType::ARRAY) {
                            if (!isdigit(command[pos])) throw runtime_error("Invalid command format. Expected a number.");
                            size_t index = 0;
                            while (pos < command.length() && isdigit(command[pos])) {
                                index = index * 10 + (command[pos] - '0');
                                ++pos;
                            }
                            p = (*(JSONArray*)p)[index];  // 访问数组元素
                        } else {
                            throw runtime_error("Cannot index into a non-object or non-array type: " + command.substr(0, pos));
                        }
                        // 处理结束的']'
                        skipspace(command, pos);
                        if (pos >= command.length()) throw runtime_error("Unterminated command at position " + to_string(pos) + ", expected ']'");
                        if (command[pos] != ']') throw runtime_error("Invalid command format. Expected ']'.");
                        ++pos;  // 跳过']'
                    }
                    if (all) {
                        p->print(cout, 0, true);  // 递归打印
                    } else {
                        p->abstruct(cout, 0);  // 打印抽象结构
                    }
                    cout << '\n';
                    continue;
                }
                // 视为一个文件，直接读取 不一定要以.json结尾
                JSONfromFile(command);
            } catch (const exception& e) {
                cout << "\033[31mError: " << e.what() << "\033[0m\n";  // 红色输出错误信息
                continue;
            }
        }
    } else if (argc == 3) {
        // 根据参数进行一次性的调用
        // 第一个参数是输入路径，第二个参数是输出路径，作用是格式化输出
        JSONfromFile(argv[1]);
        JSONtoFile(argv[2]);
    } else {
        cout << "\033[31mError: Too many arguments. Try 'help' for more information.\033[0m\n";  // 红色输出错误信息
    }
    delete json;
    return 0;
}