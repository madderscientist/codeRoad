# 学习Three.js的程序架构
three.js如何添加天空盒？
一个物体的纹理材质网格如何管理？
光照怎么加？

## 物体
创建物体：
```js
new THREE.Mesh(Geometry, Material);
```
### Geometry
three.js的所有内置的几何体都内置了UV映射。
```js
var Geometry = new THREE.BoxGeometry (5, 5, 5);
```

### Material
有很多种：
- 网格基础材质(MeshBasicMaterial)
该材质不受光照的影响，不需要光源即可显示出来，设置颜色后，各个面都是同一个颜色。
- 网格法向材质(MeshNormalMaterial)
该材质不受光照的影响，不需要光源即可显示出来，并且每个方向的面的颜色都不同，同但一个方向的面颜色是相同的，该材质一般用于调试。
- 网格朗博材质(MeshLambertMaterial)
该材质会受到光照的影响，没有光源时不会显示出来，用于创建表面暗淡，不光亮的物体。
- 网格 Phong 材质(MeshPhongMaterial)
该材质会受到光照的影响，没有光源时不会显示出来，用于创建光亮的物体。
例子：
```js
var Material = new THREE.MeshPhongMaterial({
  color: "yellow"
});
```
使用纹理：
```js
const textureLoader = new THREE.TextureLoader();
const colorTexture = textureLoader.load('path/to/color_texture.jpg');
const material = new THREE.MeshBasicMaterial({
  map: colorTexture
});
```
如果要用法线贴图、高度贴图，则多加normalMap、displacementMap属性：
```js
const normalTexture = textureLoader.load('./images/door/normal.jpg');
const colorTexture = textureLoader.load('path/to/color_texture.jpg');
const displacementTexture = textureLoader.load("./images/door/height.jpg");
const planeMaterial = new THREE.MeshStandardMaterial({
  map: colorTexture,
  normalMap: normalTexture, // 增加法线纹理
  displacementMap: displacementTexture,// 增加高度纹理
  metalnessMap: ...
});
```
如果又设置了map又设置了color，则片元着色器中两个相乘。
以下重点：
不同的材质类别意味着需要不同的着色器。
Three.js会根据每个物体的材质类型选择对应的着色器程序进行渲染。如果一个场景中的物体使用了不同类型的材质，那么Three.js在渲染这些物体时会切换不同的着色器程序。
例如，如果场景中有两个物体，一个使用MeshBasicMaterial，一个使用MeshPhongMaterial，那么在渲染这两个物体时，Three.js会先使用MeshBasicMaterial对应的着色器程序渲染第一个物体，然后切换到MeshPhongMaterial对应的着色器程序渲染第二个物体。
需要注意的是，频繁切换着色器程序可能会影响渲染性能。因此，Three.js在内部使用了一些优化策略，例如按材质类型对物体进行排序，以减少着色器程序切换的次数。


## 动态光源
[参考](https://www.zhihu.com/question/40466325)中的源码回答下面的评论：

        若光源(点光源数目，聚光灯数目，平行光数目三者组成的一个结构体)发生改变则重新生成编译着色器

所以是动态更新着色器的。

## 天空盒
