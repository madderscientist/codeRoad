# 三维渲染

- 创建时间：2022年3月5日，9:12:43
- 目的：初探计算机图形学，尝试使用线性代数的知识解决问题
- 使用：http://htmlpreview.github.io/?https://github.com/madderscientist/codeRoad/blob/main/3d_show/3d.html
- 成果：**浏览器全屏显示**、鼠标移动跟踪怎么做

## 补点数学

### 坐标变换

已知在S系中，有点A(x,y)；若S'系中，S系的基底在(m,n)和(a,b)处，那点A在S'系的哪里？<br>
根据线性组合的原理，x' = x*m + y*a, y'= x*n + y*b。写成矩阵就是：

```
[x',y']^T = [[m,n]^T,[a,b]^T] * [x,y]^T 
```

### 投影

现在要把3维点A投射到2维的屏幕。<br>
xhat和yhat是投射平面的基底在空间中的坐标，xhat始终在x-y平面内，yhat和z轴有夹角a2，和xhat成90度，平面原点和空间坐标系原点重合。<br>
用最小二乘法解决。即求平面上一点B，使vector(OA)-vector(OB)与vector(OB)垂直。此时AB垂直于平面。<br>
vector(OB)可以表示为x*xhat+y*yhat，(x,y)即为B在平面上的坐标；<br>
点乘为零，移项求解即可。
