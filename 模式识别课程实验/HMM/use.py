from hmm import HMM
m = HMM()
train_path = "../data/199801_train.txt"
test_path = "../data/test.txt"
# %%
# 训练并保存
print("training now...")
m.train(train_path)
m.toFile('./hmm_model.pkl')
print("the model has been saved to hmm_model.pkl")
# %%
# 使用模型分词
# m = HMM(fromFile='./hmm_model.pkl')
import codecs
with codecs.open(test_path, 'r', 'utf-8') as f:
    lines = [l.strip() for l in f.readlines()]  # 删去首尾的干扰空格
print("predicting now...")
with codecs.open("hmm_result.txt", 'w', 'utf-8') as f:
    for i in lines:
        x = m.predict(i)
        f.write(HMM.fromLabel(i, x)+'\n')
print("The result has been saved to hmm_result.txt")