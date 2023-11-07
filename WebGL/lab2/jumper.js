class Jumper {
    constructor(currenty = 0, g = 20, dt = 15) {
        this.v = 0;
        this.dt = dt/1000;
        this.g = g;
        this.y = currenty;
        this.clk = null;
    }
    jump(speedy, recall = (y)=>{}, d = false) {
        this.v += speedy;
        if(this.clk) return;
        let lasty = this.y;
        // 用setInterval是下乘，用requestFrameAnimation是最好的
        this.clk = setInterval(()=>{
            let newy = this.v * this.dt + this.y;
            if(newy <= 0) {
                this.y = 0;
                this.v = 0;
                clearInterval(this.clk);
                this.clk = null;
            } else {
                this.y = newy;
                this.v -= this.g * this.dt;
            }
            if(d) recall(this.y - lasty);
            else recall(this.y);
            lasty = this.y;
        }, this.dt*1000);
    }
}