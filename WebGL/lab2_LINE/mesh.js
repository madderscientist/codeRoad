class GLmesh {
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

    constructor(vertix = [], colorid = null) {
        // 二维数组
        // 按照一个顶点：x,y,z,r,g,b的方式排布
        this.vertix = vertix;
        // 在识别点击的时候的特异性颜色 颜色用8位表示法
        this.colorid = colorid || GLmesh.uniqueColor(true);
    }
    vertixArray() {
        return new Float32Array(this.vertix.map(v => v.slice(0, 3)).flat());
    }
    colorArray() {
        return new Float32Array(this.vertix.map(v => v.slice(3, 6)).flat());
    }
    static bind(v, c) {
        let vertix = [];
        // 将v和c合并为this.vertix的格式
        if (c.length != v.length) { // 视为同色 要求c是一个颜色数组
            for (let i = 0; i < v.length; i += 3) {
                vertix.push([...v.slice(i, i + 3), ...c]);
            }
        } else {
            for (let i = 0; i < v.length; i += 3) {
                vertix.push([...v.slice(i, i + 3), ...c.slice(i, i + 3)]);
            }
        }
        return vertix;
    }
    idColorArray() {
        return GLmesh.IDtoCOLOR(this.colorid);
    }
}

class GLmesh_A extends GLmesh {
    static fromSTL(data) {  // 输入是读取的文件内容
        let n = data[83] << 24 | data[82] << 16 | data[81] << 8 | data[80];
        console.log(n);
        const v = new Float32Array(n * 9); let vn = 0;
        let i = 84;
        for (; n > 0; n--) {
            i += 12;    // 跳过法向量
            for (let j = 0; j < 9; j++) {   // 3个顶点9个坐标
                let d = new DataView(data.buffer, i, 4);
                v[vn++] = d.getFloat32(0, true);
                i += 4;
            }
            i += 2;     // 跳过属性
        }
        return v;
    }
    static STL(data) {
        const v = GLmesh_A.bindWithID(GLmesh_A.fromSTL(data)); // 修改了
        return new GLmesh_A(v);
    }
    constructor(vertix = [], colorid = null) {
        super(vertix, colorid);
        this.mesh = this.meshArray();
    }
    meshArray() {
        return new Float32Array(this.vertix.flat());
    }
    static bindWithID(v) {   // 修改了
        let vertix = [];
        // 将v和c合并为this.vertix的格式
        for (let i = 0; i < v.length; i += 3) {
            vertix.push([...v.slice(i, i + 3), (i/3)%3]);
        }
        return vertix;
    }
}