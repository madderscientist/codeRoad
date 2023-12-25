# 实验四 纹理
实现背景和物体表面的贴图，实现下面几种效果：
（1） 贴图固定在物体表面，随物体变动
（2） 纹理场效果
（3） 环境贴图效果
最关键的地方在于如何将纹理整合进当前系统。

## 基础：WebGL 纹理使用
```glsl
// 顶点着色器
attribute vec4 a_Position;
attribute vec2 a_Pin;   // 顶点uv映射坐标
varying vec2 v_Pin;     // 差值后的坐uv标
void main(){
    gl_Position = a_Position;
    v_Pin=a_Pin;
}
```

```glsl
// 片元着色器
precision mediump float;
uniform sampler2D u_Sampler;    // 二维取样器
varying vec2 v_Pin;
void main(){
    gl_FragColor=texture2D(u_Sampler,v_Pin);    // 从取样器中获取片元颜色
}
```

```js
gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, 1);  //对纹理图像垂直翻转。是全局设置。因为图片的uv坐标原点在左下角
gl.activeTexture(gl.TEXTURE0);  // 激活0号纹理单元
// 把纹理对象装进纹理单元里
const texture = gl.createTexture();
gl.bindTexture(gl.TEXTURE_2D, texture);
// 加载图片
const image = new Image();
image.src = './images/erha2.jpg';
image.onload = function () { showMap(); }
function showMap() {    // 贴图
    gl.texImage2D(  // 配置纹理图像
        gl.TEXTURE_2D,
        0,
        gl.RGB,
        gl.RGB,
        gl.UNSIGNED_BYTE,
        image
    );
    gl.texParameteri(   // 配置纹理参数
        gl.TEXTURE_2D,
        gl.TEXTURE_MIN_FILTER,
        gl.LINEAR
    );
    // 将0号纹理分配给着色器，0 是纹理单元编号
    const u_Sampler = gl.getUniformLocation(gl.program, 'u_Sampler');
    gl.uniform1i(u_Sampler, 0);
    render()
}
function render() {
    gl.clear(gl.COLOR_BUFFER_BIT);
    gl.drawArrays(gl.TRIANGLE_STRIP, 0, sourceSize);
}
```
## 颜色
一个物体目前有三种颜色：材质颜色、网格颜色和纹理贴图。加入纹理之前，材质颜色和网格颜色通过材质的透明度进行混合。加入材质之后，受参考书“纹理掩码”的启发，决定让纹理颜色乘上混合后的颜色，作为片元的颜色（不考虑光照）。如果要实现纹理掩码效果，则纹理设置为0或1；如果要实现纯纹理色，则材质颜色设置为(1,1,1,1)。【后来得知Three.js也是这么处理的】

## 读取obj
在[parseObj](./parseObj.md)中我记录了obj的内容。obj链接mtl，mtl链接多张贴图。obj的数据组织方法和drawElements高度吻合，所以基本画法不变。
由于增加了贴图，因此需要给mesh增加uv属性，记录每个顶点的uv映射坐标。需要兼容uv=null的情况。
obj中会说 “usemtl 头发” 来切换贴图。
为了保持灵活性，可以将不同的mtl视为不同的子物体，分开渲染。这些子物体共用顶点数据，但index不同、texture不同。为了实现这个效果，我需要将texture对象保存至mesh类下，这和Three.js不一样。
另一种实现方式是，将多个贴图写入不同纹理单元，然后传参告知使用哪一个单元。这样有两个问题：首先，每个点需要多存储、多传递一个字节的参数。其次，纹理单元数目受限，虽然最少也有8个。
最后一种实现方式是，把多个贴图合并成一个贴图。但是这涉及uv坐标切换、分辨率不同等等问题，不用。
还是决定用子物体的思路，因为灵活。但是mtl文件中，每个材质下不仅是贴图，还有各自的光照相关参数，这是当前程序中material类的范畴。所以决定牺牲一点内存和性能，不共用顶点数据、法线数据、uv数据，而是将不同的部分完全拆成独立的物体，用lab1就构建好的“父子”系统整合成一个物体。这样就可以和Three.js保持一致了——贴图和材质是一个类别，材质不必整合进已经较为臃肿的mesh类中。数据结构如下：
```js
Mesh = {
    vertex: Float32Array,   // 顶点位置
    normal: Float32Array,   // 法向
    color: Float32Array,    // 可为null，RGB
    map: Float32Array,  // 【新增】UV
    index: Uint8Array
}
Material = {
    reflect: Float32Array,  // 光照的系数
    color: Float32Array,    // RGBA
    texture: WebglTexture       // 【新增】材质
}
```
分组的依据是是否使用同一个贴图文件。这要求读取的时候需要将可能多次出现的usemtl合并，可以使用对象{}来聚合数据，属性名设置为mtl子材质名。

