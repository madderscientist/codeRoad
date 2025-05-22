/**
 * @file jsonparser.hpp
 * @brief JSON解析器头文件
 * @author Ruigang Li
 * @date 2025-4-26
 * @version 1.0
 * @details 匆忙的实现；对JSON格式进行了一定拓展；用于示例
 */
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

#define SPACE '\t'

enum class JSONType {
    OBJECT,
    ARRAY,
    STRING,
    NUMBER,
    BOOLEAN,
    NULL_TYPE
};

// 前向声明
class JSONNode;
class JSONNumber;
class JSONBoolean;
class JSONString;
class JSONNull;
class JSONObject;
class JSONArray;

#define RESET "\033[0m"
#define RED "\033[31m"  // Null
#define GREEN "\033[32m"  // Number
#define YELLOW "\033[33m"  // Object
#define BLUE "\033[34m"  // String
#define MAGENTA "\033[35m"  // Array
#define CYAN "\033[36m"  // Bool
#define WHITE "\033[37m"

class JSONNode {
public:
    static const JSONNull* jsonNull;
    static const bool IGNORE_ERROR = false;  // 是否忽略错误
    JSONType type;
    JSONNode(JSONType t) : type(t) {}
    ~JSONNode() = default;
    JSONType getType() const { return type; }
    // 索引
    JSONNode* operator[](const string& key) {
        if (JSONNode::IGNORE_ERROR) return (JSONNode*)(jsonNull);
        throw runtime_error("Cannot use operator[] on JSONNode of type " + to_string((int)type));
    }
    const JSONNode* operator[](const string& key) const {
        if (JSONNode::IGNORE_ERROR) return (JSONNode*)(jsonNull);
        throw runtime_error("Cannot use operator[] on JSONNode of type " + to_string((int)type));
    }
    JSONNode* operator[](size_t index) {
        if (JSONNode::IGNORE_ERROR) return (JSONNode*)(jsonNull);
        throw runtime_error("Cannot use operator[] on JSONNode of type " + to_string((int)type));
    }
    const JSONNode* operator[](size_t index) const {
        if (JSONNode::IGNORE_ERROR) return (JSONNode*)(jsonNull);
        throw runtime_error("Cannot use operator[] on JSONNode of type " + to_string((int)type));
    }
    /**
     * 递归输出所有层级
     * @param os 输出流
     * @param level 缩进级别
     * @param console 是否在控制台输出 是的话会有颜色 对于文件输出此项应该为false
     * @return 输出流的引用，便于链式调用
     */
    virtual ostream& print(ostream& os, unsigned int level = 0, bool console = false) const;
    /**
     * 抽象输出，主要用于调试和查看结构。Array和Object会简略输出
     * 肯定只能在控制台输出，所以有颜色
     */
    ostream& abstruct(ostream& os, unsigned int level = 0) const;

    /**
     * 跳过空格（包括换行和制表符）和注释 注意，注释不是json标准的一部分
     * @param str JSON字符串
     * @param pos 当前解析位置的引用 会被修改
     */
    static void skipSpace(const string& str, size_t& pos) {
        while (pos < str.length() && isspace(str[pos])) ++pos;  // 跳过空格
        if (pos >= str.length() - 1) return;  // 解析结束
        // 注释至少需要两个字符："//"或"/*"
        if (str[pos] == '/') {
            const char nextChar = str[pos + 1];
            if (nextChar == '/') {  // 单行注释
                pos += 2;  // 跳过"//"
                while (pos < str.length() && str[pos] != '\n') ++pos;  // 跳过到换行符
            } else if (nextChar == '*') {  // 多行注释
                pos += 2;  // 跳过"/*"
                while (pos < str.length()) {
                    if (str[pos] == '*' && str[pos + 1] == '/') {  // 找到结束标记
                        pos += 2;  // 跳过"*/"
                        break;
                    }
                    ++pos;  // 跳过注释内容
                }
            } else
                return;  // 不是注释，返回
            while (pos < str.length() && isspace(str[pos])) ++pos;  // 再次跳过空格
        }
    }
    /**
     * 解析JSON字符串的总入口，负责判断顶层类型并调用相应的解析函数
     * @param str JSON字符串
     * @param pos 当前解析位置的引用 会被修改
     * @return 解析后的JSONNode指针
     */
    static JSONNode* parse(const string& str, size_t& pos);
};

