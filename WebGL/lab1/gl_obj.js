// 矩阵乘法，行主，4*4
function MultiMat4(a, b) {
    let out = new Float32Array(16);
    for (let i = 0; i < 4; i++) {
        for (let j = 0; j < 4; j++) {
            out[i * 4 + j] = 0;
            for (let k = 0; k < 4; k++) {
                out[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
            }
        }
    }
    return out;
}

class GLmesh {
    static idColors = [0];
    static cubetest() {     // 彩色渐变立方体
        const v = [
            // 前面
            [-0.5, -0.5, 0.5, 1, 0, 0],
            [0.5, -0.5, 0.5, 1, 1, 0],
            [0.5, 0.5, 0.5, 0, 1, 0],
            [-0.5, 0.5, 0.5, 0, 1, 1],
            // 后面
            [-0.5, -0.5, -0.5, 0, 0, 1],
            [0.5, -0.5, -0.5, 1, 0, 1],
            [0.5, 0.5, -0.5, 1, 0, 0],
            [-0.5, 0.5, -0.5, 1, 1, 0]
        ];
        const i = new Uint8Array([
            0, 1, 2, 0, 2, 3,   // 前
            4, 5, 6, 4, 6, 7,   // 后
            0, 4, 7, 0, 7, 3,   // 左
            1, 5, 6, 1, 6, 2,   // 右
            3, 2, 6, 3, 6, 7,   // 上
            0, 1, 5, 0, 5, 4,   // 下
        ]);
        return new GLmesh(v, i);
    }
    static cube(color = [1, 0, 0], vertix = null, indice = null) {
        const v = vertix || [     // 顶点坐标，颜色
            // 前面
            [-0.5, -0.5, 0.5, ...color],
            [0.5, -0.5, 0.5, ...color],
            [0.5, 0.5, 0.5, ...color],
            [-0.5, 0.5, 0.5, ...color],
            // 后面
            [-0.5, -0.5, -0.5, ...color],
            [0.5, -0.5, -0.5, ...color],
            [0.5, 0.5, -0.5, ...color],
            [-0.5, 0.5, -0.5, ...color]
        ];
        const i = indice || new Uint8Array([
            0, 1, 2, 0, 2, 3,   // 前
            4, 5, 6, 4, 6, 7,   // 后
            0, 4, 7, 0, 7, 3,   // 左
            1, 5, 6, 1, 6, 2,   // 右
            3, 2, 6, 3, 6, 7,   // 上
            0, 1, 5, 0, 5, 4,   // 下
        ]);
        return new GLmesh(v, i);
    }
    static triangle(color = [1, 0, 0]) {
        const v = [
            [-0.5, -0.5, 0, ...color],
            [0.5, -0.5, 0, ...color],
            [0, 0.5, 0, ...color]
        ];
        const i = new Uint8Array([0, 1, 2]);
        return new GLmesh(v, i);
    }
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

    constructor(vertix = [], triangle = [], colorid = null) {
        // 二维数组
        // 按照一个顶点：x,y,z,r,g,b的方式排布
        this.vertix = vertix;
        this.triangle = triangle;
        // 在识别点击的时候的特异性颜色 颜色用8位表示法
        this.colorid = colorid || GLmesh.uniqueColor(true);
    }
    meshArray() {
        return [new Float32Array(this.vertix.flat()), this.triangle];
    }
    vertixArray() {
        return new Float32Array(this.vertix.map(v => v.slice(0, 3)).flat());
    }
    colorArray() {
        return new Float32Array(this.vertix.map(v => v.slice(3, 6)).flat());
    }
    bind(v, c) {
        // 将v和c合并为this.vertix的格式
        let vertix = [];
        for (let i = 0; i < v.length; i += 3) {
            vertix.push([...v.slice(i, i + 3), ...c.slice(i, i + 3)]);
        }
        this.vertix = vertix;
    }
    idColorArray() {
        return GLmesh.IDtoCOLOR(this.colorid);
    }
}

class GLobj {
    // 此函数的返回相当于先平移后旋转，而旋转是绕着坐标轴进行的。所以如果有平移量，那旋转会影响位置
    // 全局坐标系应该左乘，局部坐标系应该右乘。平移是相对于全局的，所以应该左乘，旋转是相对于局部的，所以应该右乘
    // 所以不应该三种参数一起传，建议用下面分开的三个
    static transformation(
        x = 0, y = 0, z = 0,
        rotx = 0, roty = 0, rotz = 0,
        scalex = 1, scaley = 1, scalez = 1,
        transposed = false
    ) {
        const cosx = Math.cos(rotx), sinx = Math.sin(rotx);
        const cosy = Math.cos(roty), siny = Math.sin(roty);
        const cosz = Math.cos(rotz), sinz = Math.sin(rotz);
        if (transposed) return new Float32Array([
            scalex * cosy * cosz, scalex * cosy * sinz, -scalex * siny, 0,
            scaley * (sinx * siny * cosz - cosx * sinz), scaley * (sinx * siny * sinz + cosx * cosz), scaley * sinx * cosy, 0,
            scalez * (cosx * siny * cosz + sinx * sinz), scalez * (cosx * siny * sinz - sinx * cosz), scalez * cosx * cosy, 0,
            x, y, z, 1
        ]);
        else return new Float32Array([
            scalex * cosy * cosz, scaley * (sinx * siny * cosz - cosx * sinz), scalez * (cosx * siny * cosz + sinx * sinz), x,
            scalex * cosy * sinz, scaley * (sinx * siny * sinz + cosx * cosz), scalez * (cosx * siny * sinz - sinx * cosz), y,
            -scalex * siny, scaley * sinx * cosy, scalez * cosx * cosy, z,
            0, 0, 0, 1
        ]);
    }
    static rotation(rx = 0, ry = 0, rz = 0, transposed = false) {
        return GLobj.transformation(0, 0, 0, rx, ry, rz, 1, 1, 1, transposed);
    }
    static scale(sx = 1, sy = 1, sz = 1) {
        return new Float32Array([
            sx, 0, 0, 0,
            0, sy, 0, 0,
            0, 0, sz, 0,
            0, 0, 0, 1
        ]);
    }
    static translation(x = 0, y = 0, z = 0, transposed = false) {
        if (transposed) return new Float32Array([
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            x, y, z, 1
        ]);
        else return new Float32Array([
            1, 0, 0, x,
            0, 1, 0, y,
            0, 0, 1, z,
            0, 0, 0, 1
        ]);
    }
    static I = new Float32Array([
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    ]);

    constructor(gl, displacement, rotation, mesh = null, scale = GLobj.I) {
        this.gl = gl;
        this.displacement = displacement;       // 初始位置
        this.rotation = rotation;     // 在初始位置上的变换
        this.scale = scale;
        this.children = {};
        this.mesh = mesh;
        this.transform = GLobj.I;
    }
    transformation(t) {
        // 右乘！因为this.transform是右乘的。意思是坐标变换
        this.transform = MultiMat4(this.transform,t);
    }
    draw(transform) {
        const g = this.gl;
        // 计算变换矩阵
        // 位移。是左乘
        let t = MultiMat4(transform, this.displacement);
        // 旋转。是右乘
        t = MultiMat4(t, this.rotation);
        // 额外的变化，是坐标变换，右乘
        t = MultiMat4(t, this.transform);

        // 以下写法是绕着原点旋转。不应该用
        // let t = MultiMat4(this.rotation, transform);
        // t = MultiMat4(t, this.displacement);

        if (this.mesh) {     // 如果是null则不画，表示为一个坐标转换
            g.uniform.translation = MultiMat4(t, this.scale);   // 缩放。需要先作用在原物体，所以右乘
            const [v, i] = this.mesh.meshArray();
            g.bufferData(g.ARRAY_BUFFER, v, g.DYNAMIC_DRAW);
            g.bufferData(g.ELEMENT_ARRAY_BUFFER, i, g.DYNAMIC_DRAW);
            g.drawElements(g.TRIANGLES, i.length, g.UNSIGNED_BYTE, 0);
        }
        // 画子物体
        for (let name in this.children) {
            this.children[name].draw(t);
        }
    }

    addChild(name, child) {
        this.children[name] = child;
        return this;
    }
}

class GLobjRoot {
    constructor() {
        // 子物体
        this.children = {};
        this.gl = null;
        this.drawProgram = null;
        this.frameProgram = null;
    }
    setCamera(displacement, rotation = null, scale = null) {
        if (!this.gl) throw new Error("GL not initialized");
        if (displacement) this.gl.uniform.camera_displace = displacement;
        if (rotation) this.gl.uniform.camera_rotation = rotation;
        if (scale) this.gl.uniform.camera_scale = scale;
    }
    iniGL(canvas, viewDistance = 1) {   // viewDistance指观察距离（平行投影的）
        // GL初始化
        const gl = MyGL.new(canvas);
        this.gl = gl;
        // 用专用的着色器
        gl.useProg(this.iniDraw(gl, viewDistance));
    }
    iniDraw(gl, viewDistance) {
        const vsSource = `
            attribute vec4 a_Position;
            uniform mat4 translation;
            uniform mat4 camera_rotation;   // 画面旋转
            uniform mat4 camera_displace;   // 画面偏移
            uniform mat4 camera_scale;      // 画面大小

            uniform mat4 canvas_scale;  // 画面修正 应对画布长宽不一
        
            attribute vec4 a_Color;
            varying vec4 v_Color;
        
            void main() {
                gl_Position = canvas_scale*camera_scale*camera_rotation*camera_displace* translation*a_Position;
                v_Color = a_Color;
            }
        `;
        const fsSource = `
            precision mediump float;
            varying vec4 v_Color;

            void main() {
                gl_FragColor = v_Color;
            }
        `;
        let drawProgram = gl.iniProgram(vsSource, fsSource)
        gl.useProg(drawProgram);
        gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);
        gl.enable(gl.DEPTH_TEST);
        gl.uniform.translation = GLobj.I;
        // ！！如果画布长宽不一样，需要压缩（不然无法保持比例，z轴的长度也是个迷）。解决办法是最后将画布坐标变换到正方形。由于对画面，所以最后作用，乘在最左边
        this.canvasLen = Math.min(gl.canvas.width, gl.canvas.height) * viewDistance;   // 乘在这里可以让能展示的范围更大，而不影响normal的返回画出来的结果
        this.gl.uniform.canvas_scale = GLobj.scale(this.canvasLen / gl.canvas.width, this.canvasLen / gl.canvas.height);

        // 位置信息和顶点信息
        gl.bindBuffer(gl.ARRAY_BUFFER, gl.createBuffer());
        gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, gl.createBuffer());
        // 一个顶点：x,y,z,r,g,b; 没有a因为深度和透明度冲突
        gl.vertexAttribPointer(
            gl.attribute.a_Position,
            3, gl.FLOAT, false,
            6 * Float32Array.BYTES_PER_ELEMENT,
            0
        );
        gl.enableVertexAttribArray(gl.attribute.a_Position);
        gl.vertexAttribPointer(
            gl.attribute.a_Color,
            3, gl.FLOAT, false,
            6 * Float32Array.BYTES_PER_ELEMENT,
            3 * Float32Array.BYTES_PER_ELEMENT
        );
        gl.enableVertexAttribArray(gl.attribute.a_Color);
        return drawProgram;
    }

    draw() {
        this.gl.Clear(1, 1, 1, 1);
        for (let name in this.children) {
            this.children[name].draw(GLobj.I);
        }
    }
    addChild(name, child) {
        this.children[name] = child;
        return this;
    }
    normal(x) {
        return 2 * x / this.canvasLen;
    }
}