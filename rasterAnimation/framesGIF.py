# 获取gif的帧
from PIL import Image
im = Image.open('g.gif')
try:
    while 1:
        im.seek(im.tell()+1)
        im.save(f"./frames/{im.tell()}.png")
except EOFError:
    pass