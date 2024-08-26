/**
 * 复数乘法
 * @param {number} a 第一个数的实部
 * @param {number} b 第一个数的虚部
 * @param {number} c 第二个数的实部
 * @param {number} d 第二个数的虚部
 * @returns {Array} [实部, 虚部]
 */
function ComplexMul(a = 0, b = 0, c = 0, d = 0) {
    return [a * c - b * d, a * d + b * c];
}

/**
 * 计算复数的幅度
 * @param {Float32Array} r 实部数组
 * @param {Float32Array} i 虚部数组
 * @returns {Float32Array} 幅度
 */
function ComplexAbs(r, i) {
    const ABS = new Float32Array(r.length);
    for (let j = 0; j < r.length; j++) {
        ABS[j] = Math.sqrt(r[j] * r[j] + i[j] * i[j]);
    } return ABS;
}

/**
 * 
 * @param {number} N FFT点数
 * @returns {TypedArray} 位反转结果
 */
function reverseBits(N) {
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