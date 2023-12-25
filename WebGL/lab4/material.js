class PhongMaterial {
    /**
     * 创建 phong光照+贴图 纹理
     * @param {Number} specular 镜面反射系数 越大越强
     * @param {Number} diffusion 漫反射系数
     * @param {Number} ambient 环境光系数
     * @param {Number} shininess 高光系数
     * @param {Array[4]} color rgba a决定了和mesh本色的混合
     */
    constructor(specular, diffusion, ambient, shininess, color) {
        this.reflect = new Float32Array([specular, diffusion, ambient < 0 ? 0.25 : ambient, shininess]);
        this.color = new Float32Array([color[0], color[1], color[2], color[3]]);
        this.texture = null;
        this.program = null;    // 由GLobjRoot.addChild赋值
    }
    static Image2Texture(gl, image) {
        // 由于图片(包括画布)的原点在左上角，所以读取图片要翻转
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, 1);
        const texture = gl.createTexture();
        gl.bindTexture(gl.TEXTURE_2D, texture);
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);
        if (image.width & 0x1 == 0 && image.height & 0x1 == 0) {    // 是2的幂 
            gl.generateMipmap(gl.TEXTURE_2D);   // 生成的分子贴图与纹理对象绑定
        } else {
            // CLAMP_TO_EDGE 翻译过来就是边缘夹紧的意思，可以理解为任意尺寸的图像源都可以被宽高为1的uv尺寸夹紧。只有CLAMP_TO_EDGE 才能实现非二次幂图像源的显示，其它的参数都不可以。
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        }
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
        return texture;
    }
    setTexture(gl, image) {
        this.texture = PhongMaterial.Image2Texture(gl, image);
        return this;    // 支持用material = new GLmaterial().setTexture();
    }
}

class EnvMaterial {
    /**
     * 创建环境贴图纹理
     * @param {WebGLTexture} texture 环境cubemap纹理 一般传skyBox
     * @param {Array[4]} color rgba a决定了和mesh本色的混合
     * @param {Number} roughness 0~1 决定反射和颜色的混合 0为全镜面
     */
    constructor(texture, color = [1, 1, 1, 1], roughness = 0) {
        this.texture = texture;
        this.color = new Float32Array([color[0], color[1], color[2], color[3]]);
        this.roughness = roughness;
        this.program = null;    // 由GLobjRoot.addChild赋值
    }
}