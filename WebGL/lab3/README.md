# 实验3
A、 场景中对象不少于两个，其中至少一个可以自由移动
B、 交互式的视点变化，实现场景漫游
C、 实现光照效果，光源位置绑定在移动物体上，参数可调    —— 和要求A合并
D、 场景中至少包含两种以上的材质    —— 和要求A合并

使用方法：
- 没有进入全屏模式时，右击可以选一个模型放置，可以选“安柏.stl”。默认陶瓷质感。
- 左击进入全屏后，wasd移动，空格shift上下，ctrl可以加速，滚轮修改视窗大小，体现为能看到多少东西、视角大小、所见物体大小。
- 上下左右pageup pagedown可以控制一个带实体的点光源的移动。不过移动基于世界坐标系。
- 按键R G B可以设置光源的颜色分量+0.02，按住alt键再按则是减法。

## mesh
决定所有mesh统一使用drawElements的方式绘制。理由有2：一是Three.js广泛使用此法，二是一个索引可以索引多个缓冲区，大大减少内存开销。
每个mesh需要保存三个数组：顶点位置数组、顶点法向数组、顶点索引数组。直接创建gl缓冲区保存，因为设计理念是模型创建后不准修改。创建缓冲区保存数据的示例代码：
```js
// 创建位置缓冲区
this.vertexBuffer = gl.createBuffer();
gl.bindBuffer(gl.ARRAY_BUFFER, this.vertexBuffer);
gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(positions), gl.STATIC_DRAW);
// 创建颜色缓冲区(如果有颜色的话)
this.colorBuffer = gl.createBuffer();
gl.bindBuffer(gl.ARRAY_BUFFER, this.colorBuffer);
gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(colors), gl.STATIC_DRAW);
// 创建法向缓冲区
...
// 创建索引缓冲区
this.indexBuffer = gl.createBuffer();
this.indexBuffer.length = indices.length;
let type = indices.length>255 ? [Uint16Array, gl.UNSIGNED_SHORT]:[Uint8Array, gl.UNSIGNED_BYTE];
this.indexBuffer.type = type[1];
gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.indexBuffer);
gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new (type[0])(indices), gl.STATIC_DRAW);
```
gl.ARRAY_BUFFER指示了当前在操作哪个buffer，因为gl的函数操作的是其指向的数组。

每个顶点有多个参数：位置、法向、（可能有颜色）、序号。顶点法向的定义：其周围面的法向之和的归一化。
因此得到读取stl文件的流程：对每个面的每个点，查询已有点集中是否有这个点，如果有，则已有点的法向加上这个面的法向；如果没有，则加入点集，赋予索引，设其法向为当前面的法向。同时维护索引列表。读取完毕，写入glBuffer。
用TRIANGEL模式绘制，需要知道indexBuffer的长度和数据类型。这两个值在读取到stl面数后都可以确定。由于WebGLBuffer不存储这两个属性，因此手动赋予js对象这个属性：indexBuffer.length = 面数*3; indexBuffer.type = gl.UNSIGNED_BYTE;

## material
mesh中可以定义每个顶点的颜色，而material类接管统一的颜色。着色器需要实现“顶点颜色”和“material颜色”的兼容，具体办法是：material的颜色为rgba，顶点颜色只有rgb，着色器中根据material的a叠加两个颜色，js中判断mesh是否有颜色属性，没有就禁用attribute。
可行的依据是：如果disableVertexAttribArray之后使用该attribute，则着色器中会使用默认值零。
因此如果要实现纯material颜色，则a置1；纯mesh颜色，a置0。

material依附在obj上，和mesh同级。

## 光源
目标：可以添加多光源。
发现在着色器里可以实现uniform数组传参。光源大多是静态的，因此不设置每次更新传入，而是修改时更新值。
关于光源和物体的绑定，没有设计数据结构去支持，因此实现方式较为丑陋：
1. 设置物体的法向翻转，使之能被内部的光源照亮。
2. 光源移动后，按光源的位置设置平移变换矩阵，以更改物体的位置。
3. 为了体现光源的颜色，物体的材质中，漫反射调为1，颜色调为纯白。
```js
const lightObj = {
    light: GLroot.addLight(new Pointolite([1, 1, 1], [0, 10, 0])),
    obj: GLroot.addChild('bulb', new GLobj(
        GLroot.gl,
        GLobj.translation(0, 10, 0),
        GLobj.rotation(0),
        GLmesh.cube(GLroot.gl, true),   // 设置法向翻转，这样就可以被里面的光源照亮了
        new GLmaterial(0.1, 1, 0, 20, [1, 1, 1, 1]),  // 漫反射不能不设，不然黑的
        GLobj.scale(0.2, 0.2, 0.2)
    ))
};
// 改变光源颜色
lightObj.light.color.set([r,g,b]);
// 移动光源与绑定物
lightObj.obj.displacement = GLobj.translation(
    lightObj.light.position[0],
    lightObj.light.position[1],
    lightObj.light.position[2]
); GLroot.updateLight(lightObj.light.id);   // 只更新一个
```

## 着色器
使用Phong平滑着色，即使用平均法向、在片元着色器中计算光照。这对于圆滑的物体效果很好，但是对于棱角分明的物体，因为平均法向，会导致有凝胶效果，好似光透入了物体。如果以后要改，也许要设置多种类型的着色器，渲染每个物体的时候更改着色器。
高光的颜色与物体颜色无关，而漫反射和环境光与物体颜色有关。镜面反射使用了Blinn-Phong改进。
没有做衰减。

## 一些妥协（一些坑）
第一次在着色器中用到了int类型，gl拓展类没有实现int的传参，因为不好区分是整数还是浮点数。因此设置了“通过字符串判断是整数还是浮点数”，通过字符串中是否有小数点来决定。所以可以看到设置光源数目时使用了toString()。
着色器的for循环，判断必须是const，所以得在循环体里设置动态的循环次数。
因为写着色器的时候一直编译不通过，故开启了着色器的编译输出。得知在webgl中，不能将vec3赋值给vec4，类型之间的运算很死板。但是从js向uniform传参的时候可以不传满，对于没有涉及的位会有很人性化的自动填充，比如第四个默认传1，表示一个点。
由于光源涉及着色器编写，因此只能由GLroot统一管理了。mesh的创建由于需要直接转WebGLBuffer，所以构造的时候需要传gl，不是很优雅。