// 下面四个是简单的数据类型 都没有缩进
class JSONNumber : public JSONNode {
public:
    double value;
    JSONNumber(double v) : JSONNode(JSONType::NUMBER), value(v) {}
    ostream& print(ostream& os, unsigned int level = 0, bool console = false) const {
        if (console) return os << GREEN << value << RESET;  // 输出绿色
        return os << value;  // 默认输出
    }
    ostream& abstruct(ostream& os, unsigned int level = 0) const {
        return os << GREEN << value << RESET;
    }
    static double parseNumber(const string& str, size_t& pos) {
        size_t endPos = str.find_first_not_of("0123456789.-", pos);
        if (endPos == string::npos) endPos = str.length();  // 说明数字到末尾
        string numberStr = str.substr(pos, endPos - pos);
        size_t stod_finish_at = 0;  // 因为stod会忽略末尾的非数字 所以要检查整个字符串是否都被解析
        double value{};
        try {
            value = stod(numberStr, &stod_finish_at);
        } catch (const invalid_argument& e) {
            // 由于JSONNode::parse中以number为结尾处理所有语法错误，而抛出invalid_argument异常大概就是语法有问题
            throw runtime_error("Incorrect JSON syntax at position " + to_string(pos) + ": '" + str[pos] + "'");
        }
        if (stod_finish_at != numberStr.length())
            throw runtime_error("Invalid number at position " + to_string(pos) + ", expected a valid number");
        pos = endPos;
        return value;
    }
    /**
     * 解析数字 支持整数和小数
     * @param str JSON字符串
     * @param pos 当前解析位置的引用 会被修改
     * @return 解析后的JSONNumber指针
     */
    static JSONNumber* parse(const string& str, size_t& pos) {
        return new JSONNumber(JSONNumber::parseNumber(str, pos));
    }
};

class JSONBoolean : public JSONNode {
public:
    bool value;
    JSONBoolean(bool v) : JSONNode(JSONType::BOOLEAN), value(v) {}
    ostream& print(ostream& os, unsigned int level = 0, bool console = false) const {
        if (console) return os << CYAN << (value ? "true" : "false") << RESET;  // 输出青色
        return os << (value ? "true" : "false");
    }
    ostream& abstruct(ostream& os, unsigned int level = 0) const {
        return os << CYAN << (value ? "true" : "false") << RESET;
    }
    static JSONBoolean* parse(const string& str, size_t& pos) {
        if (str.substr(pos, 4) == "true") {
            pos += 4;
            return new JSONBoolean(true);
        } else if (str.substr(pos, 5) == "false") {
            pos += 5;
            return new JSONBoolean(false);
        }
        throw runtime_error("Invalid boolean value at position " + to_string(pos) + ", expected 'true' or 'false'");
    }
};

