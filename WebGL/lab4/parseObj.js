// 按有状态机的思路解析obj文件
/* 遇到mtllib时
1. 处理之前的mtllib：将每个有数据的材质打包
2. 加载新的mtl文件，解析后返回材质数组
mtl = {
    "头发": {   // 属性名来自于mtl文件的newmtl
        url: 图片的地址,
        Ks: [r,g,b],
        Kd: [r,g,b],
        Ka: [r,g,b],
        Ni: Number,
        // 以下是打包时用到的数据，由parseObj函数添加并填充
        index: [],
        vertex: [],
        normal: [],
        map: [],
        set: {},
        newID: 0
    },
    "头饰": {
        ...
    },
}
*/

/**
 * 打包材质对应对象
 * @param {Object} mtl 自定义的材质对象
 * @returns {Array} 打包好的对象数组 example:
 * [
 *    {
 *       index: Array,  // 数组都是一维的
 *       vertex: Array,
 *       normal: Array,
 *       map: Array,
 *       image: 已经加载好的Image对象,  // 转换成纹理对象由material类完成
 *       specular: Number,
 *       diffusion: Number,
 *       ambient: Number,
 *       shininess: Number
 *   }, ...
 * ]
 */
async function mtlCollect(mtl) {
    const objs = [];
    let promises = [];
    for (const m in mtl) {
        const mat = mtl[m];
        if (mat.index.length) {    // 有数据的材质打包
            promises.push(new Promise((resolve, reject) => {
                const img = new Image();
                img.onload = () => {
                    objs.push({
                        index: mat.index,
                        vertex: mat.vertex.flat(),
                        normal: mat.normal.flat(),
                        map: mat.map.flat(),
                        image: img,
                        specular: (mat.Ks[0] + mat.Ks[1] + mat.Ks[2]) / 12,
                        diffusion: (mat.Kd[0] + mat.Kd[1] + mat.Kd[2]) / 12,
                        ambient: (mat.Ka[0] + mat.Ka[1] + mat.Ka[2]) / 12,
                        shininess: mat.Ni,
                    });
                    resolve();
                }
                img.onerror = () => reject(new Error(`load ${mat.url} fail`));
                img.src = mat.url;
            }));
        }
    }
    await Promise.all(promises);
    return objs;
}

/**
 * 解析mtl文件
 * @param {String} mtlurl mtl地址
 * @returns {Object} 自定义的材质对象 example:
 * {
     "头发": {
         url: 图片的地址,
         Ks: [r,g,b],
         Kd: [r,g,b],
         Ka: [r,g,b],
         Ni: Number
     },
     "头饰": {
         ...
     },
 }
 */
async function parseMtl(mtlurl) {
    const response = await fetch(mtlurl);
    const text = await response.text();
    const lines = text.split('\n');
    const materials = {};
    let currentMaterial = null;
    for (let line of lines) {
        line = line.trim();
        if (line.startsWith('#')) continue;     // 跳过注释
        const parts = line.split(/\s+/);
        if (parts.length < 2) continue;
        const key = parts.shift();
        switch (key) {
            case 'newmtl':
                currentMaterial = {
                    url: '',
                    Ks: [0, 0, 0],
                    Kd: [0, 0, 0],
                    Ka: [0, 0, 0],
                    Ni: 0
                };
                materials[parts[0]] = currentMaterial;
                break;
            case 'Kd':
                currentMaterial.Kd = [parseFloat(parts[0]), parseFloat(parts[1]), parseFloat(parts[2])];
                break;
            case 'Ks':
                currentMaterial.Ks = [parseFloat(parts[0]), parseFloat(parts[1]), parseFloat(parts[2])];
                break;
            case 'Ka':
                currentMaterial.Ka = [parseFloat(parts[0]), parseFloat(parts[1]), parseFloat(parts[2])];
                break;
            case 'Ni':
                currentMaterial.Ni = parseFloat(parts[0]);
                break;
            case 'map_Kd':
                currentMaterial.url = parts[0];
                break;
        }
    }
    return materials;
}
/**
 * 解析obj文件
 * @param {String} objurl 
 * @returns 同mtlCollect的返回结果
  * [
 *    {
 *       index: Array,  // 数组都是一维的
 *       vertex: Array,
 *       normal: Array,
 *       map: Array,
 *       image: 已经加载好的Image对象,  // 转换成纹理对象由material类完成
 *       specular: Number,
 *       diffusion: Number,
 *       ambient: Number,
 *       shininess: Number
 *   }, ...
 * ]
 */
async function parseObj(objurl) {
    // 加载obj文件
    const response = await fetch(objurl);
    const text = await response.text();
    // 以行为单位处理
    const lines = text.split('\n');
    // 所有数据
    const Positions = [];
    const Normals = [];
    const UV = [];
    let currentMtllib = null;
    let currentMaterial = null;
    // 读取的结果
    const objs = [];
    for (let line of lines) {
        line = line.trim();
        if (line.startsWith('#')) continue;     // 跳过注释
        const parts = line.split(/\s+/);
        if (parts.length < 2) continue;
        const key = parts.shift();
        switch (key) {
            case 'v':
                Positions.push([parseFloat(parts[0]), parseFloat(parts[1]), parseFloat(parts[2])]);
                break;
            case 'vn':
                Normals.push([parseFloat(parts[0]), parseFloat(parts[1]), parseFloat(parts[2])]);
                break;
            case 'vt':
                UV.push([parseFloat(parts[0]), parseFloat(parts[1])]);
                break;
            case 'mtllib':
                // 处理之前的mtllib
                if (currentMtllib) objs.push(...(await mtlCollect(currentMtllib)));
                // 更新mtllib
                const mtlurl = new URL(parts[0], objurl).href;
                currentMtllib = await parseMtl(mtlurl);
                for (let m in currentMtllib) {
                    currentMtllib[m].index = [];
                    currentMtllib[m].vertex = [];
                    currentMtllib[m].normal = [];
                    currentMtllib[m].map = [];
                    currentMtllib[m].set = {};
                    currentMtllib[m].newID = 0;
                }
                break;
            case 'usemtl':
                currentMaterial = currentMtllib[parts[0]];
                break;
            case 'f':
                for (let f of parts) {
                    if (f in currentMaterial.set) {
                        currentMaterial.index.push(currentMaterial.set[f]);
                    } else {
                        const indexArray = f.split('/').map(i => parseInt(i) - 1);
                        currentMaterial.index.push(currentMaterial.newID);
                        currentMaterial.set[f] = currentMaterial.newID;
                        currentMaterial.newID++;
                        currentMaterial.vertex.push(Positions[indexArray[0]]);
                        currentMaterial.map.push(UV[indexArray[1]]);
                        currentMaterial.normal.push(Normals[indexArray[2]]);
                    }
                }
                break;
        }
    }
    if (currentMtllib) objs.push(...(await mtlCollect(currentMtllib)));
    return objs;
}