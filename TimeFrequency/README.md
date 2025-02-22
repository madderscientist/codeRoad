# 时频变换方法的研究与实现
包含FFT和CQT的研究与实现。FFT的实现结构、运行效率可以在具体js代码的开头注释中看到。

## 文件结构
```txt
mine/js/audio
├─.vscode   Cpp->WASM编译选择
│      c_cpp_properties.json
│      tasks.json
│
├─cpp       CQT的cpp实现并编译为WASM
│      cqt.cpp
│      cqt_wasm.cpp
│      cqt_wasm_down4.cpp
│      CQT_wasm_down4_result.jpg
│      CQT_wasm_result.jpg
│      fft.cpp
│
├─js        CQT和FFT的js实现
│  │  cqt(once used in noteDigger).js
│  │  cqt.js
│  │  cqt_down.js
│  │  fft_flex.html
│  │  fft_flex.js
│  │  fft_i.html
│  │  fft_i.js
│  │  fft_r.js
│  │  fft_real.js
│  │  fft_utils.js
│  │  index.html
│  │
│  └─audio  WebAudioAPI的尝试使用
│          compatible.js
│          fft.js
│          index.html
│          多振荡器生成声音.html
│          生成声音并FFT.html
│
└─wasm      WASM的编译结果
        cqt.wasm.js
        cqt.wasm.wasm
```

## CQT
见文章[CQT：从理论到代码实现](https://zhuanlan.zhihu.com/p/716574483)

学习资源（均只保留了CQT相关代码）：
- ./librosa
- ./nnAudio
- ./const-q-toolbox

相关文章（对应于上述库中使用的两个年份的算法）：
- [1992 An efficient algorithm for the calculation of a constant Q transform](<1992 An efficient algorithm for the calculation of a constant Q transform.pdf>)
- [2010 Constant-Q_transform_toolbox_for_music_processing](<2010 Constant-Q_transform_toolbox_for_music_processing.pdf>)


## WASM配置简记
见mine文件夹下的cpp和.vscode
- mine/.vscode/c_cpp_properities.json: includePath要添加emcc的路径
- mine/.vscode/tasks.json: VScode任务，懒得每次输入，注意那些参数，用bingding一定要生成js文件，不然自己根本用不起来
- mine/cpp/cqt_wasm.cpp: 以后可以复用的是开头两个高效的TypedArray传递函数。EMCC可以用引用类型

在[noteDigger](https://github.com/madderscientist/noteDigger/tree/main/dataProcess/CQT)中使用了wasm版本的CQT。

导出的js是commonJS，但是可以用import Module from ""引入，可以直接`<script src="">`导入。