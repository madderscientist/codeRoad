# 节奏分析
知乎上配有一文: https://zhuanlan.zhihu.com/p/1995849093491222501

## 文件结构
```raw
bpm_ana.ipynb       完全传统信号处理
beat_track.ipynb    用AMT的onset输出直接HMM

DBNBeatTracker.js   复刻了DBN
bpmEst.js           js版本

README.md
utils.py            基本工具函数

cowbell.wav         节拍声

加藤達也-Always in my heart.json        从noteDigger采集到的差分数组
加藤達也-Always in my heart.wav         原曲
加藤達也-Always in my heart_onset.npy   AMT的onset输出

彩音 - いつもこの場所で.json
彩音 - いつもこの場所で.wav
彩音 - いつもこの場所で_onset.npy
```