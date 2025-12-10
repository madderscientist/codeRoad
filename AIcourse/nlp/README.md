# 语法解析

## 语言学中有一堆符号

gramma: 短语结构 的列表
短语结构有如下分类，用类 `CFGRule` 存储每一条组成规则：
- S: 句子，可以有两种组成
    - NP & VP
    - S & Conj & S
- NP: 名词短语，可以有如下组成
    - Pronoun
    - Name
    - Noun
    - Article & Noun
    - Article & Adjs
    - Digit & Digit
    - NP & PP
    - NO & RelClause
- VP: 动词短语，可以有如下组成
    - Verb
    - VP & VP
    - VP & Adjective
    - VP & PP
    - VP & Adverb
- Adjs: 形容词，可以有如下组成
    - Adjective
    - Adjective & Adjs
- PP
    - Prep & NP
- RelClause
    - RelPro & VP


组成成分中存在gramma之外的，叫做 词性dictionary，用`LexRule` 存储每一个单词：
- Noun: 对应一堆单词
- Verb: 对应一堆单词
- Adjective: 对应一堆单词
- Adverb: 对应一堆单词
- Pronoun: 对应一堆单词
- RelPro: 对应一堆单词
- Name: 对应一堆单词
- Aritcle: 对应一堆单词
- Prep: 对应一堆单词
- Conj: 对应一堆单词
- Digit: 对应数字

有的单词词性有多种可能，所以引入概率。按理说`dictionary`应该是一个二维表格，行表头为词性，列表头为所有单词，每一行代表该单词属于某种词性的概率。


## 语法树
单词就是叶子节点。目标就是得到单词序列对应的句子结构树，其中用`LexRule`将单词变为符号，用`CFGRule`连接符号，根节点为`S`。

题目里的树节点很简单粗暴：每个节点都有token字段，但是其实只有叶节点（`LexRule`定义的规则）才用得上。

算法原理（自底向上）：
1. 用`LexRule`将单词变为符号，通过查字典。
2. 用`CFGRule`进行符号的连接。可能有很多种组合方式，可以用动态规划找到概率最大的建树方法。

难点在于第二步。

`greedy_match`: 遍历每一个规则，找到匹配的最长的规则。比如输入有5个符号，找到了两个匹配的规则A和B：A规则的组成就两个，匹配了这5个符号的前两个；B规则有4个成分组成，匹配了这5个符号的前4个。所以选择B进行返回。这样贪心非常不合理，因为保存了概率却没有使用。

本次实验的算法非常粗暴：输入TreeNode序列nodes，长度为size，然后找到所有“部分匹配”的情况，比如`[i:i+k]`部分匹配了某个规则，则用这个规则将这些节点整合为一个大节点，然后继续迭代。完全没概率什么事。