class JSONString : public JSONNode {
public:
    static size_t maxDisplayLength;  // 最大显示长度
    string value;
    string literal;  // 转义字符全部字面化，用于文本输出
    static string literally(const string& str) {
        string result;
        result.reserve(str.length() + 2);  // 设置缓存大小略大于str的长度
        for (char c : str) {
            switch (c) {
                case '\\':
                    result += "\\\\";  // 转义斜杠
                    break;
                case '"':
                    result += "\\\"";  // 转义双引号
                    break;
                case '\n':
                    result += "\\n";  // 转义换行符
                    break;
                case '\t':
                    result += "\\t";  // 转义制表符
                    break;
                case '\r':
                    result += "\\r";  // 转义回车符
                    break;
                case '\b':
                    result += "\\b";  // 转义退格符
                    break;
                case '\f':
                    result += "\\f";  // 转义换页符
                    break;
                default:
                    result += c;  // 普通字符
            }
        }
        return result;
    }
    /**
     * 构造函数，传入字符串值，应该是不包含转义的原始内容。本函数会据此生成有转义字符的字面值
     * @param v 字符串值
     */
    JSONString(const string& v) : JSONNode(JSONType::STRING), value(v), literal(JSONString::literally(v)) {}
    ostream& print(ostream& os, unsigned int level = 0, bool console = false) const {
        if (console) {  // 换行符要缩进 其余转义直出
            os << BLUE << "\"";
            string indent = string(level + 1, SPACE);  // 缩进
            for (size_t i = 0; i < value.length(); ++i) {
                if (value[i] == '\n')
                    os << "\n"
                       << indent;  // 换行符缩进
                else
                    os << value[i];  // 普通字符
            }
            return os << "\"" << RESET;  // 输出蓝色
        }
        return os << "\"" << literal << "\"";  // 写入文件保留转义字符字面写法
    }
    ostream& abstruct(ostream& os, unsigned int level = 0) const {
        os << BLUE;
        if (value.length() > maxDisplayLength) {
            os << "\"" << literal.substr(0, maxDisplayLength) << "...\"";  // 截断显示
        } else {
            os << "\"" << literal << "\"";
        }
        return os << RESET;  // 输出蓝色
    }
    static string parseString(const string& str, size_t& pos) {
        if (str[pos] != '"') throw runtime_error("Invalid string at position " + to_string(pos) + ", expected '\"'");
        string value;
        ++pos;  // 跳过开头的"
        while (pos < str.length()) {
            if (str[pos] == '"') break;  // 找到结尾的"
            if (str[pos] == '\\') {  // 转义字符
                ++pos;  // 跳过斜杠
                if (pos >= str.length()) throw runtime_error("Unterminated string at position " + to_string(pos));
                switch (str[pos]) {
                    case 'n':
                        value += '\n';
                        break;
                    case 't':
                        value += '\t';
                        break;
                    case 'r':
                        value += '\r';
                        break;
                    case 'b':
                        value += '\b';
                        break;
                    case 'f':
                        value += '\f';
                        break;
                    case '"':
                    case '\\':
                        value += str[pos];
                        break;
                    case 'u':
                        value += "\\u";  // 不解析Unicode，直接添加
                        break;
                    case '\n':
                        JSONNode::skipSpace(str, ++pos);  // 忽视换行符和所有缩进，实现多行字符串
                        pos--;
                        break;
                    default:
                        throw runtime_error("Unsupported escape sequence at position " + to_string(pos));
                }
            } else {
                value += str[pos];  // 普通字符
            }
            ++pos;
        }
        if (pos >= str.length()) throw runtime_error("Unterminated string at position " + to_string(pos));
        ++pos;  // 跳过结尾的"
        return value;
    }
    static JSONString* parse(const string& str, size_t& pos) {
        return new JSONString(JSONString::parseString(str, pos));  // 解析字符串
    }
};
size_t JSONString::maxDisplayLength = 32;

class JSONNull : public JSONNode {
public:
    JSONNull() : JSONNode(JSONType::NULL_TYPE) {}
    ostream& print(ostream& os, unsigned int level = 0, bool console = false) const {
        if (console) return os << RED << "null" << RESET;  // 输出红色
        return os << "null";
    }
    ostream& abstruct(ostream& os, unsigned int level = 0) const {
        return os << RED << "null" << RESET;
    }
    static JSONNull* parse(const string& str, size_t& pos) {
        if (str.substr(pos, 4) == "null") {
            pos += 4;
            return new JSONNull();
        }
        throw runtime_error("Invalid null value at position " + to_string(pos) + ", expected 'null'");
    }
};

