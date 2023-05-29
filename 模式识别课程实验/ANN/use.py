# 使用训练好的神经网络预测
# 加载数据
import json
import nn
import gzip
import numpy as np

def load_data():
    with gzip.open('../data/t10k-images-idx3-ubyte.gz', 'rb') as f:
        test_images = np.frombuffer(
            f.read(), np.uint8, offset=16).reshape(-1, 28*28)
    with gzip.open('../data/t10k-labels-idx1-ubyte.gz', 'rb') as f:
        test_labels = np.frombuffer(f.read(), np.uint8, offset=8)
    return test_images, test_labels

test_images, test_labels = load_data()
# 加偏置
test_images_b = np.stack([np.insert(i.astype('float64')/255, 0, 1) for i in test_images])
# %%
# 加载网络
with open("./model4/model_10batch_final.json", "r", encoding="utf-8") as f:
    nnn = json.load(f)
m = nn.nn(settingJson=nnn)
# %%
# 识别
result = []
wrong = []  # 错误的编号
for i in range(len(test_labels)):
        t = np.argmax(m.fp(test_images_b[i]))
        result.append(t)
        if t != test_labels[i]:
            wrong.append(i)
print(f"Identified {len(result)} pictures, wrong {len(wrong)} times, accuracy: {1-len(wrong)/len(result)}")
print("Identification completed. Waiting for result saving.")
# %%
# 保存结果
with open('result.csv','w',encoding="utf-8") as f:
    f.write("label,ann_result\n")
    for i in range(len(test_labels)):
         f.write(f"{test_labels[i]},{result[i]}\n")
print("result saved to result.csv")
# %%
# 保存识别错误的图片
import matplotlib.pyplot as plt
fig, axs = plt.subplots(len(wrong)//5+1, 5, figsize=(15, 0.4*len(wrong)))
for i in range(len(wrong)):
    axs[i//5, i%5].imshow(test_images[wrong[i]].reshape(28,28), cmap='gray')
    title = f"Predicted:{result[wrong[i]]},Actual:{test_labels[wrong[i]]}"
    axs[i//5, i%5].set_title(title)
plt.subplots_adjust(hspace=0.5)
fig.savefig("wrong_classified.jpg", dpi=200)
print("wrongly classified pictures were saved to wrong_classified.jpg")
