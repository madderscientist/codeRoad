使用方法：
没有进入全屏模式的时候右击可以选一个模型放置，可以选“安柏.stl”。左击进入全屏，wasd移动，空格shift上下，ctrl可以加速，滚轮修改视窗大小，体现为能看到多少东西，体现为视角大小、所见物体大小

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