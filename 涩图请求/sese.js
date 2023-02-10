const https = require("https");
const fs = require("fs");
function savePicture(url, name = '涩图.png') {
    https.get(url, (response) => {
        var data = "";
        response.setEncoding("binary");
        response.on('data', function (chunk) {
            data += chunk;
        });
        response.on("end", function () {
            fs.writeFile(name, data, 'binary', (err) => {
                if (err) console.log('写入文件错误');
                else console.log('写入文件成功', name);
            })
        });
    }).on("error", function () {
        console.log('读取错误:', url, name);
    });
}
function sese(key) {   //在这里输入涩图的关键词。如果写“&r18=1”会有奇效哦
    if (key) key = "?tag=" + key;
    https.get("https://api.lolicon.app/setu/v2" + key, (res) => {
        var result = '';
        res.on("data", (d) => {
            result += d;
        })
        res.on("error", (e) => {
            console.log("连接失败。。。");
        })
        res.on("end", () => {
            console.log(result);
            let picurl = JSON.parse(result).data[0];
            if (!picurl) {
                console.log("没找到~好奇怪的xp呢");
                return;
            }
            let name = picurl.title + '.' + picurl.ext;
            picurl = picurl.urls.original.split('/');
            let pixivel = "https://proxy.pixivel.moe";  // 换源
            for (let i = 0, flag = false; i < picurl.length; i++) {
                if (flag) {
                    pixivel += '/' + picurl[i];
                } else {
                    if (picurl[i].search('pixi') > -1) flag = true;
                }
            }
            savePicture(pixivel, name);
        });
    })
}
// sese("&r18=1");
var readline = require('readline');
var rl = readline.createInterface({
    input: process.stdin,
    output: process.stdout
});
rl.question("你想要什么涩图呢？输入关键词获取！\ntips：直接回车->默认搜索；输入&“&r18=1”有惊喜！\n", function (answer) {
    sese(answer);
    rl.close();
});
