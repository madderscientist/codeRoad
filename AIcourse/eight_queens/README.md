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
给出的模板代码总算像样了（和上一个实验相比）。照着伪代码无脑写就好了。

注意框架返回了诸多`NULL`（竟然不是`nullptr`，这作者有点落后了），别想那么多有没有必要、可不可能，全部加上判断就对了。

```cpp
std::vector<Queen*> search::backtrack(std::vector<Queen*> assignment, Csp& csp) {
    /*
	 * TODO
	 * Algorithm (Reference: Figure 6.5):
	 function BACKTRACK(assignment, csp) returns a solution, or failure
		if assignment is complete then return assignment (use this condition: assignment.size() == csp.variables.size())
		var<-SELECT-UNASSIGNED-VARIABLE(csp)
		for each value in ORDER-DOMAIN-VALUES(var, assignment, csp) do
			record csp state # csp.recode() require two variables, you need to create two local variables to store the state
			if value is consistent with assignment then
				assign value to var	# use var->assign(value)
				add var to assignment
				inferences<-INFERENCE(csp, var, value)	# use makeInference function here
				if inferences != failure then
					add inferences to assignment
					result<-BACKTRACK(assignment, csp)
					if result != failure then
						return result
			recover csp state (csp.recover)
			remove {var = value} and inferences from assignment # use refresh(assignment)
		return failure
	 */
	if (assignment.size() == csp.variables.size()) return assignment;
	auto var = search::selectUnassignedVariable(csp);
	auto domain = search::orderDomainValues(var, assignment, csp);
	for (auto value : domain) {
		std::vector<Position> lastPositions;
		std::vector<std::vector<Position>> lastDomains;
		csp.record(lastPositions, lastDomains);
		if (csp.consistent(value, assignment)) {
			var->assign(value);
			assignment.push_back(var);
			auto inference = search::makeInference(csp, var, value);
			if (!failed(inference)) {
				for (auto v : inference) assignment.push_back(v);
				auto result = search::backtrack(assignment, csp);
				if (!failed(result)) return result;	// 成功！
			}
		}
		// 恢复现场只要这两行即可：
		// csp.recover会将变量的赋值撤销，而assignment里面保存的是变量的指针
		// 所以csp.recover后，push_back到assignment里面的变量都会复原成未赋值，然后被refresh函数删除
		csp.recover(lastPositions, lastDomains);
		search::refresh(assignment);    // refresh删除的是没有赋值的变量
	}
    return std::vector<Queen*>({NULL});
}
```

```cpp
std::vector<Queen*> search::minConflict(Csp& csp, int maxSteps) {
    /*
	 * TODO
	 * Algorithm (Reference: Figure 6.8):
	 function MIN-CONFLICTS(csp,max steps) returns a solution or failure
		inputs: csp, a constraint satisfaction problem
				max steps, the number of steps allowed before giving up
		current<-an initial complete assignment for csp	
		for i = 1 to max steps do
			if current is a solution for csp then # use isSolution
				print how many steps used here
				return current 
			var <- a randomly chosen conflicted variable from csp.VARIABLES # use chooseConflictVariable
			value <- the value v for var that minimizes CONFLICTS(var, v, current , csp) # use getMinConflictValue
			set var =value in current	# use var->position = value
		return failure
	 */
	std::vector<Queen*>& current = csp.variables;
	for (int i = 0; i < maxSteps; i++) {
		if (isSolution(csp, current)) {
			std::cout << "Success at step " << i << '\n';
			return current;
		}
		Queen* var = chooseConflictVariable(csp);
		// 其实var必找到，因为已经不是Solution了 但是为了鲁棒性还是加一句判断
		if (var == NULL) return current;	// 没有找到冲突，说明符合要求
		// 找冲突最小的
		auto value = getMinConflictValue(csp, var);
		var->position = value;
	}
    return std::vector<Queen*>({NULL});
}
```

```cpp
int search::getConflicts(Csp& csp, Position& position) {
    /*
	* TODO
	* 得到一个position在当前棋盘上的冲突数量
	* 注意：与position在同一列的queen的冲突不应该计算
	* 样例：
	*	0 1 0 0
		1 0 0 0
		0 0 1 0
		0 0 0 1
	* Position{0, 0}的冲突数应该为3，因为它与{0, 1},{2, 2},{3, 3}冲突
	* Position{1, 0}的冲突数量应该为1，因为它与{0, 1}冲突
	*/
	int conflicts = 0;
	for (auto q : csp.variables) {
		// 跳过同一列
		if (q->position.col == position.col) continue;
		// 跳过未赋值
		if (q->position == Position::getUnassigned()) continue;
		// 有冲突则计数 csp.constraints = relation::conflict
		if (csp.constraints(q->position, position)) conflicts++;
	}
    return conflicts;
}
```

```cpp
Queen* search::chooseConflictVariable(Csp& csp) {
    /*
	* TODO
	* 返回一个目前赋值的冲突数大于0的variable
	* 注意：冲突数大于0的variable可能有多个，需要随机选择
	* 样例：
	*	0 1 0 0
		1 0 0 0
		0 0 1 0
		0 0 0 1
	* Queen1-4的冲突数都大于0，随机选择一个作为该函数的返回结果
	*/
	std::vector<Queen*> conflicted;
	for (Queen* q : csp.variables) {
		if (q->position == Position::getUnassigned()) continue;
		if (getConflicts(csp, q->position)) conflicted.push_back(q);
	}
	if (conflicted.empty()) return NULL;
	return conflicted[std::rand() % conflicted.size()];
}
```

```cpp
Position search::getMinConflictValue(Csp& csp, Queen* var) {
    /*
	* TODO
	* 返回var的domian中，可以使冲突数最小的值
	* 注意：使冲突数最小的值可能有多个，需要随机选择，如果不随机选择问题可能会陷入局部稳定点并且该稳定点不是解
	* 样例：
	*	1 1 0 0
		0 0 0 0
		0 0 1 0
		0 0 0 1
	* Queen1所在的位置的冲突数为3，它的domain为{[0-3], 0}。{1, 0},{2, 0},{3, 0}的冲突数都为1。
	* 需要从中随机选取一个作为返回值。
	*/
	std::vector<Position> minConflicts;
	int minConflictNum = INT_MAX;
	for (auto p : var->domain) {
		int conflicts = getConflicts(csp, p);
		if (conflicts == minConflictNum) {
			minConflicts.push_back(p);
		} else if (conflicts < minConflictNum) {
			minConflicts.clear();
			minConflicts.push_back(p);
			minConflictNum = conflicts;
		}
	}
	// 空则返回无解 但其实不会出现，因为必有比INT_MAX小的
	if (minConflicts.empty()) return Position::getUnassigned();
	return minConflicts[std::rand() % minConflicts.size()];
}
```

值得注意的是这个框架的AC3算法是错的，有向弧的方向反了。改为如下才是正确的：
```cpp
bool inference::revise(Csp& csp, Queen& q1, Queen& q2) {
    bool revised = false;
    auto it = q1.domain.begin();
    while (it != q1.domain.end()) {
        if (!canSatisfy(csp, *it, q2)) {
            it = q1.domain.erase(it);
            revised = true;
        } else {
            it++;
        }
    }
    return revised;
}
```