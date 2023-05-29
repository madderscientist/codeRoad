import numpy as np
import base64
import dill     # 保存函数用

# 保存函数到文件
def function_str(f):
    return base64.b64encode(dill.dumps(f)).decode('utf-8')

def str_function(functionString):
    return dill.loads(base64.b64decode(functionString))

# 转独热码 如数字8(第7类)对应00000001000
def one_hot(Y, n_classes):
    return np.eye(n_classes)[Y].astype(float)

'''
激活函数activation function
'''
# 激活函数relu
def relu(A, l=0.05):
    return np.maximum(A, A * l)

# relu的导数
def drelu(A, l=0.05):
    return np.where(A > 0, 1., l)

# 激活函数sigmoid
def sigmoid(x):
    return 1 / (1 + np.exp(-x))

# sigmoid的导数
def dsigmoid(x):
    temp = np.exp(-x)
    return temp / (1 + temp)**2

'''
误差函数loss function
'''
# 误差函数 x: 输入 t: 目标
def L2(x, t):
    return 0.5*((x-t)**2)

def dL2(x, t):
    return x-t

'''
权值初始化
'''
def Xavier(size):
    bound = np.sqrt(6/(size[0]+size[1]))
    return np.random.uniform(-bound, bound, size)

def Kaiming(size):  # 查资料得知对relu层应使用此方法
    return np.random.normal(0, np.sqrt(2/size[0]), size)

'''
优化器
必须实现：
- getParams(params)【由层向优化器传递优化权值】
- step(d)【由层向优化器传递梯度】
- toJson()【返回参数json】
- fromJson(js)【由json构造优化器】
'''
class SGD:
    def __init__(self, lr=0.001, settingJson=None) -> None:
        if settingJson != None:
            self.fromJson(settingJson)
        else:
            self.lr = lr
            self.params = None

    def getParams(self, params):
        self.params = params

    def step(self, grad):
        self.params -= self.lr*grad

    def toJson(self):
        return {
            "type": "SGD",
            "lr": self.lr
        }

    def fromJson(self, js):
        self.lr = js["lr"]
        self.params = None

class Adam:
    def __init__(self, lr=3e-4, beta1=0.9, beta2=0.999, settingJson=None) -> None:
        if settingJson != None:
            self.fromJson(settingJson)
        else:
            self.lr = lr
            self.beta1 = beta1
            self.beta2 = beta2
            self.t = 0.
            # 以下参数由 getParams给出具体的值
            self.params = None
            self.m = None
            self.v = None

    def getParams(self, params):    # 由所属层的构造函数调用
        self.params = params
        self.m = np.stack([np.zeros_like(x, dtype=float) for x in params])
        self.v = np.stack([np.zeros_like(x, dtype=float) for x in params])

    # 输入：dJ/dw 对 self.params 进行优化
    def step(self, grad):
        self.t += 1
        self.m[:] = self.m * self.beta1 + grad * (1 - self.beta1)
        self.v[:] = self.v * self.beta2 + (grad ** 2) * (1 - self.beta2)
        mh = self.m / (1 - self.beta1 ** self.t)
        vh = self.v / (1 - self.beta2 ** self.t)
        self.params -= self.lr * mh / (np.sqrt(vh) + 1e-10)

    def toJson(self):   # 导出为json格式
        return {
            "type": "Adam",
            "lr": self.lr,
            "t": self.t,
            "m": self.m.tolist(),
            "v": self.v.tolist(),
            "beta": [self.beta1, self.beta2]
        }

    def fromJson(self, js):   # 导入json构造
        self.lr = js["lr"]
        self.beta1, self.beta2 = js["beta"]
        self.t = js["t"]
        self.m = np.array(js["m"])
        self.v = np.array(js["v"])
        self.params = None

