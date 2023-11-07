# WebGL学习
https://juejin.cn/post/7214517573584945208
https://developer.mozilla.org/zh-CN/docs/Web/API/WebGL_API/WebGL_model_view_projection#%E8%A3%81%E5%89%AA%E7%A9%BA%E9%97%B4
https://developer.mozilla.org/zh-CN/docs/Web/API/WebGL_API/Tutorial/Adding_2D_content_to_a_WebGL_context
https://zhuanlan.zhihu.com/p/470401759
https://juejin.cn/post/7146385487039725582/

每个像素的varying变量会在顶点着色器中差值得到，并存储到一个数组中，这个数组会在片元着色器中使用。
默认不开深度，后绘制的画在上面
虽然webgl是列主序，但是glsl中还是左乘变换矩阵

## 基于VSCODE的端口转发
由于本项目使用了多级文件，所以使用方法是：
1. 在根目录用live server开5500端口
2. 端口转发5500，记得设置可见性为公共
3. 手动导航到需要的文件夹的目录
4. 发给别人