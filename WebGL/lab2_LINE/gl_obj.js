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
// 叉乘
function Cross(a, b) {
    return new Float32Array([
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0]
    ]);
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
        // 初始位置
        this.displacement = displacement;
        this.rotation = rotation;
        this.scale = scale;

        this.children = {};
        this.mesh = mesh;
        this.transform = GLobj.I;
    }
    transformation(t) {
        // 右乘！因为this.transform是右乘的。意思是坐标变换
        this.transform = MultiMat4(this.transform, t);
    }
    draw(transform) {
        const g = this.gl;
        // 计算变换矩阵 transform * displacement * rotation * transform * scale
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
            if (!this.mesh.mesh) this.mesh.mesh = this.mesh.meshArray();
            g.uniform.translation = MultiMat4(t, this.scale);   // 缩放。需要先作用在原物体，所以右乘
            const v = this.mesh.mesh;
            g.bufferData(g.ARRAY_BUFFER, v, g.DYNAMIC_DRAW);
            g.drawArrays(g.TRIANGLES, 0, v.length / 4);
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
        
            // 修改为传参 WEBGL没有gl_VertexID只能出此下策
            attribute float a_Barycentric;  // 只能float、vec2、vec3、vec4、mat2、mat3或mat4类型
            varying vec3 v_Barycentric;
        
            void main() {
                gl_Position = canvas_scale*camera_scale*camera_rotation*camera_displace* translation*a_Position;

                // int index = int(mod(a_Barycentric, 3.0)); 取模的计算用cpu了
                int index = int(a_Barycentric);
                if (index == 0) {
                    v_Barycentric = vec3(1.0, 0.0, 0.0);
                } else if (index == 1) {
                    v_Barycentric = vec3(0.0, 1.0, 0.0);
                } else {
                    v_Barycentric = vec3(0.0, 0.0, 1.0);
                }
            }
        `;
        const fsSource = `
            precision mediump float;
            varying vec3 v_Barycentric;

            void main() {
                gl_FragColor = vec4(0.5, 0.5, 0.5, 1.0);

                if (any(lessThan(v_Barycentric, vec3(0.01)))) {
                    // 边框颜色
                    gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
                } else {
                    // 填充背景颜色
                    gl_FragColor = vec4(0.5, 0.5, 0.5, 1.0);
                }
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
            4 * Float32Array.BYTES_PER_ELEMENT,
            0
        );
        gl.enableVertexAttribArray(gl.attribute.a_Position);
        // 重心坐标
        gl.vertexAttribPointer(
            gl.attribute.a_Barycentric,
            1, gl.FLOAT, false,
            4 * Float32Array.BYTES_PER_ELEMENT,
            3 * Float32Array.BYTES_PER_ELEMENT
        );
        gl.enableVertexAttribArray(gl.attribute.a_Barycentric);
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
        return this;    // 为了能连续addChild
    }
    normal(x) {
        return 2 * x / this.canvasLen;
    }
}

class VirtualTrackingBall {
    constructor(canvas) {
        this.canvas = canvas;
    }
    // 由屏幕的(X,Y)映射到球面坐标(X,Y,Z)
    MapToSphere(point) {
        let x = 2 * point[0] / this.canvas.width - 1;
        let y = 1 - 2 * point[1] / this.canvas.height;
        let z = 0;
        let length = x * x + y * y;
        if (length > 1) {
            let norm = 1 / Math.sqrt(length);
            x *= norm;
            y *= norm;
        } else {
            z = Math.sqrt(1 - length);
        }
        return new Float32Array([x, y, z]);
    }

    static trackBallRot(last, current) {
        // 叉乘得到旋转轴，其模为sin(angle)
        let axis = Cross(last, current);
        // angle 约等于 sin(angle)
        let angle = Math.sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);
        // 计算旋转矩阵
        axis = [axis[0] / angle, axis[1] / angle, axis[2] / angle];
        const d = Math.sqrt(axis[1] * axis[1] + axis[2] * axis[2]);
        // 先将axis旋转到z轴
        const Rx = new Float32Array([
            1, 0, 0, 0,
            0, axis[2] / d, -axis[1] / d, 0,
            0, axis[1] / d, axis[2] / d, 0,
            0, 0, 0, 1
        ]);
        const Ry = new Float32Array([
            d, 0, -axis[0], 0,
            0, 1, 0, 0,
            axis[0], 0, d, 0,
            0, 0, 0, 1
        ]);
        // 以上两个矩阵的逆
        const Ry_ = new Float32Array([
            d, 0, axis[0], 0,
            0, 1, 0, 0,
            -axis[0], 0, d, 0,
            0, 0, 0, 1
        ]);
        const Rx_ = new Float32Array([
            1, 0, 0, 0,
            0, axis[2] / d, axis[1] / d, 0,
            0, -axis[1] / d, axis[2] / d, 0,
            0, 0, 0, 1
        ]);
        const sin = angle, cos = Math.sqrt(1 - sin * sin);
        // 绕z旋转。注意这里对角度进行了取反，相当于旋转矩阵的逆，即摄像机反向运动
        let Rz = new Float32Array([
            cos, sin, 0, 0,
            -sin, cos, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        ]);
        return MultiMat4(Rx_, MultiMat4(Ry_, MultiMat4(Rz, MultiMat4(Ry, Rx))));
    }
}