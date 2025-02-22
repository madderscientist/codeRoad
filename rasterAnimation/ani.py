# 根据帧制作图片和遮罩
from PIL import Image
import os

interval = 2    # 镂空部分像素数
framedir = './frames'
frames = os.listdir(framedir)
frames.sort()

outImg = None
for i in range(0, len(frames)):
    img = Image.open(os.path.join(framedir, frames[i]), mode='r')
    if not outImg:
        outImg = Image.new('RGBA', img.size, 'rgba(0,0,0,0)')
    for w in range(img.width):
        if int(w/interval) % len(frames) == i:
            for h in range(0, img.height):
                outImg.putpixel((w, h), img.getpixel((w, h)))
outImg.save('animation.png')

mask = Image.new('RGBA', (2*outImg.width,outImg.height+4), 'rgba(0,0,0,255)')
for w in range(mask.width):
    if int(w/interval) % len(frames) == 0:
        for h in range(0, mask.height):
            mask.putpixel((w, h), (0, 0, 0, 0))
mask.save('mask.png')
