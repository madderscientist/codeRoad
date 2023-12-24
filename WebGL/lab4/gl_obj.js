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

    constructor(gl, displacement, rotation, mesh = null, material = null, scale = GLobj.I) {
        this.gl = gl;
        // 初始位置
        this.displacement = displacement;
        this.rotation = rotation;
        this.scale = scale;

        this.children = {};
        this.mesh = mesh;
        this.material = material;
        this.transform = GLobj.I;
    }
    transformation(t) {
        // 右乘！因为this.transform是右乘的。意思是坐标变换
        this.transform = MultiMat4(this.transform, t);
    }
    draw(transform) {
        const g = this.gl;
        // 计算变换矩阵 transform * displacement * rotation * transform * scale
        let t = MultiMat4(transform, this.displacement);// 位移。是左乘
        t = MultiMat4(t, this.rotation);// 旋转。是右乘
        t = MultiMat4(t, this.transform);// 额外的变化，是坐标变换，右乘
        // 以下写法是绕着原点旋转。不应该用
        // let t = MultiMat4(this.rotation, transform);
        // t = MultiMat4(t, this.displacement);

        if (this.mesh) {     // 如果是null则不画，表示为一个坐标转换
            let tempT = MultiMat4(t, this.scale);   // 缩放。需要先作用在原物体，所以右乘
            g.uniform.translation = tempT;
            g.uniform.n_translation = inverseMat4(transposeMat4(tempT));
            // material
            if(this.material) {
                g.uniform.u_material = this.material.reflect;
                g.uniform.u_color = this.material.color;
                // 纹理渲染配置 uv
                if(this.material.texture && this.mesh.map) {
                    g.uniform.u_ifTexture = '1';
                    g.bindBuffer(g.ARRAY_BUFFER, this.mesh.map);
                    g.vertexAttribPointer(
                        g.attribute.a_pin,
                        2, g.FLOAT, false,
                        0, 0
                    ); g.enableVertexAttribArray(g.attribute.a_pin);
                    g.bindTexture(g.TEXTURE_2D, this.material.texture);
                } else {
                    g.uniform.u_ifTexture = '0';
                    g.disableVertexAttribArray(g.attribute.a_pin);
                }
            }
            // mesh
            // 顶点位置绑定 xyz
            g.bindBuffer(g.ARRAY_BUFFER, this.mesh.vertexBuffer);
            g.vertexAttribPointer(
                g.attribute.a_position,
                3, g.FLOAT, false,
                0, 0
            ); g.enableVertexAttribArray(g.attribute.a_position);
            // 顶点法向绑定 xyz
            g.bindBuffer(g.ARRAY_BUFFER, this.mesh.normalBuffer);
            g.vertexAttribPointer(
                g.attribute.a_normal,
                3, g.FLOAT, false,
                0, 0
            ); g.enableVertexAttribArray(g.attribute.a_normal);
            // 颜色的绑定 rgb
            if(this.mesh.colorBuffer) {
                g.bindBuffer(g.ARRAY_BUFFER, this.mesh.colorBuffer);
                g.vertexAttribPointer(
                    g.attribute.a_color,
                    3, g.FLOAT, false,
                    0, 0
                ); g.enableVertexAttribArray(g.attribute.a_color);
            } else {
                g.disableVertexAttribArray(g.attribute.a_color);
            }
            // 序号绑定
            g.bindBuffer(g.ELEMENT_ARRAY_BUFFER, this.mesh.indexBuffer);
            // 这两个属性要求绑定在mesh.indexBuffer上！！
            g.drawElements(g.TRIANGLES, this.mesh.indexBuffer.length, this.mesh.indexBuffer.type, 0);
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

    static async fromObj(gl, objurl, position = GLobj.I, rotation= GLobj.I) {    // 需要mesh.js和parseObj.js的支持
        const objs = (await parseObj(objurl)).map(obj => new GLobj(
            gl, GLobj.I, GLobj.I,
            new GLmesh(gl, obj.vertex, obj.index, obj.normal, obj.map),
            // new GLmaterial(0.4, 0.8, -1, 600, [1,1,1,1]).setTexture(gl, obj.image)
            new GLmaterial(obj.specular, obj.diffusion, obj.ambient, obj.shininess, [1,1,1,1]).setTexture(gl, obj.image)
        ));
        const body = new GLobj(gl, position, rotation);
        for(let i in objs) body.addChild(i, objs[i]);
        return body;
    }
}
// 关于度量衡：1的单位为米，本类需要定义像素到米的转换关系
// 为什么需要这个转换？画布单位是像素，需要统一到米。而定义了相机，尤其是透视相机后，就不会被局限在2个单位的距离了
class GLobjRoot {
    constructor() {
        // 子物体
        this.children = {};
        this.gl = null;
        this.drawProgram = null;
        this.rule = 100;
        this.camera = null;
        this.pointolite = []; this.pointolite.maxLen = 10;
    }
    /**
     * 初始化GL
     * @param {canvas} canvas 画布dom
     * @param {Number} rule 比例尺 多少像素为1米
     */
    iniGL(canvas, rule) {   // viewDistance指观察距离（平行投影的）
        this.rule = rule;
        this.camera = new GLcamera([canvas.width / rule, canvas.height / rule], [0, 0, 0], 0.5, 1000);
        // GL初始化
        const gl = MyGL.new(canvas);
        this.gl = gl;
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, 1);
        gl.activeTexture(gl.TEXTURE0);
        // 用专用的着色器
        gl.useProg(this.iniDraw(gl));   // 不use也可，默认use最后ini的
    }
    iniDraw(gl) {
        const vsSource = `precision mediump float;
            attribute vec4 a_position;      // 传3个参数，则第四个参数默认为1；否则默认都是0
            attribute vec3 a_color;         // 如果禁用了此attribute，值是0
            attribute vec3 a_normal;        // 顶点法向，需要提前归一化

            uniform mat4 translation;       // 世界变化。物体本身的变化。以得到物体在世界的位置
            uniform mat4 n_translation;     // 法向的变化，应为inverse(transpose(translation))，在js里面完成
            uniform mat4 camera;            // 摄像机

            varying vec3 v_normal;          // 顶点法向在世界的原始坐标
            varying vec3 v_position;        // 像素在世界的原始坐标
            varying vec3 v_color;           // 用于从顶点颜色插值

            // 贴图
            attribute vec2 a_pin;           // 顶点uv映射坐标
            varying vec2 v_pin;             // 差值后的坐uv标
        
            void main() {
                vec4 world_position = translation*a_position;
                gl_Position = camera*world_position;
                v_normal = normalize(mat3(n_translation) * a_normal);   // a_normal默认补上的第四维是1，会引发错误
                v_position = world_position.xyz;
                v_color = a_color;
                v_pin = a_pin;
            }
        `;
        const fsSource = `precision mediump float;
            #define MAXPLNUM ${this.pointolite.maxLen}    // 最多点光源数
            uniform int u_pointoliteNum;
            uniform vec3 u_pointolitePosition[MAXPLNUM];    // 光源的世界坐标
            uniform vec3 u_pointoliteColor[MAXPLNUM];

            uniform vec3 u_amblightColor;   // 环境光颜色

            uniform vec3 u_cameraPosition;  // 观察者的位置

            uniform vec4 u_color;       // 物体材质确定的颜色
            uniform vec4 u_material;    // [镜面反射系数, 漫反射系数, 环境光反射系数, 高光系数]

            varying vec3 v_normal;      // 片元法向
            varying vec3 v_position;    // 像素在世界的原始坐标
            varying vec3 v_color;       // 由顶点着色器传入的顶点定义的颜色

            // 贴图
            uniform bool u_ifTexture;   // 是否用贴图
            uniform sampler2D u_sampler;    // 二维取样器
            varying vec2 v_pin;

            void main() {
                vec3 objColor = u_color.a * u_color.rgb + (1.0-u_color.a) * v_color;    // 物体本来的颜色，由来两个颜色来源加权得到
                if (u_ifTexture) {          // 如果开启了贴图
                    objColor *= texture2D(u_sampler, v_pin).xyz;
                }
                vec3 n = normalize(v_normal);   // 插值后不一定是单位向量
                vec3 diffuse = vec3(0.0);
                vec3 specular = vec3(0.0);
                vec3 ambient = u_material[2]*objColor*u_amblightColor;  // 环境光
                for(int i=0; i<MAXPLNUM; i++) {
                    if(i>=u_pointoliteNum) break;   // 在GLSL中，for循环的条件必须是一个常量表达式。只能这样写
                    vec3 lightDirection = normalize(u_pointolitePosition[i] - v_position);
                    diffuse += u_material[1]*objColor*u_pointoliteColor[i]*max(dot(lightDirection, n), 0.0);
                    specular += u_material[0]*u_pointoliteColor[i]*pow(max(0.0, dot(n,normalize(u_cameraPosition-v_position))), u_material[3]);
                }
                gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
            }
        `;
        let drawProgram = gl.iniProgram(vsSource, fsSource)
        gl.useProg(drawProgram);
        gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);
        gl.enable(gl.DEPTH_TEST);
        gl.uniform.translation = GLobj.I;
        gl.uniform.camera = GLobj.I;
        gl.uniform.u_amblightColor = new Float32Array([1, 1, 1]);   // 默认白色环境光
        gl.uniform.u_pointoliteNum = '0';
        gl.uniform.u_sampler = '0';
        return drawProgram;
    }
    //========光照管理=========//
    // 设计理念时光源不常动，因此在添加、删除时更新
    // 为每个点光源维护了一个id，用于某个光源需要移动，只更新这一个光源
    addLight(pointolite) {
        const l = this.pointolite;
        if (l.length > l.maxLen) throw new Error('超过最大点光源数目！');
        let i = l.push(pointolite) - 1;
        pointolite.id = i;
        this.gl.uniform[`u_pointolitePosition[${i}]`] = pointolite.position;
        this.gl.uniform[`u_pointoliteColor[${i}]`] = pointolite.color;
        this.gl.uniform.u_pointoliteNum = l.length.toString();  // 因为一些局限性只能这样写了 见gl拓展类的uniform的set函数
        return pointolite;
    }
    removeLight(id) {
        const l = this.pointolite;
        if (id < 0 || id >= l.length) return;
        l.splice(id, 1);
        for (; id < l.length; id++) {
            l[i].id = id;
            this.gl.uniform[`u_pointolitePosition[${id}]`] = l[id].position;
            this.gl.uniform[`u_pointoliteColor[${id}]`] = l[id].color;
        }
        this.gl.uniform.u_pointoliteNum = l.length.toString();
    }
    updateLight(id = -1) {  // 可以选择一个更新，也可以全部更新
        const l = this.pointolite;
        if (id < 0 || id >= l.length) {
            for (let i = 0; i < l.length; i++) {
                this.gl.uniform[`u_pointolitePosition[${i}]`] = l[i].position;
                this.gl.uniform[`u_pointoliteColor[${i}]`] = l[i].color;
            }
        } else {
            this.gl.uniform[`u_pointolitePosition[${id}]`] = l[id].position;
            this.gl.uniform[`u_pointoliteColor[${id}]`] = l[id].color;
        }
    }

    draw() {
        this.gl.Clear(0, 0, 0, 0);
        this.gl.uniform.camera = this.camera.Mat();
        this.gl.uniform.u_cameraPosition = this.camera.position;
        for (let name in this.children) {
            this.children[name].draw(GLobj.I);
        }
    }
    addChild(name, child) {
        this.children[name] = child;
        return child;
    }
    /**
     * 像素到米的单位转换
     * @param {Number} pixel 像素
     * @returns 米
     */
    normal(pixel) {
        return pixel / this.rule;
    }
    /**
     * 更改画布大小
     * @param {Number} w 新宽度 默认充满 单位像素
     * @param {Number} h 新宽度 默认充满 单位像素
     */
    resizeCanvas(w, h) {
        const c = this.gl.canvas;
        c.width = w || window.innerWidth;
        c.height = h || window.innerHeight;
        this.camera.canvasSize = [c.width / this.rule, c.height / this.rule];
        this.gl.viewport(0, 0, c.width, c.height);
    }
}

class Pointolite {
    /**
     * 定义一个全向点光源
     * @param {Array} color 四维向量，rgba，值大于零即可，越大越亮。透明度用于和mesh定义的颜色混合
     * @param {*} position 三维向量，xyz，世界坐标
     */
    constructor(color, position) {
        this.color = new Float32Array([color[0], color[1], color[2]]);
        this.position = new Float32Array([position[0], position[1], position[2]]);
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