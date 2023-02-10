var canvas = document.getElementById("Coordinate");
var ctx = canvas.getContext('2d');
ctx.lineWidth = 1;
// 坐标系范围 起点是0
const end = 10;
// 画图步长
const stepNum = 100;
// 比例
var a = canvas.offsetWidth;
// 神经网络
var ann = new ANN([1,10,25,60,25,10,1]);


// 1,30,1  0.0038  Math.random() + 0.1

var tf = "4*(Math.sin(x)+Math.cos(4*x+1))+5"
function targetFun(x){
    return eval(tf);
}
function train1000(){
    for (let i = 0; i < 1000; i++) {
        let x = Math.random() * 10
        ann.train(x,targetFun(x));
    }
    drawFunction((x) => {
        ann.z[0][0] = x;
        return ann.FP()[0]
    });
}
window.onload = () => {
    for(let i=0;i<10;i++) train1000();
    drawFunction((x) => {
        ann.z[0][0] = x;
        return ann.FP()[0]
    });
}

// 画函数
function drawFunction(fun) {
    // 清空画布
    ctx.fillStyle = "yellow";
    ctx.beginPath();
    ctx.fillRect(0, 0, a, a);
    ctx.closePath();
    // 画函数
    ctx.beginPath();
    ctx.moveTo(0, fun(0) * a / end);
    for (let i = 1; i <= stepNum; i++) {
        let I = end * i / stepNum;
        ctx.lineTo(i * a / stepNum, fun(I) * a / end);
    }
    ctx.stroke();
    ctx.closePath();
}


var clickX = 8;
var clickY = 8;
canvas.onclick = (e) => {
    // 训练
    clickX = e.offsetX * end / a;
    clickY = e.offsetY * end / a
    ann.train(clickX, clickY);
    // 画图
    drawFunction((x) => {
        ann.z[0][0] = x;
        return ann.FP()[0]
    });
}
// 自动点击 需要先点一次
document.addEventListener("keydown", (e) => {
    ann.train(clickX, clickY);
    drawFunction((x) => {
        ann.z[0][0] = x;
        return ann.FP()[0]
    });
});



// ANN
// 输入[2,4,6,5,3]表示：输入2个值，有三个层，第一层4个节点，第二层6个节点，第三层5个节点，输出3个值
function ANN(n) {
    // 训练 输入为向量
    this.train = (x, y) => {
        if (!isNaN(y)) y = [y];
        if (!isNaN(x)) x = [x];
        this.Y = y;
        for (let i = 0; i < this.attr[0]; i++) {
            this.z[0][i] = x[i];
        }
        let result = this.FP();
        this.BP();
        return result;
    }
    this.ann=(x)=>{
        if (!isNaN(x)) x = [x];
        for (let i = 0; i < this.attr[0]; i++) {
            this.z[0][i] = x[i];
        }
        return this.FP();
    }

    this.study = 0.0024;   // 学习率 5层用0.0024

    this.attr = n;

    this.Y = new Array(n[n.length - 1]);    // 实际值

    this.w = new Array(n.length);           // 权值 要多一维 前面一层节点最后有个1表示bias
    this.z = new Array(n.length);           // 节点存值

    // 创建权值数组 第一维是第几层 第二维是第几个 第三位是前面一层第几个节点的权值
    for (let x = 1; x < n.length; x++) {    // 每层（第一列空着 是输入 没权值
        this.w[x] = new Array(n[x]);
        for (let y = 0; y < n[x]; y++) {    // 每层的每个节点对前一层所有节点的权值
            this.w[x][y] = new Array(n[x - 1] + 1);
            for (let p = 0; p <= n[x - 1]; p++) this.w[x][y][p] = 0.5 - Math.random();    // 初始权值不能一样
        }
        this.z[x - 1] = new Array(n[x - 1] + 1).fill(1);
    }
    this.z[n.length - 1] = new Array(n[n.length - 1]).fill(1);  // 最后一项没有隐藏的1 是结果

    // 激活函数及其导数 激活函数使用leaky relu
    this.actfun = (x) => {
        return x < 0 ? 0.05 * x : x;
    };
    this.actder = (x) => {
        return x < 0 ? 0.05 : 1;
    };
    // 损失函数 及 其对求出的值的导数
    // myY: 求出的值  actY: 实际的值 都是向量(数组)
    this.lossfun = (myY, actY) => {
        let loss = 0;
        for (let i = 0; i < actY.length; i++) {
            loss += Math.pow(myY[i] - actY[i], 2);
        }
        return loss / 2;
    };
    this.lossder = (myY, actY) => {
        return myY - actY;
    };
    this.FP = () => {   // 正向传播
        for (let i = 1; i < this.attr.length; i++) {           // 每层
            for (let j = 0; j < this.attr[i]; j++) {    // 每层的每个（每列z最后加的1不参与被赋值
                this.z[i][j] = this.actfun(multiVec(this.z[i - 1], this.w[i][j]));
            }
        }
        return this.z[this.z.length - 1];                       // 返回结果向量
    };


    this.BP = () => {   // 反向传播
        let tempz = this.z.length;  //层数
        // dL/dz
        let dz = new Array(tempz);
        // 最后一列特殊处理 因为和L的偏导有关 规模和this.z差了每列最后一项
        let templ = this.attr[tempz - 1];
        dz[tempz - 1] = new Array(templ);
        for (let j = 0; j < templ; j++) {
            let zz = this.z[tempz - 1][j];
            dz[tempz - 1][j] = this.lossder(zz, this.Y[j]) * this.actder(zz);
        }
        // 其他列: L对上一层z全部偏导 × 激活函数导数 第一列不要，因为是输入
        for (let i = dz.length - 2; i > 0; i--) {        // 每层
            dz[i] = new Array(this.attr[i]).fill(0);
            for (let j = 0; j < dz[i].length; j++) {    // 每层的每个
                for (let k = 0; k < dz[i + 1].length; k++) {  // 对上一层的每个求偏导
                    dz[i][j] += dz[i + 1][k] * this.w[i + 1][k][j];
                }
                dz[i][j] *= this.actder(this.z[i][j]);
            }
        }
        // 更新权值
        for (let i = 1; i < this.w.length; i++) {
            for (let j = 0; j < this.w[i].length; j++) {
                for (let k = 0; k < this.w[i][j].length; k++) {
                    this.w[i][j][k] -= this.study * dz[i][j] * this.z[i - 1][k];
                }
            }
        }
    };

    // 向量点乘
    function multiVec(a, b) {
        let result = 0;
        for (let i = 0; i < a.length; i++) {
            result += a[i] * b[i];
        }
        return result;
    }
}