// 下面两个是复杂的结构
class JSONObject : public JSONNode {
public:
    vector<pair<string, JSONNode*>> members;  // 用于存储成员变量的vector 为了有序
    JSONObject() : JSONNode(JSONType::OBJECT) {}
    ~JSONObject() {
        for (auto& pair : members) delete pair.second;
    }
    const JSONNode* operator[](const string& key) const {
        auto it = find_if(members.begin(), members.end(), [&key](const pair<string, JSONNode*>& p) { return p.first == key; });
        if (it != members.end()) return it->second;
        if (JSONNode::IGNORE_ERROR) return JSONNode::jsonNull;
        throw runtime_error("Key not found in JSONObject: \"" + key + "\"");
    }
    JSONNode* operator[](const string& key) {
        auto it = find_if(members.begin(), members.end(), [&key](const pair<string, JSONNode*>& p) { return p.first == key; });
        if (it != members.end()) return it->second;
        if (JSONNode::IGNORE_ERROR) return (JSONNode*)(JSONNode::jsonNull);
        throw runtime_error("Key not found in JSONObject: \"" + key + "\"");
    }
    const JSONNode* operator[](size_t index) const {
        throw runtime_error("Cannot use operator[] with size_t on JSONObject");
    }
    JSONNode* operator[](size_t index) {
        throw runtime_error("Cannot use operator[] with size_t on JSONObject");
    }
    ostream& print(ostream& os, unsigned int level = 0, bool console = false) const;
    ostream& abstruct(ostream& os, unsigned int level = 0) const;

    /**
     * 解析JSON对象 支持最后一项的末尾是逗号
     * @param str JSON字符串
     * @param pos 当前解析位置的引用 会被修改
     * @return 解析后的JSONObject指针
     */
    static JSONObject* parse(const string& str, size_t& pos);
};

class JSONArray : public JSONNode {
public:
    vector<JSONNode*> elements;
    JSONArray() : JSONNode(JSONType::ARRAY) {}
    ~JSONArray() {
        for (auto& element : elements) delete element;
    }
    const JSONNode* operator[](size_t index) const {
        if (index < elements.size()) return elements[index];
        if (JSONNode::IGNORE_ERROR) return jsonNull;
        throw out_of_range("Index out of range in JSONArray, " + to_string(index) + " >= " + to_string(elements.size()));
    }
    JSONNode* operator[](size_t index) {
        if (index < elements.size()) return elements[index];
        if (JSONNode::IGNORE_ERROR) return (JSONNode*)(jsonNull);
        throw out_of_range("Index out of range in JSONArray, " + to_string(index) + " >= " + to_string(elements.size()));
    }
    const JSONNode* operator[](const string& key) const {
        throw runtime_error("Cannot use operator[] with string on JSONArray");
    }
    JSONNode* operator[](const string& key) {
        throw runtime_error("Cannot use operator[] with string on JSONArray");
    }
    ostream& print(ostream& os, unsigned int level = 0, bool console = false) const;
    ostream& abstruct(ostream& os, unsigned int level = 0) const;
    /**
     * 解析JSON数组 支持最后一项的末尾是逗号
     * @param str JSON字符串
     * @param pos 当前解析位置的引用 会被修改
     * @return 解析后的JSONArray指针
     */
    static JSONArray* parse(const string& str, size_t& pos);
};

//====== 因为incomplete type，所以函数放到最后定义 ======//
// 由于多态这个函数大概不会被调用到
ostream& JSONNode::print(ostream& os, unsigned int level, bool console) const {
    const string indent = string(level, SPACE);
    os << indent;
    switch (type) {
        case JSONType::OBJECT:
            return ((JSONObject*)(this))->print(os, level, console);
        case JSONType::ARRAY:
            return ((JSONArray*)(this))->print(os, level, console);
        case JSONType::STRING:
            return ((JSONString*)(this))->print(os, level, console);
        case JSONType::NUMBER:
            return ((JSONNumber*)(this))->print(os, level, console);
        case JSONType::BOOLEAN:
            return ((JSONBoolean*)(this))->print(os, level, console);
        case JSONType::NULL_TYPE:
            return ((JSONNull*)(this))->print(os, level, console);
    }
    return os;
}

