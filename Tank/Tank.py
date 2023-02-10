from PIL import Image, ImageEnhance


# 以real为里图 cover为表图 制作幻影坦克输出到outpath True为彩色幻影坦克
def tank(real, cover, outpath, mode=False):
    # 转为合适的亮度
    def toNiceBright(img: Image, mode: bool) -> Image:  # True为里图
        # 实际测试下来，里图平均灰度74为宜，表图平均灰度200为宜
        target = 74 if mode else 210

        def getBrightness(img: Image):      # 获取平均亮度
            img = img.convert('L')
            sum = 0
            for w in range(img.width):
                for h in range(img.height):
                    sum = sum+img.getpixel((w, h))
            return sum/img.width/img.height

        def P_to_value(x):                  # 拟合的 亮度比例-亮度传参值 曲线 在200以上值偏小 所以200变为215
            return 0.12*pow(x, 4)-0.3375*pow(x, 3)+0.4424*pow(x, 2)+0.7547*x+0.05

        return ImageEnhance.Brightness(img).enhance(P_to_value(target/getBrightness(img)))

    real = toNiceBright(Image.open(real), True)
    cover = toNiceBright(Image.open(cover).convert('L').resize(real.size), False)    # 表图黑白
    out = Image.new(mode='RGBA', size=real.size, color=(0,0,0,0))

    if mode:    # 里图彩色
        for w in range(real.width):
            for h in range(real.height):
                r, g, b = real.getpixel((w, h))
                a = 255 - cover.getpixel((w, h)) + int((r+g+b)/3)
                if a>0:
                    out.putpixel((w, h), (int(255*r/a), int(255*g/a), int(255*b/a), a))
    else:       # 里图黑白
        real = real.convert('L')        # 255纯白
        for w in range(real.width):
            for h in range(real.height):
                black = real.getpixel((w, h))
                white = cover.getpixel((w, h))
                a = 255-white+black
                if a>0:
                    grey = int(255*black/a)
                    out.putpixel((w, h), (grey, grey, grey, a))
    out.save(outpath)


tank('mask.jpg', 'inner.jpeg', 'tankk.png',True)
