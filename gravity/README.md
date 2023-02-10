# HTML重力模拟

- 创建时间：2022年3月12日，15:18:24
- 目的：模拟重力，以后做物理系统可以参考（在我另一个仓库KiloGraph中用到了这次的经验）
- 思路：每个球的计算用异步。delta t为settimeout的20，每次下落t*speed，并且速度变大。难点在于反弹。
- 使用：http://htmlpreview.github.io/?https://github.com/madderscientist/codeRoad/blob/main/gravity/gravity.html<br>
单击绿色区域即可创建一个落下的球