'''
层
必须实现：
属性：
- inputNum 【输入维度】
- outputNum 【输出维度】
- x 【输入 指针，由网络类分配连接】
- z 【输出，指针不可变】
- w 【存储参数】
函数：
- fp(x)【正向传播】
- bp(t)【反向传播，调用优化器实现更新】
- toJson()【导出层参数为json，嵌套优化器json】
- fromJson(js)【从json建立本层】
'''
class Dense:
    def __init__(self, input=10, output=10, optimizer=None, f=relu, df=drelu, ini = Xavier, settingJson=None) -> None:
        if settingJson != None:     # 来自文件
            self.fromJson(settingJson)
        else:
            self.inputNum = input
            self.outputNum = output
            self.f = f
            self.df = df
            self.x = None       # 输入。和上一级的输出为同一个数组。由nn类连接。{"value": ndarray}
            self.y = None       # 净激活能 np.array 值在正向传播时确定
            self.z = {"value": None}   # 输出 和下一层共用, 故地址不可变 为了兼容批量训练, 改为此形式
            # self.w = np.random.uniform(-0.1, 0.1, (output, input))
            self.w = ini((output, input))
            optimizer.getParams(self.w)
            self.optim = optimizer

    def fp(self):   # 正向传播 要求x["value"]已经有np.array 支持批量输入
        self.y = self.x["value"] @ self.w.T
        self.z["value"] = np.array([self.f(k) for k in self.y])

    # 输入：dJ/dz; 输出：dJ/dx
    # 传递 dJ/dw 给 self.optimizer 完成优化
    def bp(self, d):    # 反向传播 d为dJ/dz, 由下一级给出 支持批量输入
        djdy = d*[self.df(i) for i in self.y]
        djdx = djdy@self.w
        if djdy.ndim == 2:
            djdw = np.mean([np.outer(djdy[i], self.x["value"][i]) for i in range(len(djdy))], axis=0)
        else:
            djdw = np.outer(djdy, self.x["value"])
        self.optim.step(djdw)
        return djdx

    def toJson(self):   # 将参数导出为json格式
        return {
            "type": "Dense",
            "input": self.inputNum,
            "output": self.outputNum,
            "param": self.w.tolist(),
            "f": function_str(self.f),
            "df": function_str(self.df),
            "optim": self.optim.toJson()
        }

    def fromJson(self, js):   # 从json得到权值
        self.w = np.array(js["param"])
        self.f = str_function(js["f"])
        self.df = str_function(js["df"])
        self.inputNum = js["input"]
        self.outputNum = js["output"]
        self.x = None
        self.y = None
        self.z = {"value": None}
        # 注意，global为本文件的命名空间。所以以后拓展的优化器得写本文件中
        self.optim = globals()[js["optim"]["type"]](settingJson=js["optim"])
        self.optim.getParams(self.w)

'''
网络
'''
class nn:
    '''
    传参示例：layers = [
        Dense(28*28, 200, Adam(), relu, drelu, Kaiming),
        Dense(200, 200, Adam(), sigmoid, dsigmoid),
        Dense(200, 10, Adam(), relu, drelu, Kaiming)
    ]
    '''
    def __init__(self, layers=None, loss=L2, dloss=dL2, settingJson=None) -> None:
        if settingJson != None:     # 来自文件
            self.fromJson(settingJson)
        else:
            self.loss = loss
            self.dloss = dloss
            self.layers = layers
            self.input = {"value": None}  # 输入层 地址不可变
            # 连接所有层
            temp = self.input
            outNum = layers[0].inputNum
            for i in layers:
                if i.inputNum == outNum:
                    i.x = temp
                    temp = i.z
                    outNum = i.outputNum
                else:
                    raise Exception("connection demension error!")
            self.output = temp

    def fp(self, x):
        self.input["value"] = x
        for i in self.layers:
            i.fp()
        return self.output["value"]

    def bp(self, t):
        d = self.dloss(self.output["value"], t)
        for i in range(len(self.layers)-1, -1, -1):
            d = self.layers[i].bp(d)

    def toJson(self):
        return {
            "param": [l.toJson() for l in self.layers],
            "f": function_str(self.loss),
            "df": function_str(self.dloss),
        }

    def fromJson(self, js):
        self.loss = str_function(js["f"])
        self.dloss = str_function(js["df"])
        self.layers = [globals()[l["type"]](settingJson=l)
                       for l in js["param"]]
        self.input = {"value": None}
        # 连接所有层
        temp = self.input
        outNum = self.layers[0].inputNum
        for i in self.layers:
            if i.inputNum == outNum:
                i.x = temp
                temp = i.z
                outNum = i.outputNum
            else:
                raise Exception("connection demension error!")
        self.output = temp


if __name__ == "__main__":      # 测试用
    import json
    # 创建神经网络
    m = nn([
        Dense(2, 2, SGD(), sigmoid, dsigmoid),
        Dense(2, 1, SGD(), sigmoid, dsigmoid)
    ])
    # 保存到文件
    with open('nntest.json', 'w', encoding="utf-8") as f:
        json.dump(m.toJson(), f)
    # 从文件读取网络
    with open("nntest.json", "r", encoding="utf-8") as f:
        nnn = json.load(f)
    m = nn(settingJson=nnn)
    # 使用网络
    print("out: ", m.fp(np.array([2, 4])))
    m.bp(np.array([1]))
    print(f"after bp: {m.fp()}")
    # 保存更改
    with open('nntest2.json', 'w', encoding="utf-8") as f:
        json.dump(m.toJson(), f)
