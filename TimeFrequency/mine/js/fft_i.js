/**
 * 为密集FFT计算设计的类，不支持动态修改FFT长度，使用内存预分配，参数预计算
 * 计算速度比递归快
 */
class FFT {
    /**
     * 位反转数组 最大支持2^16点
     * @param {Number} N 2的正整数幂
     * @returns {[Uint16Array, Uint8Array]} 根据N的大小决定的位反转结果
     */
    static reverseBits(N) {
        const reverseBits = N <= 256 ? new Uint8Array(N) : new Uint16Array(N);
        let id = 0;
        function _fft(offset, step, N) {
            if (N == 2) {
                reverseBits[id++] = offset;
                reverseBits[id++] = offset + step;
                return;
            }
            let step2 = step << 1;
            N >>= 1;
            _fft(offset, step2, N);
            _fft(offset + step, step2, N);
        }
        _fft(0, 1, N);
        return reverseBits;
    }
    /**
     * 复数乘法
     * @param {Number} a 第一个数的实部
     * @param {Number} b 第一个数的虚部
     * @param {Number} c 第二个数的实部
     * @param {Number} d 第二个数的虚部
     * @returns {Array} [实部, 虚部]
     */
    static ComplexMul(a = 0, b = 0, c = 0, d = 0) {
        return [a * c - b * d, a * d + b * c];
    }

    constructor(N = 4096) {
        this.ini(N);
        // 要用到的内存全部预分配
        this.bufferr = new Float32Array(this.N);
        this.bufferi = new Float32Array(this.N);
        this.Xr = new Float32Array(this.N);
        this.Xi = new Float32Array(this.N);
    }
    /**
     * 预计算常量
     * @param {Number} N 2的正整数次幂
     */
    ini(N) {
        // 确定FFT长度
        let log2N = Math.log2(N);
        if (N && log2N % 1 != 0) {   // 不是2的幂 则自动选取为数组长度以上的2的幂
            log2N = Math.ceil(log2N);
            N = Math.pow(2, log2N);
        }
        this.N = N;
        // 位反转预计算
        this.reverseBits = FFT.reverseBits(N);
        // 旋转因子预计算
        N = N >> 1; // 只用得上一半
        this._Wr = new Float32Array(Array.from({ length: N }, (_, i) =>  Math.cos(Math.PI / N * i)));
        this._Wi = new Float32Array(Array.from({ length: N }, (_, i) => -Math.sin(Math.PI / N * i)));
    }

    /**
     * 基于位反转的非递归FFT
     * @param {Float32Array} xr 时域实部
     * @param {Float32Array} xi 时域虚部
     * @param {Number} offset 从哪个位置开始计算
     * @returns {Array} [实部Float32Array, 虚部Float32Array]
     */
    fft(xr, xi, offset = 0) {
        let Xr = this.Xr, Xi = this.Xi;
        let bufferr = this.bufferr, bufferi = this.bufferi;
        // 处理第一层
        for (let i = 0; i < this.N; i += 2) {
            let k1 = this.reverseBits[i] + offset;
            let k2 = this.reverseBits[i + 1] + offset;
            let xrr1 = xr[k1] || 0; let xrr2 = xr[k2] || 0;
            let xii1 = xi[k1] || 0; let xii2 = xi[k2] || 0;
            bufferr[i] = xrr1 + xrr2;
            bufferi[i] = xii1 + xii2;
            bufferr[i + 1] = xrr1 - xrr2;
            bufferi[i + 1] = xii1 - xii2;
        }
        // 处理后面 log2(N)-1 层 每次从buffer到X
        for (let groupNum = this.N >> 2, groupMem = 2; groupNum; groupNum >>= 1, groupMem <<= 1) {
            // groupNum: 组数；groupMem：一组里有几个蝶形结构，同时也是一个蝶形结构两个元素的序号差值
            // groupNum: N/4,  N/8, ...,    1
            // groupMem: 2,    4,   ...,    N/2
            // W's base: 4,    8,   ...,    N
            // offset between groups: 4, 8, ..., N
            let groupOffset = groupMem<<1;
            for (let mem = 0, k = 0; mem < groupMem; mem++, k+=groupNum) {     // 对每种蝶形结构
                // let k = mem * groupNum; // 归一化以this.N为底
                let[Wr, Wi] = [this._Wr[k], this._Wi[k]];
                for(let gn = mem; gn<this.N; gn+=groupOffset) {
                    let gn2 = gn + groupMem;
                    let [gwr, gwi] = FFT.ComplexMul(bufferr[gn2], bufferi[gn2], Wr, Wi);
                    Xr[gn] = bufferr[gn] + gwr;
                    Xi[gn] = bufferi[gn] + gwi;
                    Xr[gn2] = bufferr[gn] - gwr;
                    Xi[gn2] = bufferi[gn] - gwi;
                }
            }
            [bufferr, bufferi, Xr, Xi] = [Xr, Xi, bufferr, bufferi];
        }
        return [bufferr, bufferi];
    }
}