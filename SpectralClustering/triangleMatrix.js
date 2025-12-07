class TriangleMatrix {
    constructor(size) {
        this.size = size;
        this.data = new Float32Array((size * (size + 1)) / 2);
    }

    // 发现效果比exp(cos-1)好
    static cosineAffinityExp(featureA, featureB) {
        let dot = 0;
        for (let i = 0; i < featureA.length; i++) {
            dot += featureA[i] * featureB[i];
        }
        return Math.exp(dot);
    }

    /**
     * 直接计算归一化拉普拉斯矩阵
     * 但是正交迭代法求的是绝对值最大特征值对应的特征向量，而需要的是最小特征值对应的特征向量
     * 因此将本来的"I-"换成了"I+"，此时特征向量不变，特征值变为原来的2-λ
     * @param {Array<Float32Array>} features 
     * @param {Function(Float32Array, Float32Array): number} func W(i, j) = func(features[i], features[j])
     * @returns {TriangleMatrix} D^(-1/2) * W * D^(-1/2) + I
     */
    static Lsym(features, func = TriangleMatrix.cosineAffinityExp) {
        const size = features.length;
        const affine = new TriangleMatrix(size);
        const rowSums = new Float32Array(size);
        const data = affine.data;
        // 不算自环，直接跳过第一个
        for (let j = 1, idx = 1; j < size; j++, idx++) {    // col
            const colFeature = features[j];
            for (let i = 0; i < j; i++, idx++) {            // row
                const affi = func(colFeature, features[i]);
                data[idx] = affi;
                rowSums[i] += affi;
                rowSums[j] += affi;
            }
        }
        // 此时对角线元素均为0
        // 归一化
        for (let j = 0, idx = 0; j < size; j++, idx++) {
            for (let i = 0; i < j; i++, idx++) {    // 本应是i <= j，但是对角线元素单独处理，所以将最后一个idx++放在外层
                const div = Math.sqrt(rowSums[i] * rowSums[j]);
                if (div > 1e-10) data[idx] = data[idx] / div;
                else data[idx] = 0;
            }
            // 对角线元素设为1
            data[idx] = 1;
        }
        return affine;
    }

    _index(i, j) {
        // 内联优化建议：在热路径中尽量手动计算，减少函数调用开销
        if (i > j) return (i * (i + 1)) / 2 + j;
        return (j * (j + 1)) / 2 + i;
    }

    /**
     * 优化的矩阵乘法 Z = A * Q
     * @param {Array<Float32Array>} Q_in 输入向量组 (k个)
     * @param {Array<Float32Array>} Z_out 输出向量组 (k个，预分配好)
     */
    mult_mat_optimized(Q_in, Z_out) {
        const size = this.size;
        const k = Q_in.length;
        const data = this.data;

        // 清空输出 buffer
        for (let r = 0; r < k; r++) Z_out[r].fill(0);

        for (let j = 0, idx = 0; j < size; j++) {
            for (let i = 0; i <= j; i++, idx++) {
                const val = data[idx];
                for (let r = 0; r < k; r++) {
                    const vecIn = Q_in[r];
                    const vecOut = Z_out[r];
                    vecOut[i] += val * vecIn[j];
                    if (i !== j) vecOut[j] += val * vecIn[i];
                }
            }
        }
    }

    /**
     * 正交迭代法 求前k个绝对值最大的特征值对应的特征向量
     * @param {TriangleMatrix} A 建议是 I + D^{-0.5}WD^{-0.5}
     * @param {number} numVectors 需要的特征向量数量
     * @param {number} numIterations 迭代次数
     * @return {Array<Float32Array>} 特征向量矩阵 size * numVectors
     */
    static orthogonalIteration(A, numVectors, numIterations = 30) {
        const size = A.size;

        // 双缓冲
        let Q = Array.from({ length: numVectors }, () => new Float32Array(size));
        let Z = Array.from({ length: numVectors }, () => new Float32Array(size));

        // 初始化 Q 为随机并正交化
        for (let r = 0; r < numVectors; r++) {
            for (let i = 0; i < size; i++) Q[r][i] = Math.random();
        }
        SchmidtInPlace(Q); // 原地正交化

        // 迭代
        for (let iter = 0; iter < numIterations; iter++) {
            // Z = A * Q (写入 Z buffer)
            A.mult_mat_optimized(Q, Z);
            // Q = Schmidt(Z) (原地正交化 Z，结果仍在 Z buffer 中)
            SchmidtInPlace(Z);
            // 交换 buffer：Z 变成了下一次的 Q，原来的 Q 变成下一次的废弃 buffer (Z)
            const temp = Q; Q = Z; Z = temp;
        }
        return Q;
    }
}

/**
 * 施密特正交化 (原地修改版 / In-Place MGS)
 * 没有任何内存分配，速度极快
 * @param {Array<Float32Array>} V 向量组
 */
function SchmidtInPlace(V) {
    const k = V.length;
    const n = V[0].length;

    for (let i = 0; i < k; i++) {
        const qi = V[i];

        // 归一化当前向量
        let dot = 0.0;
        for (let x = 0; x < n; x++) dot += qi[x] * qi[x];
        const norm = Math.sqrt(dot);
        const scale = norm < 1e-10 ? 0 : 1.0 / norm;
        for (let x = 0; x < n; x++) qi[x] *= scale;

        // 正交化后续向量 (MGS)
        for (let j = i + 1; j < k; j++) {
            const vj = V[j];

            // 计算投影 proj = <vj, qi>
            let proj = 0.0;
            for (let x = 0; x < n; x++) proj += vj[x] * qi[x];

            // 减去投影 vj = vj - proj * qi
            for (let x = 0; x < n; x++) vj[x] -= proj * qi[x];
        }
    }
}