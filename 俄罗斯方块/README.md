# 俄罗斯方块

- 创建时间：2021年12月26日，14:57:06（大一上C++期末考试复习时写的

- 目的：练习数组使用，了解easyx图形库使用

- 思路：
[参考](https://kevinkun.cn/tetrix/)<br>
使用一维数组记录“棋盘”，每位存颜色，边界存9。<br>
position[7][4][4]保存了7种组合，每种组合有4个方向，每个方向占4个格子；每一位是相对于中心的位移。<br>
烦点在于判断是否能落下（是否创建下一个），和满格之后的消去。遍历即可。<br>
每次刷新，根据“棋盘”存储的数据，重画所有格子，再画当前落下的那个。
- 使用：用英文输入法，asd（平移）和qe（转）

- 成果：

1. 读取键盘信息：char down = _getch(); switch (down) {...}
2. 创建线程用于键盘输入：CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)keydown, NULL, NULL, NULL);