### 数据重组
"安柏.obj"中，顶点/uv/法向 的序号并不同，三者的数目也不一定相同，说明对应关系很灵活，并不是按顺序一一对应。这需要重组数据，比如"安柏.obj"中，法线数目少于顶点数目、顶点数目等于uv数目，说明存在多顶点共用法线的情况。而drawElements要求每个buffer的数据一一对应，共用索引。所以组织数据的时候，应该将共用的法线数据给每个相关的顶点都复制一份。当然也存在顶点数小于法向数的情况。uv贴图的数目也不一定相同。
此外，一个obj可以有多个mtl文件。myllib和usemtl类似状态机状态转移
所以有了读取obj的流程：
1. 将v、vn、vt分别读取入数组，用于随机访问。关于存储形式，我觉得没必要解析为数字，可以直接一行的字符串作为一个元素。
2. 按usemtl将f的读取分为几个部分（注意可能多处use同一个mtl，这里需要将文件上不连续但use相同mtl的数据合并为一个部分），每个部分提取mtl的信息。首先是贴图的地址，存储于urls数组（用于后期统一读取，统一读取快一点）；其次提取材质参数中的[Ks,Kd,Ka,Ni]，如果指有三个分量则取平均。为了和程序适配可能需要一些映射，比如环境反射乘上一个系数。
3. 对f的每个部分，重建索引，提取v、vn、vt。需要一些算法，比如将“顶点ID/纹理ID/法向ID”作为一个点的哈希值，如果没有就加入集合，分配新的索引，并从原始数据提取信息添加进数组（如果之前用字符串提取存储，则此步需要split后解析为数字），如果已经有了就提取新的索引。
基本流程：
```js
// 按usemtl分类
const faces = [分好类];
let materialParam = [从mtl提取的数据：Ks,Kd,Ka,Ni];
// 对每一种usemtl
const index = new Uint32Array(faces.length*3);
const vertex = [];  // 长度不确定所以不用类型化数组
const normal = [];
const map = [];
const set = {};
let newID = 0;
let n = 0;  // 指示index中的位置
for (let f of faces) {
    // f="1496/1801/1683 1497/1800/1682 1498/1802/1684"
    let vs = f.split(' ');  // 分成三个点
    for (let v of vs) {
        if(v in set) {  // 直接将字符串视为哈希值
            index[n++] = set[v];
        } else {
            index[n++] = set[v] = newID++;
            let ids = v.split('/');
            {   // 提取点坐标
                let position = V[parseInt(ids[0])].split(' ');  // 假设V是第一步分割好的字符串数组，比如["0.004636 0.718000 -0.022299","0.010986 0.722331 -0.015208",...]
                vertex.push(parseFloat(position[0]),parseFloat(position[1]),parseFloat(position[2])); // 循环展开 当然push效率不高不过没办法
            }{  
                // 提取法向和uv同理
            }
        }
    }
}
return {index: index, vertex: vertex, normal: normal, map: map, materialParam: materialParam, image: null};
```
4. 统一加载图片，保存至image属性中。从image到texture将在material的构造函数中完成。
5. 写另一个函数负责将这些数据打包成程序对象，构建父子关系。