ostream& JSONNode::abstruct(ostream& os, unsigned int level) const {
    const string indent = string(level, SPACE);
    os << indent;
    switch (type) {
        case JSONType::OBJECT:
            return ((JSONObject*)(this))->abstruct(os, level);
        case JSONType::ARRAY:
            return ((JSONArray*)(this))->abstruct(os, level);
        case JSONType::STRING:
            return ((JSONString*)(this))->abstruct(os, level);
        case JSONType::NUMBER:
            return ((JSONNumber*)(this))->abstruct(os, level);
        case JSONType::BOOLEAN:
            return ((JSONBoolean*)(this))->abstruct(os, level);
        case JSONType::NULL_TYPE:
            return ((JSONNull*)(this))->abstruct(os, level);
    }
    return os;
}

const JSONNull* JSONNode::jsonNull = new JSONNull();

JSONNode* JSONNode::parse(const string& str, size_t& pos) {
    JSONNode::skipSpace(str, pos);  // 跳过空格和注释
    if (pos >= str.length()) return nullptr;  // 解析结束
    switch (str[pos]) {
        case '{':
            return JSONObject::parse(str, pos);  // 对象
        case '[':
            return JSONArray::parse(str, pos);  // 数组
        case '"':
            return JSONString::parse(str, pos);  // 字符串
        case 't':
        case 'f':
            return JSONBoolean::parse(str, pos);  // 布尔值true
        case 'n':
            return JSONNull::parse(str, pos);  // null
        default:
            return JSONNumber::parse(str, pos);  // 数字 需要处理所有错误
    }
}

ostream& JSONObject::print(ostream& os, unsigned int level, bool console) const {
    const string indent = string(level, SPACE);
    const string indent2 = string(level + 1, SPACE);
    // 开头没有缩进，这个缩进由父层级控制
    if (console)
        os << YELLOW << "{\n"
           << RESET;  // 输出黄色
    else
        os << "{\n";
    for (size_t i = 0; i < members.size(); ++i) {
        os << indent2;
        if (console)
            os << YELLOW << "\"" << members[i].first << "\": " << RESET;  // 输出黄色
        else
            os << "\"" << members[i].first << "\": ";
        members[i].second->print(os, level + 1, console);
        if (i != members.size() - 1) os << ',';
        os << '\n';
    }
    os << indent;  // 结尾缩进是level
    if (console)
        os << YELLOW << "}" << RESET;  // 输出黄色
    else
        os << '}';
    return os;
}
ostream& JSONObject::abstruct(ostream& os, unsigned int level) const {
    const string indent = string(level, SPACE);
    const string indent2 = string(level + 1, SPACE);
    os << YELLOW << "{\n"
       << RESET;
    for (size_t i = 0; i < members.size(); ++i) {
        os << indent2 << YELLOW << "\"" << members[i].first << "\": " << RESET;
        if (members[i].second->getType() == JSONType::OBJECT) {
            JSONObject* obj = (JSONObject*)(members[i].second);
            os << YELLOW << "OBJECT{" << obj->members.size() << "}" << RESET;
        } else if (members[i].second->getType() == JSONType::ARRAY) {
            JSONArray* arr = (JSONArray*)(members[i].second);
            os << MAGENTA << "ARRAY[" << arr->elements.size() << "]" << RESET;
        } else {
            members[i].second->abstruct(os, level + 1);
        }
        if (i != members.size() - 1) os << ',';
        os << '\n';
    }
    os << indent << YELLOW << '}' << RESET;
    return os;
}
JSONObject* JSONObject::parse(const string& str, size_t& pos) {
    if (str[pos] != '{') throw runtime_error("Invalid object at position " + to_string(pos) + ", expected '{'");
    ++pos;  // 跳过'{'
    JSONObject* obj = new JSONObject();
    while (pos < str.length()) {
        JSONNode::skipSpace(str, pos);  // 跳过空格和注释
        if (str[pos] == '}') {  // 找到结束的'}'
            ++pos;  // 跳过'}'
            return obj;
        }

        string key = JSONString::parseString(str, pos);  // 解析键
        auto it = find_if(obj->members.begin(), obj->members.end(), [&key](const pair<string, JSONNode*>& p) { return p.first == key; });
        if (it != obj->members.end()) throw runtime_error("Duplicate key \"" + key + "\" at position " + to_string(pos));

        JSONNode::skipSpace(str, pos);  // 跳过空格和注释
        if (str[pos] != ':') throw runtime_error("Invalid object at position " + to_string(pos) + ", expected ':'");
        ++pos;  // 跳过':'

        JSONNode* value = JSONNode::parse(str, pos);  // 解析值
        obj->members.push_back(make_pair(key, value));  // 将键值对加入对象中
        JSONNode::skipSpace(str, pos);  // 跳过空格和注释
        if (str[pos] == ',') ++pos;  // 跳过','，支持最后一项的末尾是逗号
    }
    throw runtime_error("Unterminated object at position " + to_string(pos));
}

