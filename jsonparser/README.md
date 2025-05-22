# JSON解析器
助教完成自己出的题时的感想

JSON出自于JavaScript，这是一个万物皆对象的语言，非常适合体现本学期“类与对象”的学习内容。解释型语言列表的每一个元素都可以是不同的类型，JSON中也有这样的机制，经由本任务可以探索一下如何实现这样的灵活。

## 题目
完成一个JSON解析库，并实现一些命令用于调用。

### JSON语法
参考：https://www.runoob.com/json/json-syntax.html

至少需要支持的数据类型：
- 数值(float)
- 字符串(string)
- 布尔(bool)
- 空(null)
- 对象(即JSON中的{})
- 列表(即JSON中的[])

### 命令设计
假设文件file.json内容如下（未格式化）
```json
{"name": "LiHua",
    "gender": true,
    "age": 18, "mate": null,
"family": [2, {    "relation": "father",
"name": "LiMing",
            "age": 45
    }, {
            "relation":"mother",
            "name": "WangMei",
            "age": 43,
            "education": [
            "Southeast University","Nanjing University"]
        }
    ], "hobbies": ["reading", "swimming", "coding"],}
```
可以设计如下命令（可自行设计于拓展）：
- 打开文件：`read file.json`
- 访问层级：`json["family"][2]`，会格式化打印出当前层级的内容：
    ```json
    json["family"][2]: {
        "relation": "mother",
        "name": "WangMei",
        "age": 43,
        "education": LIST[2]
    }
    ```
    更多例子：
    ```json
    json["family"]: [
        [0]: 2, // 前面的[0]是LIST输出的格式，不是json语法
        [1]: OBJECT{3},
        [2]: OBJECT{4}
    ]

    json["hobbies"][0]: "reading"   // 字符串有引号，其余没有

    json["age"]: 18

    json["mate"]: null
    ```

- 打印层级：`json["family"][2] all`，区别在于上一个命令会缩写LIST和OBJECT，此命令会展开所有子层级：
    ```json
    json["family"][2]: {
        "relation":"mother",
        "name": "WangMei",
        "age": 43,
        "education": [
            "Southeast University",
            "Nanjing University"
        ]
    }
    ```

- 格式化保存：`write format.json`

- 更多：修改、读取、创建等

## 分析
此类程序的基本结构是，主要功能写成一个库（jsonparser.hpp），库可以导出为dll之类的链接库供其他程序使用（本任务不需要）；在主程序中实现与人的交互，即命令行功能。主程序一般有两种用法，可以参考python的做法：单独运行python就进入写代码模式（即cmd中输入`python.exe`），如果后面跟了一个文件则运行该文件（如`python.exe test.py`）。

难点：
- 数据类型可变（比如List中如何存储六种不同的类型？）【类型擦除？继承？】
- 转义字符处理
- 人机交互与异常处理
- 输出格式

相比于“聚类”，本任务数学性不高，门槛较低；但比“简易Matlab”更需要数据结构上的设计。

（其实聚类的数学性也不是很高，题目要求的算法都很简单，几个函数就能解决，也许“层次聚类”的方法才能体现本学期“类与对象”的特点）

## 拓展性
基本功能难度不高，但卷起来可以很难。

头脑风暴了一些可能的拓展方向：
- 格式化参数可配置（读取配置文件的指令、配置文件作为exe参数、默认配置文件），比如颜色、缩进距离、输出模板。比如配置文件就用JSON写。
- 更丰富的功能，比如修改和创建。
- 实现JSON5的一些特性（自行搜索JSON5和JSON的区别）。
- 更丰富的提示：比如仿C++编译器的错误输出，将出问题的地方的附近输出，并有下划线。
- 更丰富的格式：可以改变输出的文件格式，比如json转toml、yaml。这些数据序列化格式有一定相似性。
- 更进一步，多个JSON之间的交互。可以在命令行里引入“变量”等概念，实现“JSONlab”。
- 数据图形化，比如 https://www.json.cn/ 上可以通过点击加减号观察数据。可以完成一个基于cmd的GUI，鼠标交互可以参考https://www.cnblogs.com/fox-nest/p/12233197.html
- 实时预览与实时语法高亮：用户在控制台输入时（回车前）也可以动态高亮代码、并输出当前层级。类似浏览器控制台。


