class LineNode {
    /**
     * 创造一个连接点，实质上代表一根指向linkto的连杆
     * @param {LineNode} linkto 连接的对象，作为从属 
     * @param {number} distance 连接的距离
     * @param {number} angle 初始的角度 canvas坐标系，0为右侧，pi/2为下；以linkto为原点，指向this
     * @param {boolean} xymode 是否是xy坐标系，true传参是[x, y]
     */
    constructor(linkto, distance, angle = 0, xymode = false) {
        this.distance = -1;
        if (xymode) {   // distance是x坐标，angle是y坐标
            this.x = distance;
            this.y = angle;
            if (linkto) this.distance = Math.sqrt((distance - linkto.x) ** 2 + (angle - linkto.y) ** 2);
        } else if (linkto) {
            this.x = linkto.x + Math.cos(angle) * distance;
            this.y = linkto.y + Math.sin(angle) * distance;
            this.distance = distance;
        } else {
            throw new Error('linkto is null, please set x and y');
        }
        this.father = linkto;   // father是主导 为null表示这是根
        this.son = null;
        if (linkto) linkto.son = this;  // 连接的对象是主导
    }
    angle2(node = this.father) {    // 默认为指向father的角度
        if (node == null) return [1, 0];  // 根节点的角度是0
        const offsetX = node.x - this.x;
        const offsetY = node.y - this.y;
        const distance_now = Math.sqrt(offsetX * offsetX + offsetY * offsetY);
        return [offsetX / distance_now, offsetY / distance_now];
    }
    /**
     * 返回[cos, sin]，表示以father为原点的角度
     * @returns {number[]}
     */
    get angle() {
        if (this.father == null) return [1, 0];  // 根节点的角度是0
        return this.father.angle2(this);
    }
    // 正向动力学 theta为以father为原点的角度
    set angle(theta) {
        if (this.father == null) return;  // 根节点的角度是0
        this.x = this.father.x + Math.cos(theta) * this.distance;
        this.y = this.father.y + Math.sin(theta) * this.distance;
        if (this.son) this.son.ik_forward();    // 更改了位置，子节点也要跟着改变
    }
    // 反向动力学 末端位置会变
    ik_forward(chain = true) {
        if (this.father) {
            // 此时father已经改变了位置
            const [cos, sin] = this.angle;
            this.x = this.father.x + cos * this.distance;
            this.y = this.father.y + sin * this.distance;
        }
        if (chain && this.son) this.son.ik_forward();
    }
    ik_backward(chain = true) {
        if (this.son) {
            // 此时son已经改变了位置
            const [cos, sin] = this.angle2(this.son);
            this.x = this.son.x - cos * this.son.distance;
            this.y = this.son.y - sin * this.son.distance;
        }
        if (chain && this.father) this.father.ik_backward();
    }
}

class Body extends LineNode {
    constructor() {
        super(...arguments);
        this.friends = [];  // 位置固定的连杆
        this.feet = [];
        this.forward_step = this.distance * 2.2;
        this.back_step = this.distance / 2.5;
        this.footwidth = this.distance * 2.2;
    }
    addFriend(node) {
        // 保持两者相对距离不变 假设此时连杆方向为alpha，node和this的距离为d，和连杆方向成theta角
        // 则x偏移为d*(cos(alpha + theta)) = [d*cos(theta)]*cos(alpha) - [d*sin(theta)]*sin(alpha)
        // 且y偏移为d*(sin(alpha + theta)) = [d*cos(theta)]*sin(alpha) + [d*sin(theta)]*cos(alpha)
        // 初始化的时候可以根据偏移量求出dcos(theta)和dsin(theta)
        // 求解矩阵得到 dsin(theta) = cos(alpha) * dy - sin(alpha) * dx
        // dcos(theta) = cos(alpha) * dx + sin(alpha) * dy
        const offsetX = node.x - this.x;
        const offsetY = node.y - this.y;
        const [c, s] = this.angle2();
        this.friends.push([
            node,
            c * offsetX + s * offsetY,  // dcos(theta)
            c * offsetY - s * offsetX   // dsin(theta)
        ]);
    }
    addFoot(foot) {
        // 绑定腿部末端
        const offsetX = foot.x - this.x;
        const offsetY = foot.y - this.y;
        const [c, s] = this.angle2();
        const leftorright = offsetX * s - offsetY * c > 0 ? 1 : -1;   // 叉乘判断左右
        foot.lr = leftorright;  // 记录左右
        this.feet.push(foot);
    }
    // 更新绑定的腿
    updateLeg() {
        // 更新所有朋友的位置 即腿的根节点
        const [c, s] = this.angle2();
        for (const [node, dcos, dsin] of this.friends) {
            node.x = this.x + c * dcos - s * dsin;
            node.y = this.y + s * dcos + c * dsin;
        }
        // 更新所有脚的位置 如果在angle方向上两者距离小于一定距离则脚往前一步
        for (const foot of this.feet) {
            const offsetX = foot.x - this.x;
            const offsetY = foot.y - this.y;
            const r_dis = offsetX * c + offsetY * s;   // 投影长度 余弦定理
            const n_dis = Math.abs(offsetX * s - offsetY * c);   // 垂直距离 正弦定理
            if (r_dis < this.back_step || n_dis < this.distance / 1.5) {
                const ratio = Math.random() * 0.8 + 0.2;
                let foot_width = this.footwidth * ratio + n_dis * (1 - ratio);    // 加一个低通和随机数
                let forward_step = this.forward_step * (Math.random() * 0.3 + 0.9);
                foot.x = this.x + forward_step * c + foot.lr * foot_width * s;
                foot.y = this.y + forward_step * s - foot.lr * foot_width * c;
            }
            let root = foot;
            while (root.father) root = root.father;  // 找到根节点
            const [originalX, originalY] = [root.x, root.y];
            foot.ik_backward();  // 反向动力学 会改动root的坐标
            // 恢复root的坐标
            root.x = originalX;
            root.y = originalY;
            // 重新计算所有脚的位置
            root.ik_forward();
        }
    }
    update(ctx) {
        this.ik_forward(false);
        this.updateLeg();
        // 绘制
        for (const foot of this.feet) {
            ctx.strokeStyle = 'red';
            ctx.lineWidth = 2;
            ctx.beginPath();
            let current = foot;
            ctx.moveTo(current.x, current.y);
            current = current.father;
            while (current) {
                ctx.lineTo(current.x, current.y);
                current = current.father;
            }
            ctx.stroke();
        }
        if (this.father) {
            ctx.strokeStyle = 'blue';
            ctx.lineWidth = 2;
            const halfSide = this.distance / 2;
            const [c, s] = this.angle2();
            const dx = halfSide * s;
            const dy = halfSide * c;

            ctx.beginPath();
            ctx.moveTo(this.x - dx, this.y + dy);  // Top-left corner
            ctx.lineTo(this.x + dx, this.y - dy);  // Top-right corner
            ctx.lineTo(this.father.x + dx, this.father.y - dy);  // Bottom-right corner
            ctx.lineTo(this.father.x - dx, this.father.y + dy);  // Bottom-left corner
            ctx.closePath();
            ctx.stroke();
        }
        if (this.son) this.son.update(ctx);  // 更新子节点
    }
}