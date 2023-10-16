class MyGL extends (WebGL2RenderingContext || WebGLRenderingContext) {
    // WebGLRenderingContext虽然是函数但不能作为函数用，所以不能从构造函数访问
    static new(canvas) {
        let gl = (
            canvas.getContext('webgl2')
            || canvas.getContext('webgl')
            || canvas.getContext('experimental-webgl')
        );
        if (!gl) {
            alert("无法初始化 WebGL，你的浏览器、操作系统或硬件等可能不支持 WebGL。");
            return null;
        }
        // 拓展其功能
        // gl.canvas = canvas;  // getContext已经设置了其canvas属性
        Object.setPrototypeOf(gl, MyGL.prototype);          // gl.__proto__ = MyGL.prototype; 不行，因为.__protp__已经被废弃
        MyGL.prototype.params.call(gl);
        return gl;
    }

    params() {
        // 需要保证program处于活动状态
        this._attribute = {};
        this._uniform = {};
        this.attribute = new Proxy(this._attribute, {
            // 用箭头函数，this指向MyGL；用function，this指向这个handler
            get: (target, key) => {
                // getAttribLocation只会获取在main中用到的量。定义了没用估计就被优化掉了，所以获取返回的是-1
                if (!(key in target)) {
                    const loc = this.getAttribLocation(this.program, key);
                    if (loc == -1) return null;
                    else target[key] = loc;
                }
                return target[key];
            },
            set: (target, key, value) => {
                if (!(key in target)) {
                    const loc = this.getAttribLocation(this.program, key);
                    if (loc == -1) return null;
                    else target[key] = loc;
                }
                if (value instanceof Float32Array || Array.isArray(value)) {
                    value = new Float32Array(value);
                    switch (value.length) {
                        // 如果用用超过长度的会将没涉及的置零
                        case 1: this.vertexAttrib1fv(target[key], value); break;
                        case 2: this.vertexAttrib2fv(target[key], value); break;
                        case 3: this.vertexAttrib3fv(target[key], value); break;
                        default: this.vertexAttrib4fv(target[key], value);
                    }
                } else if (typeof value == 'number') {
                    this.vertexAttrib1f(target[key], value);
                }
                else throw new Error("value not an Array!");
                return true;
            }
        });
        this.uniform = new Proxy(this._uniform, {
            // 用箭头函数，this指向MyGL；用function，this指向这个handler
            get: (target, key) => {
                if (!(key in target))
                    target[key] = this.getUniformLocation(this.program, key);
                return target[key];
                // return this.getUniform(this.program, target[key]);   // 和attribute保持一致，故不用这种。而且uniform的幅值方式有很多
            },
            set: (target, key, value) => {
                if (!(key in target))
                    target[key] = this.getUniformLocation(this.program, key);

                function d1(value) {
                    switch (value.length) {
                        case 1: this.uniform1fv(target[key], value); break;
                        case 2: this.uniform2fv(target[key], value); break;
                        case 3: this.uniform3fv(target[key], value); break;
                        // 除了2*2矩阵不能用，其他都可以用一位数组传参
                        case 9: this.uniformMatrix3fv(target[key], true, value); break;
                        case 16: this.uniformMatrix4fv(target[key], true, value); break;
                        default: this.uniform4fv(target[key], value);
                    }
                }
                function d2(value) {
                    value = new Float32Array(value.flat());
                    switch (value.length) {
                        // 如果用用超过长度的会将没涉及的置零
                        // 只有Webgl2可以用转置true
                        case 4: this.uniformMatrix2fv(target[key], true, value); break;
                        case 9: this.uniformMatrix3fv(target[key], true, value); break;
                        default: this.uniformMatrix4fv(target[key], true, value);
                    }
                }

                if (value instanceof Float32Array) {
                    d1.call(this, value);
                } else if (Array.isArray(value)) {
                    if (Array.isArray(value[0])) {  // 矩阵
                        d2.call(this, new Float32Array(value.flat()));
                    } else {
                        d1.call(this, new Float32Array(value));
                    }
                } else if (typeof value == 'number') {
                    this.uniform1f(target[key], value);
                } else throw new Error("value not an Array!");
                return true;
            }
        });
    }

    //根据着色类型，建立着色器对象
    loadShader(type, source) {
        const shader = this.createShader(type);
        //将着色器源文件传入着色器对象中
        this.shaderSource(shader, source);
        //编译着色器对象，使其成为二进制数据
        this.compileShader(shader);
        return shader;
    }

    iniProgram(vsSource, fsSource) {
        // 创建程序对象
        const program = this.createProgram();
        // 建立着色对象
        const vertexShader = this.loadShader(this.VERTEX_SHADER, vsSource);
        const fragmentShader = this.loadShader(this.FRAGMENT_SHADER, fsSource);
        // 把着色对象装进程序对象中
        this.attachShader(program, vertexShader);
        this.attachShader(program, fragmentShader);
        // 连接webgl上下文对象和程序对象
        this.linkProgram(program);
        // 暂时不启动程序对象
        // this.useProgram(program);
        // this.program = program;
        return program;
    }

    Clear(r = 0, g = 0, b = 0, a = 1) {
        // 设置清空颜色缓冲时的颜色值。颜色值域为[0, 1]，RGBA
        this.clearColor(r, g, b, a);
        // 用预设值清除缓冲区
        /* 可选：
            gl.COLOR_BUFFER_BIT     //颜色缓冲区clearColor设置
            gl.DEPTH_BUFFER_BIT     //深度缓冲区
            gl.STENCIL_BUFFER_BIT   //模板缓冲区
        */
        this.clear(this.COLOR_BUFFER_BIT | this.DEPTH_BUFFER_BIT);
    }

    iniBuffer(loc, size) {
        // createBuffer在GPU创建了缓存，而bindBuffer说明这块缓冲区用于gl.ARRAY_BUFFER。后续对 gl.ARRAY_BUFFER 的操作都会映射到这个缓存
        this.bindBuffer(this.ARRAY_BUFFER, this.createBuffer());
        // 告诉 OpenGL 如何从 Buffer 中获取数据
        this.vertexAttribPointer(
            loc,            // 顶点属性的索引
            size,           // 每个顶点的数据数量(1/2/3/4)
            this.FLOAT,     // 数据类型 gl.BYTE: 8bit; gl.SHORT: 16bit; gl.UNSIGNED_BYTE: 8bit unsigned; gl.UNSIGNED_SHORT: 16bit unsigned; gl.FLOAT: 32bit; gl.HALF_FLOAT: 16bit
            false,          // 是否归一化到特定的范围，对FLOAT无效
            0,              // stride步长。以字节为单位指定连续顶点属性开始之间的偏移量。0表示数据是紧密的没有空隙。传0是自动计算。
            0               // offset字节偏移量，必须是类型的字节长度的倍数。
        );
        // 开启 attribute 变量，使顶点着色器能够访问缓冲区数据
        this.enableVertexAttribArray(loc);
    }

    useProg(program) {
        this.program = program;
        this.useProgram(program);   
        // 多program时，每次切换program后都要重新设置attribute和uniform
        this._attribute = {};
        this._uniform = {};
    }
}