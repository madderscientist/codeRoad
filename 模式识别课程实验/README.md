# 模式识别课程实验代码、结果、报告
东南大学2023模式识别课程实验<br>
选做了朴素贝叶斯分类MNIST、神经网络分类MNIST、隐马尔可夫中文分词。更多请见各文件夹下的README。<br>
其中，神经网络类设计之初考虑了其他形式的层（比如卷积池化），但是仅用了全连接就达成了目的。虽然留了接口，但是以后可能不会再更新。<br>
data为训练测试所用数据，打包了，包含MNIST、北京大学199801人民日报语料、老师提供的中文分词文本。使用时注意相对路径。应该按下面的方式组织文件：
```
模式识别实验
│  README.md
│  报告.docx
│  
├─ANN
│  │  ann.py
│  │  nn.py
│  │  README.md
│  │  result.csv
│  │  use.py
│  │  wrong_classified.jpg
│  │
│  ├─MindSpore
│  │      mindspore.html
│  │      mindspore.ipynb
│  │
│  ├─model1
│  │      model.txt
│  │      model_final.json
│  │
│  ├─model2
│  │      model.txt
│  │      model2_final.json
│  │
│  ├─model3
│  │      ann.html
│  │      model.txt
│  │      model_single_final.json
│  │
│  └─model4
│          batch=10.jpg
│          model.txt
│          model_10batch_final.json
│
├─data
│  │  199801_train.txt
│  │  t10k-images-idx3-ubyte.gz
│  │  t10k-labels-idx1-ubyte.gz
│  │  test.txt
│  │  train-images-idx3-ubyte.gz
│  │  train-labels-idx1-ubyte.gz
│  │  train.txt
│  │
│  └─199801
│          199801.txt
│          shengming.doc
│
├─HMM
│  │  hmm.py
│  │  hmm_model.pkl
│  │  hmm_result.txt
│  │  hmm_result_bef.txt
│  │  hmm_result_on_train.txt
│  │  makedata.py
│  │  README.md
│  │  use.py
│  │
│  └─MindSpore
│          hmm_model.utf8
│          jieba.dict.utf8
│          mindspore.py
│          MindSpore_fenci_result.txt
│
└─navieBayes
    │  bayes.html
    │  bayes.ipynb
    │  result.csv
    │
    └─MindSpore
            mindspore.html
            mindspore.ipynb
```

还有**华为的mindspore代码，请忽视。要不是课程要求谁会碰这玩意。。。