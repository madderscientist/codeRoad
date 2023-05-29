import pickle
import codecs   # 避免乱码问题
import math

'''
显状态：{字集合}
隐状态：{'B', 'M', 'E', 'S'}
数据结构： （全是log）
    A: 4*4的矩阵 状态转移矩阵
    B: [
        {"字1": 由状态B激发字1的概率, ...},
        {...},
        ...
    ] 隐状态激发矩阵
    Pi: [P("B"), P("M"), P("E"), P("S")] 初始概率分布
'''
class HMM:
    # 静态属性和方法
    state_map = {0: "B", 1: "M", 2: "E", 3: "S",
                'B': 0, "M": 1, "E": 2, "S": 3}

    def makeLabel(text):    # 为一个词语做标注
        l = len(text)
        if l == 0:
            return []
        elif l == 1:
            return [3]
        else:
            return [0] + [1] * (l - 2) + [2]

    def fromLabel(text, label):  # label是字母不是序号 根据label进行分词
        new = []
        lastB = False
        for i in zip(text,label):
            if i[1] == 'B':
                if lastB:
                    new.append(' ')
                lastB = True
                new.append(i[0])
            else:
                new.append(i[0])
                lastB = False
                if i[1] == 'S' or i[1] == 'E':
                    new.append(' ')
        return ''.join(new)

    def __init__(self, fromFile=None):
        if fromFile != None:
            self.fromFile(fromFile)
        else:
            self.A = None   # 隐状态转移矩阵
            self.B = None   # 发射概率矩阵
            self.Pi = None  # 初始状态

    def fromFile(self, model_path):     # 读取模型
        with open(model_path, 'rb') as f:
            self.A = pickle.load(f)
            self.B = pickle.load(f)
            self.Pi = pickle.load(f)

    def toFile(self, model_path):       # 保存模型
        with open(model_path, 'wb') as f:
            pickle.dump(self.A, f)
            pickle.dump(self.B, f)
            pickle.dump(self.Pi, f)

    def train(self, train_path):    # 训练模型 按似然估计方法，不能增量学习，即每次从头开始
        # 观察train.txt得知，一行是一个句子，即“句子”是输入量。一个句子被分词，说明输出是分词结果
        with codecs.open(train_path, 'r', 'utf-8') as f:
            lines = [l.strip() for l in f.readlines()]  # 删去首尾的干扰空格
        # 初始化参数【采用似然函数法，不能迭代增量学习，所以每次训练都是重新开始】
        self.B = [{}, {}, {}, {}]
        self.A = [[0 for j in self.B] for i in self.B]
        self.Pi = [0 for i in self.B]
        # 统计相关数据
        for line in lines:  # 对每句话分析（一句一行）
            if not line:
                continue
            # 制作标签。例：sentence_state = [0,1,2,3]（即['B','M','E','S']）
            words = line.split()
            sentence_state = []
            for w in words:
                sentence_state.extend(HMM.makeLabel(w))
            words = line.replace(' ', '')
            # 统计Pi频数
            self.Pi[sentence_state[0]] += 1
            # 统计B
            for i in zip(sentence_state, words):
                try:
                    self.B[i[0]][i[1]] += 1
                except:
                    self.B[i[0]][i[1]] = 1
            # 统计A
            for i in range(0, len(sentence_state)-1):
                self.A[sentence_state[i]][sentence_state[i+1]] += 1
        # 得到ABPi (取log)
        # 1. 得到Pi
        s = sum(self.Pi) + 0.002
        for i in range(4):
            self.Pi[i] = math.log((self.Pi[i]+0.001)/s)
        # 2. 得到A
        for i in self.A:
            s = sum(i) + 0.002
            for j in range(len(i)):
                i[j] = math.log((i[j]+0.001)/s)
        # 3. 得到B
        for i in self.B:
            if not i:   # 如果是小样本可能会没有某状态
                continue
            s = math.log(sum(i.values()))
            for k in i:     # 不会有0
                i[k] = math.log(i[k]) - s

    def predict(self, text):    # Viterbi 根据字的序列, 求出隐状态
        if not (isinstance(text, str) and len(text) > 0):
            return None
        minValue = [10 * min(i.values()) for i in self.B]
        # 初始化: 由每个初始分布得到第一个字的log概率
        delta = [self.Pi[i] + self.B[i].get(text[0], minValue[i]) for i in range(4)]
        path = [[], [], [], []]    # 路径记录
        delta2 = delta[:]
        # 第二个字：对每个第二个字的状态，计算第一个字的每一个状态到其的概率，保留最大值，记录路径
        for w in range(1, len(text)):
            for i in range(4):  # 对当前的每个状态
                maxpath = 0
                maxP = -math.inf
                for j in range(4):  # 对上一步的每个状态
                    temp = delta[j] + self.A[j][i] + self.B[i].get(text[w], minValue[i])
                    if temp > maxP:  # 保留最短路径(最大log概率)
                        maxP = temp
                        maxpath = j
                path[i].append(maxpath)
                delta2[i] = maxP     # 由哪个状态转移而来
            delta = delta2[:]
        # 找最终的最大值
        maxi = 0
        maxP = -math.inf
        for i in range(4):
            if delta[i] > maxP:
                maxi = i
                maxP = delta[i]
        # 回溯路径
        result = [HMM.state_map[maxi]]
        for i in range(len(text)-2, -1, -1):
            maxi = path[maxi][i]
            result.append(HMM.state_map[maxi])
        result.reverse()
        return result