ostream& JSONArray::print(ostream& os, unsigned int level, bool console) const {
    const string indent = string(level, SPACE);
    const string indent2 = string(level + 1, SPACE);
    if (console)
        os << MAGENTA << "[\n"
           << RESET;
    else
        os << "[\n";
    for (size_t i = 0; i < elements.size(); ++i) {
        os << indent2;
        if (console) os << MAGENTA << '[' << i << "]: " << RESET;
        elements[i]->print(os, level + 1, console);
        if (i != elements.size() - 1) os << ',';
        os << '\n';
    }
    os << indent;  // 结尾缩进是level
    if (console)
        os << MAGENTA << "]" << RESET;
    else
        os << ']';
    return os;
}
ostream& JSONArray::abstruct(ostream& os, unsigned int level) const {
    const string indent = string(level, SPACE);
    const string indent2 = string(level + 1, SPACE);
    os << MAGENTA << "[\n"
       << RESET;
    for (size_t i = 0; i < elements.size(); ++i) {
        os << indent2 << MAGENTA << '[' << i << "]: " << RESET;
        if (elements[i]->getType() == JSONType::OBJECT) {
            JSONObject* obj = (JSONObject*)(elements[i]);
            os << YELLOW << "OBJECT{" << obj->members.size() << "}" << RESET;
        } else if (elements[i]->getType() == JSONType::ARRAY) {
            JSONArray* arr = (JSONArray*)(elements[i]);
            os << MAGENTA << "ARRAY[" << arr->elements.size() << "]" << RESET;
        } else {
            elements[i]->abstruct(os, level + 1);
        }
        if (i != elements.size() - 1) os << ',';
        os << '\n';
    }
    os << indent << MAGENTA << ']' << RESET;
    return os;
}
JSONArray* JSONArray::parse(const string& str, size_t& pos) {
    if (str[pos] != '[') throw runtime_error("Invalid array at position " + to_string(pos));
    ++pos;  // 跳过'['
    JSONArray* arr = new JSONArray();
    while (pos < str.length()) {
        JSONNode::skipSpace(str, pos);  // 跳过空格和注释
        if (str[pos] == ']') {  // 找到结束的']'
            ++pos;  // 跳过']'
            return arr;
        }
        JSONNode* value = JSONNode::parse(str, pos);  // 解析值
        arr->elements.push_back(value);  // 将值加入数组中
        JSONNode::skipSpace(str, pos);  // 跳过空格和注释
        if (str[pos] == ',') ++pos;  // 跳过','，支持最后一项的末尾是逗号
    }
    throw runtime_error("Unterminated array at position " + to_string(pos));
}