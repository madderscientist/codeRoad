import mindspore.dataset as ds
import mindspore.dataset.text as text

with open('../data/test.txt','r',encoding='utf-8') as f:
    input_list = f.readlines()
dataset = ds.NumpySlicesDataset(input_list, column_names=["text"], shuffle=False)

# load model(pre trained)
HMM_FILE = "./hmm_model.utf8"
MP_FILE = "./jieba.dict.utf8"
jieba_op = text.JiebaTokenizer(HMM_FILE, MP_FILE)
dataset = dataset.map(operations=jieba_op, input_columns=["text"], num_parallel_workers=1)

with open('MindSpore_fenci_result.txt','w',encoding='utf-8') as f:
    for i in dataset.create_dict_iterator(num_epochs=1, output_numpy=True):
        f.write(' '.join(i['text']))