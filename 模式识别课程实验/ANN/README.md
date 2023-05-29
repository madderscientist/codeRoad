# 神经网络实验
最终模型参数存于./model4/model_10batch_final.json，正确率达98.08%。
## 文件结构
```
神经网络
│  ann.py       训练网络
│  nn.py        神经网络类
│  README.md    本文件
│  result.csv   use.py的运行结果
│  use.py       使用训练好的网络
|  wrong_classified.jpg     use.py的运行结果，识别错误的图片
│
├─MindSpore
|      mindspore.ipynb      mindspore代码
|      mindspore.html       运行结果
|
├─model1                    第一代模型
│      model.txt            模型结构和训练过程
│      model_final.json     模型参数
│      
├─model2                    第二代模型
│      model.txt            模型结构和训练过程
│      model2_final.json    模型参数
│      
├─model3                    第三代模型
│      ann.html             训练过程
│      model.txt            模型结构
│      model_single_final.json  模型参数
│      
└─model4                    第四代模型
       batch=10.jpg         训练过程
       model.txt            模型结构
       model_10batch_final.json 模型参数
```
model1、model2、model3是相对失败的模型，仅用于佐证实验报告的内容，体现网络的升级优化过程。验收请使用model4文件夹中的模型。
## 使用说明（自己的神经网络）
从**上一级目录**的data文件夹中加载MNIST数据集（和朴素贝叶斯实验共用），请勿移动本文件夹的位置。工作文件夹应该设为本文件夹。
### 从头开始训练：
1. 运行ann.py，等待约15分钟完成训练。注意设置模型保存的文件夹。
2. 运行use.py，把在测试集上识别结果输出到reslut.csv。注意修改使用的模型参数文件。
### 使用我已训练的模型：
- 直接运行use.py即可。模型加载路径已设为model4（目前的最优模型）