以上只是基本思路，具体实现可以改进。比如存储顶点数据的时候就解析为Float（内存占用大概会小一点）（还有个原因是split函数不支持只切一次）。比如三个Float凑成一个Array存储，或者拆开来，索引时乘3加i。比如用状态机的思路解析文件。

### 加载图片
图片大多压缩过，所以要用Image进行解码。顺便也承担了请求数据的功能。图片加载是异步的，所以需要等待所有加载结束之后再执行后面的内容。因此需要用async包裹整个函数。
基本原理：
```js
// 图片URL数组
var urls = ['images/image1.jpg', 'images/image2.jpg', 'images/image3.jpg'];
// 创建一个Promise数组，每个Promise对应一个图片加载操作
var promises = urls.map(url => {
    return new Promise((resolve, reject) => {
        var img = new Image();
        img.onload = () => resolve(img);
        img.onerror = () => reject(new Error(`load ${url} fail`));
        img.src = url;
    });
});
// 用Promise.all等待所有图片加载完成
Promise.all(promises)
    .then(images => {
        // images是一个数组，包含了所有加载完成的图片。顺序和urls一致
    }).catch(error => {
        // 如果任何一个图片加载失败，都会进入这里
        console.error('Error:', error);
    });
```

## 天空盒
[全景图转cubemap](https://jaxry.github.io/panorama-to-cubemap/?source=post_page-----dd28ebaef48c--------------------------------)
pz: 放z正半轴的
py：放y正半轴的（上面）
ny：下面

[用一个平面完成天空盒](https://webglfundamentals.org/webgl/lessons/zh_cn/webgl-skybox.html)
说是天空盒，其实只是使用了一个平面。这个平面处于相机坐标系下的绝对位置(即视角变换不作用于平面)，但是纹理的映射关系和相机一起变换。
默认是小于1才渲染，而等于1不渲染。为了让平面处于最深处，需要设置其z为1.0。这时需要调用gl.depthFunc(gl.LEQUAL)。

## 多着色器
在[学习three的架构](./学习three的架构.md)中，我总结了Three的着色器管理。复杂情况多着色器很重要。比如天空盒、纹理光照、环境贴图、纹理场，就是三种不同的着色器。
为了实现多着色器，当前gl拓展类需要更改，uniform的proxy需要转移到program上。
其次，每个material需要绑定着色器。着色器的选择由material决定。为了解耦，material只有在加入GLobjRoot的时候才会被赋予着色器(地址)，也就是说需要手动绑定。不同着色器对应不同的赋值操作，最懂着色器的非着色器对象莫属。所以决定充分利用js“基于对象”的编程思维，在编译着色器之后立马为着色器程序对象配备配套绘制程序，程序使用类似闭包的做法固定了gl，这样GLobj就不需要gl这个对象了。传参如何确定？由于GLobj类只提供了父子系统，即每个物体的位置变换矩阵，所以传参为{mesh, material, translation}。
总结，gl和着色器绑定在绘制程序中，绘制程序绑定着色器程序，着色器程序绑定到material上，material绑定到GLobj上。
创建一个新材质的流程：
1. 新建材质类
2. 在GLobjRoot中新增着色器生成函数，要求返回编译好的着色器对象，其draw属性完成绘制，传参为{mesh, material, translation}
3. 在GLobjRoot.iniGL中调用新的着色器生成函数并保存至GLobjRoot.programs['新着色器名']
4. 在GLobjRoot.setMaterialProgram中关联两者：if(material instanceof 新材质类名) material.program = this.programs['新着色器名'];


## 坑
gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, 1); 对于一般的纹理要这么做，但是对于cubemap不需要。

# 来不及实现（懒得实现）的想法
## 纹理场的其他用途
幻影坦克！专门写一个带透明度的纹理着色器，用纹理场的方法处理。