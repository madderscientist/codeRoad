# N皇后问题

## 约束满足问题Constraint Satisfaction Problems (CSP)
概念：
- 待赋值的变量集合X：目标就是找到这些变量的取值
- 值域集合D：每个变量可以取的值的集合。每个变量只能择其一赋值。每个变量有一个
- 变量间的约束集合C
    - 绝对约束：
        - 一元约束：就是限制某个变量的取值。修建该变量的值域即可。
        - 高阶约束：几个变量一起满足某个关系
    - 偏好约束：比如有“代价”。要让某优化函数最小。
- 状态S：一种变量的赋值，即 $\{X_1 = D_3, X_2 = D_1, X_3 = ...\}$。某变量未赋值也是状态的一部分，比如初始状态为 $\{\}$。

CSP就是要找到S使得所有变量都有值(称为完全)、且满足约束C。

### 回溯搜索
也就是提前终止的DFS。以填色问题为例，这里认为传统DFS在每个块都分配完颜色后再判断是否可行，显然是非常愚蠢的做法。正常人的做法是在某一层发现不行就不用继续探索了。

伪代码（书上的怎么这么难看）
```typescript
// 值域 为了表现离散与一般化，使用了枚举类型
enum Domain {
    Value1,
    Value2,
    Value3,
    // ...
}

// Solution 就是一个赋值的映射
type Solution = state: Map<Variable, Domain>;

// 是变量、约束的集合
class CSP {
    variables: Array<Variable>,
    constructor() {
        this.variables = [
            // new Variable()
            // ...
        ];
    }
    check(state: Solution): bool {
        // 检查赋值是否能满足约束
    }
    selectNext(state: Solution): Variable {
        // 根据某种策略，在variables中选择一个不在Solution中的Variable
        // 策略的选择见下面（MRV、Degree）
    }
    getDomain(state: Solution, v: Variable): Array<Domain> {
        // 根据约束，返回v的可能取值，要求满足约束
        // 值的顺序可以用启发式LCV
    }
}

// null 表示搜索失败
function 递归回溯搜索(state: Solution, csp: CSP): Solution | null {
    if (csp.check(state)) return state;
    const v: Variable = csp.selectNext(state);  // 选本轮要决定值的变量
    const domain: Array<Domain> = csp.getDomain(state, v);  // 该变量的可能取值
    for(const i of domain) {
        state.set(v, i);
        const solution = 递归回溯搜索(state, csp);
        if (solution != null) return state;
        state.delete(v);
    }
    return null;    // domain都不行
}
// 用法
const solution: Solution = 递归回溯搜索(new Map(), new CSP());
```
这里用的是最省内存的做法。但是对于更复杂的算法，可能在递归前一定需要复制一份原状态，比如下面的算法。

有几个问题：

1. 顺序试探？还是按照别的什么顺序？
    - 变量选择顺序selectNext
        - 最受约束变量(MRV)启发式：选择合法取值少的变量。比如A有4种可选，B有2种可选，肯定先选B啊，因为一次决断消除的不确定性最多
        - 度启发式：选择约束别人最多的变量。也是做一次决断的消除的不确定性最多
    - 值选择顺序
        - 最少约束值(LCV)启发式：尽量选已经出现过值，给后面的留下更多自由选择的空间

2. 真的要遍历每一个？可能可以减少搜索空间：取值前就发现矛盾
    - 前向检验：选择了一个赋值，就据此修剪被影响的未赋值的变量的值域。再用不复制的方式实现
    - 弧相容：指的是有向约束中，起点的值域中每个值都有终点值域中的值满足约束。例如当前选择A准备赋值，根据约束，A可以影响B、C、D，根据约束。遍历A的值域，如果存在不相容的值就删掉。这其实是判断A的各个取值是否可行。如果递归做下去就可以精简后面节点的值域，也就是递归进行的前向检验（真的有必要吗？）

弧相容AC-3伪代码，采用的是复制的方法回溯，仅支持二元约束：
```ts
class Variable {
    // 值域
    domain: Set<Domain>;    // 如果只有一个值，认为已经赋值了
    constructor(domain: Set<Domain> = new Set()) {
        this.domain = domain;
    }
    // 用于DFS时“恢复现场”
    copy(): Variable {
        // 深拷贝变量对象，包括 domain 集合和值
        return new Variable(new Set(this.domain));
    }
};

class CSP {
    variables: Array<Variable>;
    arcs: Array<[number, number]>;  // 有向弧

    // 判断两个值是否满足约束，有向，会修改v1的值域
    // 返回是否修改值域
    static consistent(v1: Variable, v2: Variable): bool {
        let deleteAny = false;
        for (const d of v1.domain) {
            let findConsistent = false
            for (const d2 of v2.domain) {
                // 考察d和d2是否满足约束
                // 如果找到满足的就置findConsistent为true
            }
            if (findConsistent == false) {
                v1.domain.delete(d);
                deleteAny = true;
            }
        }
        return deleteAny;
    }

    constructor() {
        this.varables = [
            // ...
        ];
        this.arcs = [
            [0, 1], [1, 0], // 无向边两个都要加
            [2, 5]  // 有向边只要加一个
        ];
    }
    
    copy(): CSP {
        const newCSP = new CSP();
        newCSP.variables = this.variables.map(v => v.copy());
        newCSP.arcs = this.arcs;    // 这个不会变，不用拷贝，直接引用
        return newCSP;
    }

    // 有副作用：会修改值域
    AC3(): bool {
        const queue = [...this.arcs];   // 拷贝一份
        while (queue.length) {
            const arc = queue.shift();  // 取第一个并pop
            const v1 = this.variables[arc[0]]
            if (CSP.consistent( // 这里修改了v1的值域
                v1,
                this.variables[arc[1]]
            )) {
                if (v1.domain.size == 0) return false;  // 没有可用值了！
                // (v1, v2)发生了值域变动，需要重新考虑(vk, v1)
                for (const arcc of this.arcs) {
                    if (arcc[1] == arc[0]) {
                        queue.push(arcc);
                    }
                }
            }
        }
        return true;
    }

    selectNext(): Variable {
        // 在值域大小非1的variables中，用某种策略选择一个
        // AC3保证不会有值域为空的情况
    }
}

// null 表示搜索失败
function 递归回溯搜索(csp: CSP): CSP | null {
    if (csp.check()) return csp;
    const v = csp.selectNext();  // 选本轮要决定值的变量
    const oldDomain = v.domain;
    for(const i of oldDomain) {
        v.domain = new Set([i]);    // 值域只保留一个，视为取值
        const copyCSP = csp.copy();
        if(copyCSP.AC3()) {
            const solution = 递归回溯搜索(copyCSP);
            if (solution != null) return solution;
        }
    }
    v.domain = oldDomain;
    return null;    // domain都不行
}
// 用法
const solution: CSP = 递归回溯搜索(new CSP());
```

## 局部搜索
初始状态是给每个变量都赋一个值，搜索过程是一次改变一个变量的取值。八皇后问题为例

```ts
class Variable {
    value: Domain;  // 当前取值
    domain: Set<Domain>;
}

function MIN-CONFLICTS(csp: CSP, max_steps = 10000): CSP | null {
    // csp的每个变量已经有值了
    for (let i = 0; i < max_steps; i++) {
        if (scp.check()) return csp;
        const v: Variable = csp.choseNext();    // 找一个冲突的变量
        v.value = csp.argminConflicts(v);   // 值域里选一个最小冲突的
    }
    return null;
}
```

## 解题
给出的模板代码总算像样了（和上一个实验相比）