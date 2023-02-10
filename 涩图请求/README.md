# 涩图请求
非常经典的请求+下载示范

- 创建时间：2022年7月3日，10:18:06
- 目的：~色色~ 学习nodejs的http请求、url图片保存（js QQ机器人的副产物）

python的比这简单多了
```
def savePicFromUrl(url,path):   # 从url下载图片
    image = requests.get(url)
    with open(path, 'wb') as f:
        f.write(image.content)

# 请求
ls = json.loads(requests.get("https://api.lolicon.app/setu/v2?r18="+r18+keyword+"&size=small&size=mini").text)['data']
```