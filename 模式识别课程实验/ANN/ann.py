# 训练模型
import gzip
import numpy as np
import json
import nn
from tqdm import tqdm


def load_data():    # 读取到npArray, 每维是一个28*28, 0~255取值的数组。
    with gzip.open('../data/train-images-idx3-ubyte.gz', 'rb') as f:
        train_images = np.frombuffer(
            f.read(), np.uint8, offset=16).reshape(-1, 28*28)
    with gzip.open('../data/train-labels-idx1-ubyte.gz', 'rb') as f:
        train_labels = np.frombuffer(f.read(), np.uint8, offset=8)
    with gzip.open('../data/t10k-images-idx3-ubyte.gz', 'rb') as f:
        test_images = np.frombuffer(
            f.read(), np.uint8, offset=16).reshape(-1, 28*28)
    with gzip.open('../data/t10k-labels-idx1-ubyte.gz', 'rb') as f:
        test_labels = np.frombuffer(f.read(), np.uint8, offset=8)
    return train_images, train_labels, test_images, test_labels


train_images, train_labels, test_images, test_labels = load_data()
# 数据加入偏置，标签改为独热码
train_labels = np.stack([nn.one_hot(i, 10) for i in train_labels])
test_images = np.stack(
    [np.insert(i.astype('float64')/255, 0, 1) for i in test_images])
train_images = np.stack(
    [np.insert(i.astype('float64')/255, 0, 1) for i in train_images])
# %%
m = nn.nn([
    nn.Dense(28*28+1, 201, nn.Adam(0.0003), nn.relu, nn.drelu, nn.Kaiming),
    nn.Dense(201, 10, nn.Adam(0.0003), nn.sigmoid, nn.dsigmoid)
])

# 乱序分批
def iterate(X, Y, batch_size=100):
    idx = np.arange(len(X))
    np.random.shuffle(idx)
    return [([X[idx[i:i+batch_size]], Y[idx[i:i+batch_size]]]) for i in range(0, len(X), batch_size)]

# 批训练
def batch_train():
    bar = iterate(train_images, train_labels, 10)
    accs = []
    for sample in tqdm(bar):
        X, Y = sample
        res = m.fp(X)
        m.bp(Y)
        # 计算一个batch中的正确率
        acc = np.mean(np.argmax(res, axis=-1) == np.argmax(Y, axis=-1))
        accs.append(acc)
    print(f"train acc: {np.mean(accs)}")

# 验证
def test():
    right = 0
    for i in range(len(test_labels)):
        t = np.argmax(m.fp(test_images[i]))
        if t == test_labels[i]:
            right += 1
    return right/len(test_labels)


# %%
import os
if not os.path.exists('./model'):
    os.makedirs('./model')

for i in range(10):     # 训练
    batch_train()
    print(f"epoch_{i} test acc: {test()}")
    with open(f'./model/model_{i}.json', 'w', encoding="utf-8") as f:
        json.dump(m.toJson(), f)
