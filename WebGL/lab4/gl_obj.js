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

    constructor(displacement, rotation, mesh = null, material = null, scale = GLobj.I) {
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
        // 计算变换矩阵 transform * displacement * rotation * transform * scale
        let t = MultiMat4(transform, this.displacement);// 位移。是左乘
        t = MultiMat4(t, this.rotation);// 旋转。是右乘
        t = MultiMat4(t, this.transform);// 额外的变化，是坐标变换，右乘
        // 以下写法是绕着原点旋转。不应该用
        // let t = MultiMat4(this.rotation, transform);
        // t = MultiMat4(t, this.displacement);

        if (this.mesh) {     // 如果是null则不画，表示为一个坐标转换
            this.material.program.draw({
                mesh: this.mesh,
                material: this.material,
                translation: MultiMat4(t, this.scale)   // 缩放。需要先作用在原物体，所以右乘
            });
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

    static async fromObj(gl, objurl, position = GLobj.I, rotation = GLobj.I) {    // 需要mesh.js和parseObj.js的支持
        const objs = (await parseObj(objurl)).map(obj => new GLobj(
            GLobj.I, GLobj.I,
            new GLmesh(gl, obj.vertex, obj.index, obj.normal, obj.map),
            // new GLmaterial(0.4, 0.8, -1, 600, [1,1,1,1]).setTexture(gl, obj.image)
            new PhongMaterial(obj.specular, obj.diffusion, obj.ambient, obj.shininess, [1, 1, 1, 1]).setTexture(gl, obj.image)
        ));
        const body = new GLobj(position, rotation);
        for (let i in objs) body.addChild(i, objs[i]);
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
        this.programs = {};
        this.pointolite = []; this.pointolite.maxLen = 10;
        this.skyBox = {
            texture: null,
            vertexBuffer: null
        };   // 天空盒
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
        gl.activeTexture(gl.TEXTURE0);
        // 用专用的着色器
        this.programs['skyBox'] = this.iniSkyBox(gl);
        this.programs['env'] = this.iniEnv(gl);
        this.programs['phong'] = this.iniBasic(gl);
        gl.viewport(0, 0, canvas.width, canvas.height);
        gl.enable(gl.DEPTH_TEST);
        gl.cullFace(gl.FRONT);
    }
    //========着色器=========//
    // 贴图平滑光照
    iniBasic(gl) {
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
        let drawProgram = gl.iniProgram(vsSource, fsSource);
        gl.useProg(drawProgram);
        // 初值
        gl.uniform.camera = GLobj.I;
        gl.uniform.u_amblightColor = new Float32Array([1, 1, 1]);   // 默认白色环境光
        gl.uniform.u_pointoliteNum = '0';
        gl.uniform.u_sampler = '0';
        // 着色器配置
        drawProgram.draw = ({mesh, material, translation}) => {
            gl.useProg(drawProgram);
            gl.uniform.translation = translation;
            gl.uniform.n_translation = inverseMat4(transposeMat4(translation));
            if (material) {
                gl.uniform.u_material = material.reflect;
                gl.uniform.u_color = material.color;
                // 纹理渲染配置 uv
                if (material.texture && mesh.map) {
                    gl.uniform.u_ifTexture = '1';
                    gl.bindBuffer(gl.ARRAY_BUFFER, mesh.map);
                    gl.vertexAttribPointer(
                        gl.attribute.a_pin,
                        2, gl.FLOAT, false,
                        0, 0
                    ); gl.enableVertexAttribArray(gl.attribute.a_pin);
                    // gl.activeTexture(gl.TEXTURE0);   // 默认只用0
                    gl.bindTexture(gl.TEXTURE_2D, material.texture);
                } else {
                    gl.uniform.u_ifTexture = '0';
                    gl.disableVertexAttribArray(gl.attribute.a_pin);
                }
            }
            // 顶点位置绑定 xyz
            gl.bindBuffer(gl.ARRAY_BUFFER, mesh.vertexBuffer);
            gl.vertexAttribPointer(
                gl.attribute.a_position,
                3, gl.FLOAT, false,
                0, 0
            ); gl.enableVertexAttribArray(gl.attribute.a_position);
            // 顶点法向绑定 xyz
            gl.bindBuffer(gl.ARRAY_BUFFER, mesh.normalBuffer);
            gl.vertexAttribPointer(
                gl.attribute.a_normal,
                3, gl.FLOAT, false,
                0, 0
            ); gl.enableVertexAttribArray(gl.attribute.a_normal);
            // 颜色的绑定 rgb
            if (mesh.colorBuffer) {
                gl.bindBuffer(gl.ARRAY_BUFFER, mesh.colorBuffer);
                gl.vertexAttribPointer(
                    gl.attribute.a_color,
                    3, gl.FLOAT, false,
                    0, 0
                ); gl.enableVertexAttribArray(gl.attribute.a_color);
            } else {
                gl.disableVertexAttribArray(gl.attribute.a_color);
            }
            // 序号绑定
            gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, mesh.indexBuffer);
            // 这两个属性要求绑定在mesh.indexBuffer上！！
            gl.drawElements(gl.TRIANGLES, mesh.indexBuffer.length, mesh.indexBuffer.type, 0);
        };
        return drawProgram;
    }
    // 天空盒
    iniSkyBox(gl) {
        const vsSource = `precision mediump float;
            attribute vec4 a_position;
            varying vec4 v_position;
            void main() {
                v_position = a_position;
                gl_Position = a_position;
                gl_Position.z = 1.0;
            }
        `;
        const fsSource = `precision mediump float;
            uniform samplerCube u_skybox;
            uniform mat4 camera;
            varying vec4 v_position;
            void main() {
                vec4 t = camera * v_position;
                gl_FragColor = textureCube(u_skybox, normalize(t.xyz / t.w));
            }
        `;
        let skyBoxProgram = gl.iniProgram(vsSource, fsSource);
        gl.useProg(skyBoxProgram);
        gl.uniform.camera = GLobj.I;
        gl.uniform.u_skybox = '0';
        // skyBoxProgram.draw不外泄 在GLobjRoot.draw中
        return skyBoxProgram;
    }
    // 环境贴图
    iniEnv(gl) {
        const vsSource = `precision mediump float;
            attribute vec4 a_position;
            attribute vec3 a_normal;
            attribute vec3 a_color;
            
            uniform mat4 translation;
            uniform mat4 n_translation;
            uniform mat4 camera;        // 统一配置
            
            varying vec3 v_position;    // 世界坐标
            varying vec3 v_normal;      // 世界法向
            varying vec3 v_color;       // 由顶点着色器传入的顶点定义的颜色
            
            void main() {
                vec4 world_position = translation*a_position;
                gl_Position = camera*world_position;
                v_normal = normalize(mat3(n_translation) * a_normal);
                v_position = world_position.xyz;
                v_color = a_color;
            }
        `;
        const fsSource = `precision mediump float;
            varying vec3 v_normal;      // 片元法向
            varying vec3 v_position;    // 像素在世界的原始坐标
            varying vec3 v_color;       // 网格自带的颜色

            uniform samplerCube u_environment;  // 环境贴图
            uniform vec3 u_cameraPosition;  // 观察者的位置 统一配置
            uniform float u_roughness;      // 粗糙度 0~1 决定和本色的混合
            uniform vec4 u_color;           // 物体材质确定的颜色
            
            void main() {
                vec3 objColor = u_color.a * u_color.rgb + (1.0-u_color.a) * v_color;    // 物体本来的颜色，由来两个颜色来源加权得到
                vec3 worldNormal = normalize(v_normal);
                vec3 eyeToSurfaceDir = normalize(v_position - u_cameraPosition);
                vec3 direction = reflect(eyeToSurfaceDir, worldNormal);
                gl_FragColor = vec4((1.0 - u_roughness) * textureCube(u_environment, direction).rgb + u_roughness * objColor, 1.0);
            }
        `;
        let envProgram = gl.iniProgram(vsSource, fsSource);
        // 初值
        gl.useProg(envProgram);
        gl.uniform.camera = GLobj.I;
        gl.uniform.u_environment = '0';
        // 着色器配置
        envProgram.draw = ({mesh, material, translation}) => {
            gl.useProg(envProgram);
            gl.uniform.translation = translation;
            gl.uniform.n_translation = inverseMat4(transposeMat4(translation));
            gl.uniform.u_roughness = material.roughness;
            gl.uniform.u_color = material.color;
            // gl.activeTexture(gl.TEXTURE0);   // 默认只用0
            gl.bindTexture(gl.TEXTURE_CUBE_MAP, material.texture);
            // 顶点位置绑定 xyz
            gl.bindBuffer(gl.ARRAY_BUFFER, mesh.vertexBuffer);
            gl.vertexAttribPointer(
                gl.attribute.a_position,
                3, gl.FLOAT, false,
                0, 0
            ); gl.enableVertexAttribArray(gl.attribute.a_position);
            // 顶点法向绑定 xyz
            gl.bindBuffer(gl.ARRAY_BUFFER, mesh.normalBuffer);
            gl.vertexAttribPointer(
                gl.attribute.a_normal,
                3, gl.FLOAT, false,
                0, 0
            ); gl.enableVertexAttribArray(gl.attribute.a_normal);
            // 颜色的绑定 rgb
            if (mesh.colorBuffer) {
                gl.bindBuffer(gl.ARRAY_BUFFER, mesh.colorBuffer);
                gl.vertexAttribPointer(
                    gl.attribute.a_color,
                    3, gl.FLOAT, false,
                    0, 0
                ); gl.enableVertexAttribArray(gl.attribute.a_color);
            } else {
                gl.disableVertexAttribArray(gl.attribute.a_color);
            }
            // 序号绑定
            gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, mesh.indexBuffer);
            // 这两个属性要求绑定在mesh.indexBuffer上！！
            gl.drawElements(gl.TRIANGLES, mesh.indexBuffer.length, mesh.indexBuffer.type, 0);
        };
        return envProgram;
    }
    //========光照管理=========//
    // 设计理念时光源不常动，因此在添加、删除时更新
    // 为每个点光源维护了一个id，用于某个光源需要移动，只更新这一个光源
    addLight(pointolite) {
        const l = this.pointolite;
        if (l.length > l.maxLen) throw new Error('超过最大点光源数目！');
        let i = l.push(pointolite) - 1;
        pointolite.id = i;
        this.gl.useProg(this.programs['phong']);
        this.gl.uniform[`u_pointolitePosition[${i}]`] = pointolite.position;
        this.gl.uniform[`u_pointoliteColor[${i}]`] = pointolite.color;
        this.gl.uniform.u_pointoliteNum = l.length.toString();  // 因为一些局限性只能这样写了 见gl拓展类的uniform的set函数
        return pointolite;
    }
    removeLight(id) {
        const l = this.pointolite;
        if (id < 0 || id >= l.length) return;
        l.splice(id, 1);
        this.gl.useProg(this.programs['phong']);
        for (; id < l.length; id++) {
            l[i].id = id;
            this.gl.uniform[`u_pointolitePosition[${id}]`] = l[id].position;
            this.gl.uniform[`u_pointoliteColor[${id}]`] = l[id].color;
        }
        this.gl.uniform.u_pointoliteNum = l.length.toString();
    }
    updateLight(id = -1) {  // 可以选择一个更新，也可以全部更新
        const l = this.pointolite;
        this.gl.useProg(this.programs['phong']);
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
        const g = this.gl;
        g.Clear(0, 0, 0, 0);
        const cameraP = this.camera.perspectiveMat();
        const cameraL = this.camera.postureMat();
        const camera = MultiMat4(cameraP, cameraL);

        // 准备着色器
        g.useProg(this.programs['env']);
        g.uniform.camera = camera;
        g.uniform.u_cameraPosition = this.camera.position;
        g.useProg(this.programs['phong']);
        g.uniform.camera = camera;
        g.uniform.u_cameraPosition = this.camera.position;
        // 绘制物体
        g.depthFunc(g.LESS);
        g.enable(g.CULL_FACE);
        for (let name in this.children) {
            this.children[name].draw(GLobj.I);
        }
        // 绘制天空盒
        if (this.skyBox.texture) {
            g.depthFunc(g.LEQUAL);  // 让背景可以画在1的位置
            g.disable(g.CULL_FACE);
            g.useProg(this.programs['skyBox']);
            let viewMatrix = cameraL;
            viewMatrix[3] = 0; viewMatrix[7] = 0; viewMatrix[11] = 0; // 保证天空盒不会移动。没有这行效果很奇怪
            g.uniform.camera = inverseMat4(MultiMat4(cameraP, viewMatrix));
            g.bindBuffer(g.ARRAY_BUFFER, this.skyBox.vertexBuffer);
            g.vertexAttribPointer(
                g.attribute.a_position,
                2, g.FLOAT, false, 0, 0
            ); g.enableVertexAttribArray(g.attribute.a_position);
            g.drawArrays(g.TRIANGLES, 0, 6);
        }
    }
    //========天空盒管理=========//
    setSkyBox({ nx, ny, nz, px, py, pz }) {   // 输入是可以直接用的img
        const g = this.gl;
        g.pixelStorei(g.UNPACK_FLIP_Y_WEBGL, 0);    // 立方体贴图（cubemap）时，通常不需要翻转 y 轴。这是因为立方体贴图的六个面的纹理坐标系统与 WebGL 的纹理坐标系统是一致的，都是原点（0,0）在左下角。
        g.useProg(this.programs['skyBox']);
        if (this.skyBox.texture) g.deleteTexture(this.skyBox.texture);
        this.skyBox.texture = g.createTexture();
        g.bindTexture(g.TEXTURE_CUBE_MAP, this.skyBox.texture);
        g.texImage2D(g.TEXTURE_CUBE_MAP_NEGATIVE_X, 0, g.RGBA, g.RGBA, g.UNSIGNED_BYTE, nx);
        g.texImage2D(g.TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, g.RGBA, g.RGBA, g.UNSIGNED_BYTE, ny);
        g.texImage2D(g.TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, g.RGBA, g.RGBA, g.UNSIGNED_BYTE, nz);
        g.texImage2D(g.TEXTURE_CUBE_MAP_POSITIVE_X, 0, g.RGBA, g.RGBA, g.UNSIGNED_BYTE, px);
        g.texImage2D(g.TEXTURE_CUBE_MAP_POSITIVE_Y, 0, g.RGBA, g.RGBA, g.UNSIGNED_BYTE, py);
        g.texImage2D(g.TEXTURE_CUBE_MAP_POSITIVE_Z, 0, g.RGBA, g.RGBA, g.UNSIGNED_BYTE, pz);
        g.texParameteri(g.TEXTURE_CUBE_MAP, g.TEXTURE_MIN_FILTER, g.LINEAR);
        g.texParameteri(g.TEXTURE_CUBE_MAP, g.TEXTURE_MAG_FILTER, g.LINEAR);
        g.texParameteri(g.TEXTURE_CUBE_MAP, g.TEXTURE_WRAP_S, g.CLAMP_TO_EDGE);
        g.texParameteri(g.TEXTURE_CUBE_MAP, g.TEXTURE_WRAP_T, g.CLAMP_TO_EDGE);
        g.texParameteri(g.TEXTURE_CUBE_MAP, g.TEXTURE_WRAP_R, g.CLAMP_TO_EDGE);
        if (!this.skyBox.vertexBuffer) {
            this.skyBox.vertexBuffer = g.createBuffer();
            g.bindBuffer(g.ARRAY_BUFFER, this.skyBox.vertexBuffer);
            g.bufferData(g.ARRAY_BUFFER, new Float32Array([-1, -1, 1, -1, -1, 1, -1, 1, 1, -1, 1, 1]), g.STATIC_DRAW);
        }
        console.log('天空盒设置成功');
    }
    setSkyBoxFromUrl({ nx, ny, nz, px, py, pz }) {
        const promises = [nx, ny, nz, px, py, pz].map(url => new Promise((resolve, reject) => {
            const img = new Image();
            img.src = url;
            img.onload = () => resolve(img);
            img.onerror = (e) => {
                console.error(e);
                reject(url);
            };
        }));
        return Promise.all(promises).then(imgs => {
            this.setSkyBox({
                nx: imgs[0], ny: imgs[1], nz: imgs[2],
                px: imgs[3], py: imgs[4], pz: imgs[5]
            });
        }).catch(url => console.error(`天空盒图片加载失败：${url}`));
    }

    /**
     * 加入世界树 会自动为材质配置着色器
     * @param {String} name 
     * @param {GLobj} child 
     * @returns {GLobj}
     */
    addChild(name, child) {
        this.children[name] = child;
        const addProgram = (parent) => {
            // 赋予着色器
            this.setMaterialProgram(parent.material);
            // 递归
            for (let c in parent.children) addProgram(parent.children[c]);
        }
        addProgram(child);
        return child;
    }
    /**
     * 给材质配置本gl的着色器
     * @param {PhongMaterial | EnvMaterial} material 材质类
     * @returns {PhongMaterial | EnvMaterial} 配置好着色器的material
     */
    setMaterialProgram(material) {
        if(material instanceof PhongMaterial) material.program = this.programs['phong'];
        else if(material instanceof EnvMaterial) material.program = this.programs['env'];
        return material;
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
/* 创建一个材质：
首先新建材质类
其次在GLobjRoot中新增着色器生成函数，要求返回编译好的着色器对象，其draw属性完成绘制，传参为{mesh, material, translation}
接着，在GLobjRoot.iniGL中调用新的着色器生成函数并保存至GLobjRoot.programs
最后，在GLobjRoot.setMaterialProgram中关联两者
 */