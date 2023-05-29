# 将199801语料库转换为只有分词的结果
import codecs
with codecs.open('../data/199801/199801.txt', 'r', encoding='gb2312', errors='ignore') as f:
    lines = f.readlines()

import unicodedata
def to_halfwidth(text): # 数字字母转半角
    result = ""
    for char in text:
        if unicodedata.east_asian_width(char) in ('F', 'W'):
            code = ord(char)
            if 65281 <= code <= 65374:
                halfwidth_char = chr(code - 65248)
                result += halfwidth_char
                continue
        result += char
    return result

import re
pattern = r"/[a-zA-Z]+|\][a-zA-z]+|\["
with open('../data/199801_train.txt', 'w', encoding='utf-8') as f:
    for l in lines:
        f.write(re.sub(pattern, "", to_halfwidth(l[23:])))