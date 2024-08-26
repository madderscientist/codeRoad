/**
 * 基于迭代的FFT。参数可自定义 在取8192点时比递归快
 * @param {{Uint16Array, Float32Array, Float32Array}} param0 reverseBits决定FFT点数，Wr和Wi决定旋转因子
 * @param {{Float32Array, number, number}} param1 实部数据，偏移量，步长
 * @param {{Float32Array, number, number}} param2 虚部数据，偏移量，步长
 * @param {[Float32Array, Float32Array, Float32Array, Float32Array]} param3 输出与缓冲的四个数组。其中，最后两个最终会存有FFT结果
 * @returns {[Float32Array, Float32Array]} [实部, 虚部]
 */
function fft_flex(
    { reverseBits = new Uint16Array(), Wr = new Float32Array(), Wi = new Float32Array() },
    { data: xr = new Float32Array(), offset: or = 0, step: sr = 1 },
    { data: xi = new Float32Array(), offset: oi = 0, step: si = 1 },
    [ Xr, Xi, bufferr, bufferi]
) {
    let N = reverseBits.length;
    let Nw = (Wr.length << 1) / N;  // 必须要大于等于1
    Xr = Xr || new Float32Array(N);
    Xi = Xi || new Float32Array(N);
    bufferr = bufferr || new Float32Array(N);
    bufferi = bufferi || new Float32Array(N);
    for (let i = 0, ii = 1; i < N; i += 2, ii += 2) {
        let k1r = reverseBits[i] * sr + or;
        let k2r = reverseBits[ii] * sr + or;
        let k1i = reverseBits[i] * si + oi;
        let k2i = reverseBits[ii] * si + oi;
        let xrk1r = xr[k1r] || 0; let xrk2r = xr[k2r] || 0;
        let xik1i = xi[k1i] || 0; let xik2i = xi[k2i] || 0;
        bufferr[i] = xrk1r + xrk2r;
        bufferr[ii] = xrk1r - xrk2r;
        bufferi[i] = xik1i + xik2i;
        bufferi[ii] = xik1i - xik2i;
    }
    for (let groupNum = N >> 2, groupMem = 2; groupNum; groupNum >>= 1, groupMem <<= 1) {
        for (let mem = 0, groupOffset = groupMem << 1, tempk = groupNum * Nw, k = 0; mem < groupMem; mem++, k+=tempk) {
            let [_Wr, _Wi] = [Wr[k], Wi[k]];
            for (let gn = mem; gn < N; gn += groupOffset) {
                let gn2 = gn + groupMem;
                let [gwr, gwi] = ComplexMul(bufferr[gn2], bufferi[gn2], _Wr, _Wi);
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

function iniRealParam(N) {
    // 实数FFT只需要一半
    N = Math.pow(2, Math.ceil(Math.log2(N))-1);
    return {
        buffer: [new Float32Array(N), new Float32Array(N), new Float32Array(N), new Float32Array(N)],
        option: {
            reverseBits: reverseBits(N),
            Wr: new Float32Array(Array.from({ length: N }, (_, i) => Math.cos(Math.PI / N * i))),
            Wi: new Float32Array(Array.from({ length: N }, (_, i) => -Math.sin(Math.PI / N * i)))
        },
        N: N,
        fft: function(d, from = 0) {
            let t = fft_flex(
                this.option, {
                    data: d,
                    offset: from,
                    step: 2
                }, {
                    data: d,
                    offset: 1 + from,
                    step: 2
                }, this.buffer
            );
            const [tempr, tempi, Xr, Xi] = this.buffer;
            const {Wr, Wi} = this.option;
            const N = this.N;
            for (let k = 0; k < N; k++) {
                let [Ir, Ii] = ComplexMul(Xi[k] + (Xi[N - k] || Xi[k]), (Xr[N - k] || Xr[k]) - Xr[k], Wr[k], Wi[k]);
                tempr[k] = (Xr[k] + (Xr[N - k] || Xr[k]) + Ir) * 0.5;
                tempi[k] = (Xi[k] - (Xi[N - k] || Xi[k]) + Ii) * 0.5;
            }
            return ComplexAbs(tempr, tempi);
        }
    }
}