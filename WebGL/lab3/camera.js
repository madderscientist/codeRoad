/**
 * @file camera.js
 * @description 相机类
 * @description 用于处理相机的位姿和透视矩阵
 * @dependency mat.js
 * 刻意和其他的类解耦了，只依赖矩阵运算文件
 */
class GLcamera {
    /**
     * 相机位姿矩阵 效果：前方为z正方向，向相机靠近表现为z轴减小
     * @param {Float32Array} param0 相机焦点坐标 需要归一化
     * @param {Float32Array} at 相机朝向 需要归一化
     * @param {Float32Array} up 相机上方向 不需要归一化，不需要在平面内
     * @returns {Float32Array} 相机位姿矩阵
     */
    static lookAt([x, y, z], at, up = [0, 1, 0]) {
        let n = [at[0] - x, at[1] - y, at[2] - z];    // 法向量
        if (!(n[0] || n[1] || n[2])) n = [0, 0, 1];
        else n = normalVec3(n);
        let v = normalVec3(Cross(up, n));   // 另一个向量
        let u = normalVec3(Cross(n, v));    // 向上的向量
        return new Float32Array([
            v[0], v[1], v[2], -x * v[0] - y * v[1] - z * v[2],
            u[0], u[1], u[2], -x * u[0] - y * u[1] - z * u[2],
            n[0], n[1], n[2], -x * n[0] - y * n[1] - z * n[2],
            0, 0, 0, 1
        ]);
    }
    static perspective(fov, aspect, near, far) {
        let f = 1 / Math.tan(fov / 2);
        return new Float32Array([
            f / aspect, 0, 0, 0,
            0, f, 0, 0,
            0, 0, (far + near) / (near - far), 2 * far * near / (near - far),
            0, 0, -1, 0
        ]);
    }

    /**
     * 构造函数
     * @param {canvas} canvasSize 画布宽高 单位米
     * @param {Array} position 近平面中心点的位置 单位米
     * @param {Number} f 焦距，即焦点与平面的距离 单位米
     * @param {Number} sight 能看多远，从平面开始算 单位米
     */
    constructor(canvasSize, position = [0, 0, 0], f = 0.5, sight = 1000) {
        this.canvasSize = canvasSize;
        this.position = new Float32Array(position);
        this.f = f;
        this.sight = sight;
        this.up = new Float32Array([0, 1, 0]);  // 相机上方向 无所谓归一化
        // 法向有两种确定方法：向量和角度
        this._n = new Float32Array([0, 0, -1]);  // 近平面法向量 应始终归一化
        this._a = new Float32Array([Math.PI / 2, 0]); // 与x轴的夹角(逆时针为正，-π.π)，与xoz平面的夹角，π/2,-π/2
    }
    // this._n和this._a用两种形式表示同一个东西，因此需要同时更新
    get n() {
        return this._n;
    }
    set n(N) {
        this._n.set(normalVec3([N[0] || this._n[0], N[1] || this._n[1], N[2] || this._n[2]]));
        // 根据this.n更新this._a
        this._a[0] = Math.atan2(-this._n[2], this._n[0]);
        this._a[1] = Math.asin(this._n[1]);
    }
    get angle() {
        return this._a;
    }
    set angle(A) {
        if (A[0] != undefined) this._a[0] = A[0];
        if (A[1] != undefined) this._a[1] = A[1];
        let xoz = Math.cos(this._a[1]);
        this._n[0] = xoz * Math.cos(this._a[0]);
        this._n[1] = Math.sin(this._a[1]);
        this._n[2] = -xoz * Math.sin(this._a[0]); // 由于逆时针为正，所以有负号
    }
    /**
     * 调整相机位姿
     * @param {Float32Array} position 近平面的位置 需要归一化 可以传null
     * @param {Float32Array} at 相机朝向 需要归一化 必须传
     * @param {Float32Array} up 相机上方向 不需要归一化，不需要在平面内 可以传null
     * @returns {Float32Array} 相机位姿矩阵
     */
    lookAt(position, at, up) {
        if (!at) return;
        if (position) this.position.set(position);
        if (up) this.up.set(up);
        let n = [at[0] - this.position[0], at[1] - this.position[1], at[2] - this.position[2]];    // 法向量
        if (!(n[0] || n[1] || n[2])) n = [0, 0, 1];
        this.n = n;
    }
    /**
     * 得到当前相机的位姿矩阵
     * @returns {Float32Array} 相机位姿矩阵
     */
    postureMat() {
        let v = normalVec3(Cross(this.up, this._n));   // 另一个向量
        let u = normalVec3(Cross(this._n, v));    // 向上的向量
        // 焦点的位置
        // let [x,y,z] = this.position;    // 人眼习惯转轴在眼睛后面
        let [x, y, z] = [this.position[0] - this.n[0] * this.f, this.position[1] - this.n[1] * this.f, this.position[2] - this.n[2] * this.f];
        return new Float32Array([
            v[0], v[1], v[2], -x * v[0] - y * v[1] - z * v[2],
            u[0], u[1], u[2], -x * u[0] - y * u[1] - z * u[2],
            this._n[0], this._n[1], this._n[2], -x * this._n[0] - y * this._n[1] - z * this._n[2],
            0, 0, 0, 1
        ]);
    }
    /**
     * 得到当前相机的透视矩阵 处理了宽度比例
     * @returns {Float32Array} 透视矩阵
     */
    perspectiveMat() {
        return new Float32Array([
            2 * this.f / this.canvasSize[0], 0, 0, 0,
            0, 2 * this.f / this.canvasSize[1], 0, 0,
            0, 0, 1 + (this.f + this.f) / this.sight, -2 * this.f * (this.f + this.sight) / this.sight,
            0, 0, 1, 0
        ]);
    }
    Mat() {
        return MultiMat4(this.perspectiveMat(), this.postureMat());
    }
    /**
     * 根据鼠标移动的距离调整相机的法向
     * @param {MouseEvent} e mousemove event
     * @param {Number} sensitivity 灵敏度
     */
    mouseN(e, sensitivity = 0.002) {
        // 假设在半径为1米的球壳内旋转 省略了除以1
        let [thetaX, thetaY] = [e.movementX * sensitivity, -e.movementY * sensitivity];
        let a1 = this._a[0] + thetaX;
        let a2 = this._a[1] + thetaY;
        if (a1 <= -Math.PI) a1 += 2 * Math.PI;
        else if (a1 > Math.PI) a1 -= 2 * Math.PI;
        if (a2 > 1.57) a2 = 1.57;
        else if (a2 < -1.57) a2 = -1.57;
        this.angle = [a1, a2];
    }
    move(direction, distance) {
        this.position[0] += distance * direction[0];
        this.position[1] += distance * direction[1];
        this.position[2] += distance * direction[2];
    }
}