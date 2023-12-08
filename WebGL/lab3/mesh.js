class GLmaterial {
    constructor(specular, diffusion, ambient, shininess, color) {
        this.reflect = new Float32Array([specular, diffusion, ambient < 0 ? 0.25 : ambient, shininess]);
        this.color = new Float32Array([color[0], color[1], color[2], color[3]]);
    }
}

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
     * @param {Array} color 一维颜色数组 可不传
     * @param {*} colorid 用于颜色拾取的特异性颜色
     */
    constructor(gl, vertex, index, normal = null, color = null, colorid = null) {
        // 创建位置缓冲区
        this.vertexBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, this.vertexBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertex), gl.STATIC_DRAW);
        // 创建法向缓冲区
        // todo: 自动计算法向
        this.normalBuffer = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, this.normalBuffer);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(normal), gl.STATIC_DRAW);
        // 创建颜色缓冲区(如果有颜色的话)
        if (color) this.addColor(gl, color);
        else this.colorBuffer = null;
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
    static cube(gl) {
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
        return new GLmesh(gl, v, i, n);
    }
    static cubetest(gl) {     // 彩色渐变立方体
        const c = GLmesh.cube(gl);
        c.addColor(gl, new Float32Array([
            1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 1,
            0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0
        ]));
        return c;
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