## 关于本实现
实现了基本解析功能与交互功能，一共大概用了5个小时，完成解析库大概用了一半时间，剩下一半用在主函数的交互上了。可见基本实现还是很简单的。

### 使用
运行程序后，可以使用以下命令：

- **`exit`**: 退出程序
- **`help`**: 显示帮助信息
- **`json[key]...`**: 简要打印指定层级的内容
- **`json[key]... all`**: 递归详细打印指定层级的内容
- **`file.json`**: 从 `file.json` 文件中读取 JSON 数据  
- **`write file.json`**: 将格式化后的 JSON 写入到 `file.json`

或者直接通过命令行传递两个参数运行程序：

- **`./jsonparser input.json output.json`**: 从 `input.json` 文件中读取 JSON 数据，格式化后写入到 `output.json` 文件中。

例子：
```bash
./jsonparser
$ example.json
$ json
$ json all
$ json[family]
$ json["family"][1]
$ json["family"][99]
$ write formatted.json
$ exit
./jsonparser example.json formatted2.json
```

如果win10的cmd不显示ANSI转义序列（没显示颜色），管理员运行：
```bat
reg add HKCU\Console /v VirtualTerminalLevel /t REG_DWORD /d 1 /f
```

### 本实现的特点
- 使用颜色表示不同的类别。
- 较为完善的错误提醒。
- 支持C风格的注释（其实是JSON5特性）。
- Object和List的最后一个元素末尾可以有逗号（允许尾随逗号也是JSON5标准）。
- 转义中没有支持Unicode字符（JSON支持\uXXXX）。
- 支持解析输入文本中的换行、tab之类，而不用写成转义形式。
- 支持解析多行文本（在换行前加'\\'）（这是JSON标准不允许的，但JS有此类语法），而格式化后将变成标准JSON格式。
- 没有用文件流，而是一次性读取文件为string。这是可以改进的地方。
- Object的key索引效率不高（对顺序的妥协），可能有更好的方法。整体性能可能也不够好。
- 多类型的实现非常直接，代码结构非常清晰。
- 偷懒而全部使用了public。
- 偷懒而大量使用stl库。

## 对开题的点评
都是“词法分析”“语法分析”那一套，但正如9组的文档里所说，其实不需要“词法分析”。https://www.cnblogs.com/javastack/p/12441538.html
对难点的认识有偏差：不是怎么解析，而是怎么存，即数据结构的设计。
此外没有对主函数作用的认识。
对任务量没有明确的认识：其实很简单。所以分工后工作量偏少。
不必拘泥于json的语法，因为有一些局限设计，所以有了json5
很多细节没考虑到，不过实现的时候都会发现的。

- 2组：JavaScript不是Java；
- 5组：建议是一个类由一个人完成，第二周第三周的任务放到第一周同步开展，通过接口约定实现协作
- 9组：json其实有自己的后缀名；缺少main函数的分工。
- Markdown组：实时预览的优化？markdown语法高亮和代码块语法高亮如何实现？html语法支持？

## 团队协作
需要有一个“架构师”（“产品经理”？“leader”？），对项目进行拆解，明确好项目所有的分块与接口，每个人完成一个模块。

“代码未动，文档先行”，先将所有目标与功能确定好，再设计接口（比如类对外暴露的方法与变量），最后开始分工写代码。因此需要“架构师”对整个项目有深入的思考，最好是能独立完成整个项目的人来。
就像每次作业给出了main.cpp（目标），提供了mystring.h（接口定义），“架构师”的任务就是编写这些内容。

记得将思考过程、设计理由、框架结构、接口约定等全部记录下来（推荐使用markdown格式），这是良好的开发习惯，也便于结题。

下面给出一些对齐进度的方法：
- 土法：群里面直接发文件。可行但效率太低，需要分工足够明确。
- 用git+github(gitee、gitcode、学校的gitlab)：适用于大项目协作。可能git对与大一来说早了点，想学可以在这里学：https://learngitbranching.js.org/?locale=zh_CN
- 在线协作IDE：类似腾讯文档。比较著名的是replit，但是免费版一个项目最多两个人。发现了另一个，不知道可以支持多少人：https://cloudstudio.net/。