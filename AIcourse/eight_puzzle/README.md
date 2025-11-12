# 八数码问题
完成四个函数：
1. 广度优先遍历
2. 深度有限遍历
3. 启发式搜索，启发函数为差异数目
4. 启发式搜索，启发函数为最小修改的曼哈顿度量

原来的实验代码是一坨屎，我对其进行了重构，在数据结构基本保持不变的情况下，使出了我毕生所学进行优化，主要是防止复制构造。

1. 广度优先是最简单的；还有一种升级写法是使用“双向BFS”，即在目标已知的情况下，再用一个队列从目标开始向起点搜索。
2. 深度有限要注意“恢复现场”，用栈实现的时候要回溯；更简单的写法是用递归框架，可以方便地恢复现场。
3. 启发式搜索主要是代码框架，注意遇到已经遇到过的状态怎么处理？如果还在候选中需要更新，如果已经被挑选过则跳过。

## 运行结果示例
expand为历史候选状态的数目，当 入栈/队列/集合 时自增，可以反映算法性能和内存压力。另一种计数方式是取 栈顶/队头/集合最小元素 的次数（写在while开头），指访问了多少状态。

```
========广度优先搜索========
initial State: 
1  5  4
6  3  8
2  7  0

@step 1: left
@step 2: left
@step 3: up
@step 4: right
@step 5: down
@step 6: right
@step 7: up
@step 8: up
@step 9: left
@step 10: down
@step 11: right
@step 12: up
@step 13: left
@step 14: left

final result:
0  1  2
3  4  5
6  7  8

success, steps: 14, expand: 7286

=======深度有限优先搜索=======
max depth: 25, initial State:
1  5  4
6  3  8
2  7  0

@step 1: up
@step 2: up
@step 3: left
@step 4: left
@step 5: down
@step 6: down
@step 7: right
@step 8: up
@step 9: left
@step 10: up
@step 11: right
@step 12: right
@step 13: down
@step 14: left
@step 15: up
@step 16: right
@step 17: down
@step 18: left
@step 19: down
@step 20: left
@step 21: up
@step 22: right
@step 23: up
@step 24: left

final result:
0  1  2
3  4  5
6  7  8

success, steps: 24, expand: 37018

========启发式搜索1========
initial State:
1  5  4
6  3  8
2  7  0

@step 1: left
@step 2: left
@step 3: up
@step 4: right
@step 5: down
@step 6: right
@step 7: up
@step 8: up
@step 9: left
@step 10: down
@step 11: right
@step 12: up
@step 13: left
@step 14: left

final result:
0  1  2
3  4  5
6  7  8

success, steps: 14, expand: 363

========启发式搜索2========
initial State:
1  5  4
6  3  8
2  7  0

@step 1: left
@step 2: left
@step 3: up
@step 4: right
@step 5: down
@step 6: right
@step 7: up
@step 8: up
@step 9: left
@step 10: down
@step 11: right
@step 12: up
@step 13: left
@step 14: left

final result:
0  1  2
3  4  5
6  7  8

success, steps: 14, expand: 61
```


```
========广度优先搜索========
initial State: 
1  5  8
3  2  7
6  4  0

@step 1: up
@step 2: up
@step 3: left
@step 4: down
@step 5: down
@step 6: right
@step 7: up
@step 8: up
@step 9: left
@step 10: left

final result:
0  1  2
3  4  5
6  7  8

success, steps: 10, expand: 1061

=======深度有限优先搜索=======
max depth: 25, initial State:
1  5  8
3  2  7
6  4  0

@step 1: up
@step 2: up
@step 3: left
@step 4: down
@step 5: right
@step 6: down
@step 7: left
@step 8: up
@step 9: right
@step 10: down
@step 11: left
@step 12: left
@step 13: up
@step 14: right
@step 15: right
@step 16: up
@step 17: left
@step 18: down
@step 19: left
@step 20: down
@step 21: right
@step 22: up
@step 23: up
@step 24: left

final result:
0  1  2
3  4  5
6  7  8

success, steps: 24, expand: 4318

========启发式搜索1========
initial State:
1  5  8
3  2  7
6  4  0

@step 1: up
@step 2: up
@step 3: left
@step 4: down
@step 5: down
@step 6: right
@step 7: up
@step 8: up
@step 9: left
@step 10: left

final result:
0  1  2
3  4  5
6  7  8

success, steps: 10, expand: 44

========启发式搜索2========
initial State:
1  5  8
3  2  7
6  4  0

@step 1: up
@step 2: up
@step 3: left
@step 4: down
@step 5: down
@step 6: right
@step 7: up
@step 8: up
@step 9: left
@step 10: left

final result:
0  1  2
3  4  5
6  7  8

success, steps: 10, expand: 26
```
