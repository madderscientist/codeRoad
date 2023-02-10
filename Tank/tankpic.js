module.exports = {
    tank,
    savepicture
}
const fs = require("fs");
const http = require("http");
const Jimp = require('jimp');
function tank(f1, f2, tof3,huidiao) {
    var p1, p2,w,h;
    Jimp.read(f1).then(p=>{
        p.color([
            { apply: 'brighten', params: [45] },
        ]);
        p.greyscale();
        p1=p;
        w=p.bitmap.width;
        h=p.bitmap.height;
    }).then(()=>{
        Jimp.read(f2).then(pp=>{
            pp.color([
                { apply: 'darken', params: [40] },
            ]);
            pp.resize(w,h).greyscale();
            p2=pp;
        }).then(()=>{
            new Jimp(w,h,0x00000000,(err,img)=>{
                if(err) throw err;
                for(let i=0;i<4*w*h;i++){
                    let col1=p1.bitmap.data[i];
                    let col2=p2.bitmap.data[i];
                    let aa=255-col1+col2;
                    let grey=parseInt(255*col2/aa);
                    img.bitmap.data[i++]=grey;
                    img.bitmap.data[i++]=grey;
                    img.bitmap.data[i++]=grey;
                    img.bitmap.data[i]=aa;
                }
                img.write(tof3);
                huidiao();
            })
        })
    })
}
function savepicture(url, name='test1.png') {
    http.get(url, (response) => {
        var data = "";
        response.setEncoding("binary");
        response.on('data', function (chunk) {
            data += chunk;
        });
        response.on("end", function () {
            fs.writeFile(name, data, 'binary', (err) => {
                if (err) console.log('写入文件错误');
                else console.log('写入文件成功');
            })
        });
    }).on("error", function () {
        console.log('读取错误:',url);
    });
}
