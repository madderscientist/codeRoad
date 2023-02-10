# 加密工具

- 创建时间：2022年2月1日，18:18:26
- 起因：这位[高中同学](https://space.bilibili.com/174819631/)写了个C++的加密库，找我写ui，但是我刚好在看python，就用python写了个ui。
- 目的：学习使用py创建窗口，**学习C++和python联合编程**，学习python导出为exe文件。
- 思路：使用SWIG打包C++接口，使用tk创建窗口。
- 使用：运行exe文件即可，需要注册一下，不同用户的秘钥不同
- 成果：一篇[SWIG入门文章](https://zhuanlan.zhihu.com/p/462193340)。后记：py调用C++的方法比较少，调用C的就优雅很多，直接编译为dll。C++如果写extern "C"就可以按C的方法，但是要在源文件中处理好重载等C编译器不支持的语法。（Why? 因为要支持重载，C++编译后函数名会变，就不知道入口在哪了。）

彼时刚接触py不久，对这阴间语法很是讨厌。
<br>
后记: 后来写Nonebot了，这位同学用C++写了个原神抽卡的内核，要我装bot上去。于是extern “C” 编译为so文件（把要打包的接口用extern "C" 括起来就行），直接调用美滋滋。在此记录一下怎么传数组：
```
import ctypes
so = ctypes.CDLL('genshin/ys.so')   // 加载so库

// 设置传参信息（不设置也没关系）
so.u_draw.restype = ctypes.c_int
so.u_draw.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_char_p]

// 调用so函数
name = ctypes.create_string_buffer(('').encode('utf-8'), 25)
res = so.u_draw(usr, pool_id, name)     // 这个C函数内部向name这个char*写数据了，格式是utf8，返回值是成功与否
result = name.value.decode('utf-8', 'ignore')    // 这样提取返回值的文本
```

怎么编译：
```
gcc draw.cpp json11.cpp sqlite3.c sqlite3pp.cpp -fPIC -shared -o ys.so -l stdc++ -Wl,--as-needed
```