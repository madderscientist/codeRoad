var dxy = 126;                   //每格的边长
var player = [];
var turn = 0;
var ScoreBoard = null;           //分数板，需绑定

class node {
    constructor(position, dir, map) {   //dir取二进制,0b????
        this.map = map;             //绑定的棋盘
        this.color = 0;             //当前颜色
        this.maxium = 0;            //num的天花板
        //值表，上右左下，非null表示有值，根据值在表中位置，以及map的direction确定相邻的node序号
        this.value = new Array(4);
        for (let i = 0; i < 4; i++) {
            if (dir & 1 << (3 - i)) {
                this.value[i] = false;
                this.maxium++;
            } else {
                this.value[i] = null;
            }
        }
        this.num = 0;               //值的个数，随value表一起维护
        this.position = position;   //位置
    }
    draw() {
        var dx = [1, 2, 0, 1];
        var dy = [0, 1, 1, 2];
        let ctx = this.map.canvas.getContext('2d');
        let x = (this.position % this.map.col) * dxy;
        let y = (Math.floor(this.position / this.map.col)) * dxy;
        let da = dxy / 3;
        //画中心点
        ctx.beginPath();
        ctx.fillStyle = 'black';
        ctx.rect(x + da, y + da, da, da);
        ctx.fill();
        ctx.stroke();
        //画落脚点
        for (let i = 0; i < 4; i++) {
            if (this.value[i] != null) {
                ctx.beginPath();
                ctx.rect(x + dx[i] * da, y + dy[i] * da, da, da);
                ctx.fillStyle = colorlist[(this.value[i]) ? this.color : 0];
                ctx.fill();
                ctx.stroke();
            }
        }
    }
    receive(col, dir) {  //传来什么颜色（颜色名称），哪个方向（上0，右1，左2，下3）
        if (this.color != 0)
            this.map.score[colorlist[this.color]] -= this.num;
        this.color = col;
        this.map.score[colorlist[col]] += this.num;
        let n = ++this.num;
        //解决本位已有值的情况
        if (this.value[dir]) {
            for (let i = 0; i < 4; i++) {
                if (this.value[i] == false) {
                    this.value[i] = true;
                    break;
                }
            }
        } else this.value[dir] = true;
        this.draw();
        setTimeout(() => {
            if (n == this.num && this.num >= this.maxium) {    //表明这是接收到的最后一个且要输出
                this.num -= this.maxium;
                for (let i = 0; i < 4; i++) {
                    if (this.value[i]) {
                        this.map[this.position + this.map.direction[i]].receive(this.color, 3 - i);
                        if (i >= this.num) this.value[i] = false;
                    }
                }

                this.color = 0;
                this.draw();
            }
        }, 400);
        //以下是输赢判断
        if (this.map.step < 0) return;
        ScoreBoard.innerHTML = JSON.stringify(this.map.score);
        if (this.map.step > 2) {
            let s = this.map.score;
            let over = 0;
            for (let i in s) {
                if (s[i] == 0) {
                    let who = player.indexOf(colorlist.indexOf(i));
                    player[who] = 0;        //淘汰的玩家置零
                    delete s[i];
                } else over++;
            }
            if (over == 1 && player.length != 1) {
                alert("游戏结束！")
            }
        }
    }
}

class board {                   //棋盘，以字典形式存储节点
    constructor(canvas, ini) {
        this.step = 0;          //步数
        this.canvas = canvas;   //绑定的画布
        this.col = ini.col;     //棋盘列数
        this.row = ini.row;     //棋盘行数
        this.direction = [-ini.col, 1, -1, ini.col];   //每个节点的方向标架

        this.score = {};        //分数，颜色序号为索引的字典
        for (let i = 0; i < player.length; i++) {
            this.score[colorlist[player[i]]] = 0;
        }

        let x = ini.nodes;      //[position,direction]
        for (let i = 0; i < x.length; i++) {
            this[x[i][0]] = new node(x[i][0], x[i][1], this);
        }
        this.iflegel();
    }
    iflegel() {                 //检查是否合法，自动修复，原则是堵
        for (let i = 0; i < this.col * this.row; i++) {
            let thenode = this[i];
            if (thenode) {  //如果节点存在
                //联通关系，双向检测
                for (let j = 0; j < 4; j++) {
                    if (thenode.value[j] != null) {
                        //左右边界判断
                        let ano = i + this.direction[j];
                        if (j == 1 || j == 2) {
                            if (Math.floor(ano / this.col) != Math.floor(i / this.col)) {   //不在同一行
                                thenode.value[j] = null;
                                thenode.maxium--;
                                continue;
                            }
                        }
                        //指向判断
                        ano = this[ano];
                        if (ano == undefined) {                 //如果指向为空，说明缺节点
                            thenode.value[j] = null;
                            thenode.maxium--;
                        } else if (ano.value[3 - j] == null) {  //单项指向，堵上
                            thenode.value[j] = null;
                            thenode.maxium--;
                        }
                    }
                }
                if (!thenode.maxium) delete this[i];  //四个方向都不联通，删掉
            }
        }
    }

    inidraw() {
        let i = this.canvas.getContext('2d');
        i.beginPath();
        i.fillStyle = 'white';
        i.fillRect(0, 0, this.canvas.width, this.canvas.height);
        //清空完毕。画
        this.canvas.width = this.col * dxy;
        this.canvas.height = this.row * dxy;
        for (i = 0; i < this.col * this.row; i++) {
            let thenode = this[i];
            if (thenode) {
                thenode.draw();
            }
        }
        if (this.step >= 0) ScoreBoard.innerHTML = JSON.stringify(this.score);
    }
    putat(relativex, relativey) {
        //放在了哪个节点的哪个位置，合不合法
        //合法：有节点->位置为空->颜色相同
        let x = Math.floor(relativex / dxy);
        let y = Math.floor(relativey / dxy);
        let thenode = this[x + y * this.col];
        if (thenode) {      //有节点
            let da = dxy / 3;
            let duiying = { 1: 0, 3: 2, 5: 1, 7: 3 };
            x = Math.floor((relativex - x * dxy) / da);
            y = Math.floor((relativey - y * dxy) / da);
            let index = duiying[x + y * 3];
            if (index != undefined) {    //位置存在
                if (this.step < 0) {       //编辑模式
                    thenode.value[index] = null;
                    thenode.maxium--;
                    this.inidraw();
                    return;
                }
                if (thenode.value[index] == false && (thenode.color == player[turn] || thenode.color == 0)) {    //位置为空且颜色相同(或无颜色)
                    this.score[colorlist[player[turn]]]++;
                    thenode.receive(player[turn], index);
                    do { turn = ++turn % player.length; } while (player[turn] == 0);    //淘汰了的玩家就跳过
                    this.step++;
                    return;
                }
            }
        }
        console.log("这里不能放！");
    }
}

var colorlist = ['grey', 'red', 'green', 'yellow', 'blue', 'purple'];

function rectangleBoard(Col, Row) { //生成矩形地图
    var m = new Array(Col * Row);
    for (let i = 0; i < m.length; i++) {
        m[i] = [i, 15];             //反正会自动修复，就不做边界处理了
    }
    return { col: Col, row: Row, nodes: m };
}
function win() {
    let s = JSON.parse(ScoreBoard.innerHTML);

}