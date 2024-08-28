# 时频变换方法的研究与实现

## CQT
见文章[CQT：从理论到代码实现](https://zhuanlan.zhihu.com/p/716574483)

学习资源（均只保留了CQT相关代码）：
- librosa
- nnAudio
- const-q-toolbox

## WASM配置简记
见mine文件夹下的cpp和.vscode
- mine/.vscode/c_cpp_properities.json: includePath要添加emcc的路径
- mine/.vscode/tasks.json: VScode任务，懒得每次输入，注意那些参数，用bingding一定要生成js文件，不然自己根本用不起来
- mine/cpp/cqt_wasm.cpp: 以后可以复用的是开头两个高效的TypedArray传递函数。EMCC可以用引用类型

导出的js是commonJS，但是可以用import Module from ""引入，可以直接`<script src="">`导入。