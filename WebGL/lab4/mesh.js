class GLmesh {  // 主要实现了特异性颜色的提取 和lab2相比，去掉了mesh中的颜色，颜色由GLmaterial实现
    static idColors = [0];
    static uniqueColor(addin = false) {     // 是否添加入列表
        let tryID = GLmesh.idColors.length;
        let count = 0;
        while (tryID in GLmesh.idColors && count < (1 << 24)) {
            tryID = tryID == (1 << 24) ? 1 : tryID + 1;
            count++;
        }
        if (count == (1 << 24)) return 0;
        else {
            if (addin) GLmesh.idColors.push(tryID);
            return tryID;
        }
    }
    static IDtoCOLOR(idColor) {
        const r = (idColor >> 16) & 0xff;
        const g = (idColor >> 8) & 0xff;
        const b = idColor & 0xff;
        return new Float32Array([r / 255, g / 255, b / 255]);
    }
    static COLORtoID(color) {
        const r = Math.round(color[0] * 255);
        const g = Math.round(color[1] * 255);
        const b = Math.round(color[2] * 255);
        return (r << 16) + (g << 8) + b;
    }
    idColorArray() {
        return GLmesh.IDtoCOLOR(this.colorid);
    }

    /**
     * 生成一个绑定到gl的mesh
     * @param {WebGL2RenderingContext} gl
     * @param {Array} vertex 一维顶点位置数组 已经归一化
     * @param {Array} index 一维索引数组
     * @param {Array} normal 一维法向数组 不传则自动计算 要求index顺序符合右手法则 没必要归一化
     * @param {Array} map   一维UV贴图坐标数组 可不传
     * @param {Array} color 一维颜色数组 可不传
     * @param {*} colorid 用于颜色拾取的特异性颜色
     */
    constructor(gl, vertex, index, normal = null, map = null, color = null, colorid = null) {
        // 创建位置缓冲区
        this.vertexBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, this.vertexBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertex), gl.STATIC_DRAW);
        // 创建法向缓冲区 如果不传则自动按右手法则计算
        if (!normal || !normal.length) normal = facesNormal(vertex, index);  // 需要mat.js
        this.normalBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, this.normalBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(normal), gl.STATIC_DRAW);
        // 创建颜色缓冲区(如果有颜色的话)
        if (color && color.length) this.addColor(gl, color);
        else this.colorBuffer = null;
        // 创建UV缓冲区(如果有贴图的话)
        if (map && map.length) {
            this.map = gl.createBuffer();
            gl.bindBuffer(gl.ARRAY_BUFFER, this.map);
            gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(map), gl.STATIC_DRAW);
        } else this.map = null;
        // 创建索引缓冲区
        this.indexBuffer = gl.createBuffer();
        this.indexBuffer.length = index.length;
        let type = index.length > 65535 ? [Uint32Array, gl.UNSIGNED_INT] : (
            index.length > 255 ? [Uint16Array, gl.UNSIGNED_SHORT] : [Uint8Array, gl.UNSIGNED_BYTE]
        );
        this.indexBuffer.type = type[1];
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this.indexBuffer);
        gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new (type[0])(index), gl.STATIC_DRAW);
        // 在识别点击的时候的特异性颜色 颜色用8位表示法
        this.colorid = colorid || GLmesh.uniqueColor(true);
    }
    static cube(gl, inverse = false) {  // 是否法线翻转
        const v = [
            -0.5, -0.5, 0.5,
            0.5, -0.5, 0.5,
            0.5, 0.5, 0.5,
            -0.5, 0.5, 0.5,
            -0.5, -0.5, -0.5,
            0.5, -0.5, -0.5,
            0.5, 0.5, -0.5,
            -0.5, 0.5, -0.5
        ];
        const i = [
            0, 1, 2, 0, 2, 3,   // 前
            5, 7, 6, 5, 4, 7,   // 后
            0, 7, 4, 0, 3, 7,   // 左
            2, 1, 5, 2, 5, 6,   // 右
            2, 7, 3, 2, 6, 7,   // 上
            0, 4, 5, 0, 5, 1,   // 下
        ];
        const n = [ // 懒得归一化了
            -1, -1, 1,
            1, -1, 1,
            1, 1, 1,
            -1, 1, 1,
            -1, -1, -1,
            1, -1, -1,
            1, 1, -1,
            -1, 1, -1
        ];
        const UV = [
            0, 0,
            1, 0,
            1, 1,
            0, 1,
            0, 1,
            1, 1,
            1, 0,
            0, 0
        ];
        if (inverse) {
            for (let i = 0; i < n.length; i++) n[i] = -n[i];
        }
        return new GLmesh(gl, v, i, n, UV);
    }
    static cubetest(gl) {     // 彩色渐变立方体
        const c = GLmesh.cube(gl);
        c.addColor(gl, new Float32Array([
            1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1,
            0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0
        ]));
        return c;
    }
    static solidCube(gl) {
        const v = [
            // 前
            -0.5, -0.5, 0.5,
            0.5, -0.5, 0.5,
            0.5, 0.5, 0.5,
            -0.5, 0.5, 0.5,
            // 后
            -0.5, -0.5, -0.5,
            0.5, -0.5, -0.5,
            0.5, 0.5, -0.5,
            -0.5, 0.5, -0.5,
            // 左
            -0.5, -0.5, -0.5,
            -0.5, -0.5, 0.5,
            -0.5, 0.5, 0.5,
            -0.5, 0.5, -0.5,
            // 右
            0.5, -0.5, -0.5,
            0.5, -0.5, 0.5,
            0.5, 0.5, 0.5,
            0.5, 0.5, -0.5,
            // 上
            -0.5, 0.5, 0.5,
            0.5, 0.5, 0.5,
            0.5, 0.5, -0.5,
            -0.5, 0.5, -0.5,
            // 下
            -0.5, -0.5, 0.5,
            0.5, -0.5, 0.5,
            0.5, -0.5, -0.5,
            -0.5, -0.5, -0.5
        ];
        const i = [
            0, 1, 2, 0, 2, 3,   // 前
            5, 7, 6, 5, 4, 7,   // 后
            8, 9, 10, 8, 10, 11,   // 左
            13, 15, 14, 13, 12, 15,   // 右
            16, 17, 18, 16, 18, 19,   // 上
            21, 23, 22, 21, 20, 23   // 下
        ];
        const n = [
            0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1,
            0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1,
            -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0,
            1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
            0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0,
            0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0
        ];
        const UV = [
            0, 0, 1, 0, 1, 1, 0, 1,
            0, 0, 1, 0, 1, 1, 0, 1,
            0, 0, 1, 0, 1, 1, 0, 1,
            0, 0, 1, 0, 1, 1, 0, 1,
            0, 0, 1, 0, 1, 1, 0, 1,
            0, 0, 1, 0, 1, 1, 0, 1
        ];
        return new GLmesh(gl, v, i, n, UV);
    }
    /**
     * 球体的网格
     * @param {WebGL2RenderingContext} gl 
     * @param {Number} subdivision 180度内的细分次数 最后赤道上为2*subdivision边形
     */
    static sphere(gl, subdivision = 18) {
        let da = Math.PI / subdivision;
        let subdivision2 = subdivision << 1;
        let pointNum = (subdivision - 1) * subdivision2 + 2;
        const vertex = new Float32Array(pointNum * 3);
        vertex.set([0, 1, 0, 0, -1, 0], vertex.length - 6);    // 顶点和底点
        for (let i = 0, thetai = 0, n = 0; i < subdivision2; i++, thetai += da) {   // 经度
            for (let j = 1, thetaj = da; j < subdivision; j++, thetaj += da) {    // 纬度
                let sin = Math.sin(thetaj);
                vertex[n++] = sin * Math.cos(thetai);
                vertex[n++] = Math.cos(thetaj);
                vertex[n++] = sin * Math.sin(thetai);
            }
        }
        da = subdivision - 1;
        const index = new Float32Array(3 * da * subdivision2 << 1);
        let top = pointNum - 2, buttom = pointNum - 1;
        for (let i = 0, n = 0; i < subdivision2; i++) {
            let nexti = (i + 1) % subdivision2;
            let lastPoint = top;
            let lastPoint2 = buttom;
            for (let j = 0, k = da - 1; j < da; j++, k--) { // 特意把两个循环合并了，加快速度
                index[n] = j + i * da;
                index[n + 1] = lastPoint;
                index[n + 2] = j + nexti * da;
                lastPoint = index[n];
                index[n + 3] = lastPoint2;
                index[n + 4] = k + i * da;
                index[n + 5] = k + nexti * da;
                lastPoint2 = index[n + 5];
                n += 6;
            }
        }
        return new GLmesh(gl, vertex, index);
    }
    static STL(gl, data) {
        return new GLmesh(gl, ...GLmesh.fromSTL(data));
    }
    static fromSTL(data) {
        let n = data[83] << 24 | data[82] << 16 | data[81] << 8 | data[80]; // 面数
        let num = 0;    // 点集内点的数目
        let index = new Uint32Array(n * 3 * 3);
        let indexn = 0; // 当前是第几个点
        let pSet = {};

        for (let i = 84; n > 0; n--) {
            // 读取法向 假设stl文件里面全是归一化的了
            let dx = (new DataView(data.buffer, i, 4)).getFloat32(0, true),
                dy = (new DataView(data.buffer, i + 4, 4)).getFloat32(0, true),
                dz = (new DataView(data.buffer, i + 8, 4)).getFloat32(0, true);
            i += 12;
            // 读取三个顶点
            for (let j = 0; j < 3; j++) {
                let phash = (new DataView(data.buffer, i, 12)).getBigInt64(0);
                if (!pSet[phash]) { // 如果不在点集内，则添加
                    pSet[phash] = {
                        x: (new DataView(data.buffer, i, 4)).getFloat32(0, true),
                        y: (new DataView(data.buffer, i + 4, 4)).getFloat32(0, true),
                        z: (new DataView(data.buffer, i + 8, 4)).getFloat32(0, true),
                        dx: 0,
                        dy: 0,
                        dz: 0,
                        index: num++,
                    };
                }
                const p = pSet[phash];
                p.dx += dx;
                p.dy += dy;
                p.dz += dz;
                index[indexn++] = p.index;
                i += 12;
            }
            i += 2;
        }

        let vertex = new Float32Array(num * 3);
        let normal = new Float32Array(num * 3);
        for (let pHash in pSet) {
            const p = pSet[pHash];
            let loc = p.index * 3;
            let l = Math.sqrt(p.dx * p.dx + p.dy * p.dy + p.dz * p.dz);
            vertex[loc] = p.x;
            vertex[loc + 1] = p.y;
            vertex[loc + 2] = p.z;
            normal[loc] = p.dx / l;
            normal[loc + 1] = p.dy / l;
            normal[loc + 2] = p.dz / l;
        }
        return [vertex, index, normal];
    }
    addColor(gl, color) {
        this.colorBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, this.colorBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(color), gl.STATIC_DRAW);
    }
    removeColor(gl) {
        gl.deleteBuffer(this.colorBuffer);
        this.colorBuffer = null;
    }
}