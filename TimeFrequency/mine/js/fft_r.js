/**
 * @fileOverview FFT
 * @description 基2的FFT实现 最大支持4096点 预存了一些数据，且基于递归，适用于动态长度的FFT
 */

// fft常量 最大支持4096
const _Wr = new Float32Array(Array.from({ length: 2048 }, (_, i) => Math.cos(Math.PI / 2048 * i)));
const _Wi = new Float32Array(Array.from({ length: 2048 }, (_, i) => -Math.sin(Math.PI / 2048 * i)));

/**
 * 通用FFT
 * @param {Float32Array} xr 实部
 * @param {Float32Array} xi 虚部
 * @param {Number} N 2的幂 如果小于数组长度则
 * @returns {Array} [实部Float32Array, 虚部Float32Array]
 */
function fft(xr, xi = null, N = 3) {
    let log2N = Math.log2(N);
    if (N && log2N % 1 != 0) {   // 不是2的幂 则自动选取为数组长度以上的2的幂
        log2N = Math.ceil(Math.log2(xr.length));
        N = Math.pow(2, log2N);
    }
    // 补0到长度为N
    var tempr = null;
    var tempi = null;
    if (xr.length <= N) {
        tempr = new Float32Array(N);
        tempi = new Float32Array(N);
        tempr.set(xr);
        if (xi) tempi.set(xi);
    } else {
        tempr = xr.slice(0, N);
        if (xi) tempi = xi.slice(0, N);
        else tempi = new Float32Array(N);
    }
    // 频域结果
    var Xr = new Float32Array(N);
    var Xi = new Float32Array(N);
    // 需要确保到最后一级时temp里面存的是输入
    if (log2N % 2 == 0) [Xr, Xi, tempr, tempi] = [tempr, tempi, Xr, Xi];
    _fft(tempr, tempi, Xr, Xi, 0, 1, N);
    return [Xr, Xi];
}

/**
 * 递归计算FFT
 * @param {Float32Array} inputr 输入的实部
 * @param {Float32Array} inputi 输入的虚部
 * @param {Float32Array} outputr 输出的实部
 * @param {Float32Array} outputi 输出的虚部
 * @param {Number} offset 第一个元素的偏移量
 * @param {Number} step 每个元素的间隔
 * @param {Number} N FFT points
 */
function _fft(inputr, inputi, outputr, outputi, offset, step, N) {
    if (N == 2) {   // 直接从输入获取值 由于是递归，能保证数据不被覆盖
        let k = offset + step;
        outputr[offset] = inputr[offset] + inputr[k];
        outputi[offset] = inputi[offset] + inputi[k];
        outputr[k] = inputr[offset] - inputr[k];
        outputi[k] = inputi[offset] - inputi[k];
        return;
    }
    let step2 = step << 1;
    N = N >> 1;
    let offset1 = offset + step;
    _fft(outputr, outputi, inputr, inputi, offset, step2, N);
    _fft(outputr, outputi, inputr, inputi, offset1, step2, N);
    // di是两个分支的间隔
    for (let i = 0, n = 0, di = N * step; n < N; i += step2, n++) {
        let k = 2048 / N * n;   // 换算为4096为底
        let [rp, ip] = ComplexMul(inputr[offset1 + i], inputi[offset1 + i], _Wr[k], _Wi[k]);
        k = n * step + offset;
        outputr[k] = inputr[offset + i] + rp;
        outputi[k] = inputi[offset + i] + ip;
        k += di;
        outputr[k] = inputr[offset + i] - rp;
        outputi[k] = inputi[offset + i] - ip;
    }
}

/**
 * 实数的FFT
 * @param {Float32Array} input 实数信号
 * @param {Number} N 做几点的FFT
 * @returns {Array} 注意，只有N/2的长度
 */
function fftR(input, N = 3) {
    let log2N = Math.log2(N);
    if (N && log2N % 1 != 0) {
        log2N = Math.ceil(Math.log2(input.length));
        N = Math.pow(2, log2N);
    }
    N >>= 1; // 转换为N/2点的FFT

    // 提取偶数项和奇数项
    let tempr = new Float32Array(N);
    let tempi = new Float32Array(N);
    for (let n = 0, l = Math.min(input.length, 2 * N), i = 0; n < l; n += 2, i++) {
        tempr[i] = input[n];
        tempi[i] = input[n + 1] || 0; // 防越界
    }
    let Xr = new Float32Array(N);
    let Xi = new Float32Array(N);
    if (log2N % 2 == 1) [Xr, Xi, tempr, tempi] = [tempr, tempi, Xr, Xi];

    _fft(tempr, tempi, Xr, Xi, 0, 1, N);
    // 组装成结果
    for (let k = 0; k < N; k++) {
        let K = 2048 / N * k;   // 换算为4096为底
        let [Ir, Ii] = ComplexMul(Xi[k] + (Xi[N - k] || Xi[k]), (Xr[N - k] || Xr[k]) - Xr[k], _Wr[K], _Wi[K]);
        tempr[k] = (Xr[k] + (Xr[N - k] || Xr[k]) + Ir) * 0.5;
        tempi[k] = (Xi[k] - (Xi[N - k] || Xi[k]) + Ii) * 0.5;
    }
    return [tempr, tempi];
}