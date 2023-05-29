# 隐马尔可夫分词
**没有使用提供的训练集！使用了自己找的北大199801语料库！**
## 文件
```
隐马尔可夫
│  hmm.py               隐马尔科夫类
│  hmm_model.pkl        模型参数
│  hmm_result.txt       使用【我找的训练集】在测试集上的分词结果
|  hmm_result_on_train.txt  使用【我找的训练集】在【原本的训练集】上的分词结果
|  hmm_result_bef.txt   使用【原本的训练集】在测试集上的分词结果
│  makedata.py          制作训练集
│  README.md            本文件
|  use.py               训练、使用网络
|
├─MindSpore
       hmm_model.utf8   预训练的隐马尔可夫模型
       jieba.dict.utf8  字典
       mindspore.py     mindspore分词
       MindSpore_fenci_result.txt   mindspore分词结果

```

## 使用说明（自己的HMM）

从**上一级目录**的data文件夹中加载语料库，请勿移动本文件夹的位置。工作文件夹应该设为本文件夹。<br>
使用顺序：
1. 运行makedata.py，从北京大学199801语料库提取所需训练文本
2. 运行use.py，模型数据存于hmm_model.pkl，测试集识别结果存于hmm_result.txt

## 要求之外的文件
用自己找的训练集（199801.txt）训练，对原本的训练集进行了分词，结果在hmm_result_on_train.txt中，用以对比