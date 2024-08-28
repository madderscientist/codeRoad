// 基于降采样的CQT

function filter(input, num, den, output = undefined, reverse = false) {
    const len = input.length;
    const order = Math.max(num.length, den.length);
    if(!output) output = new Float32Array(len);
    if(reverse) {
        for (let i = len - 1; i >= 0; i--) {
            let y = num[0] * input[i];
            for (let j = 1; j < order; j++) {
                if (i + j < len) {
                    y += num[j] * input[i + j];
                    y -= den[j] * output[i + j];
                } else break;
            }
            output[i] = y;
        }
    } else {
        for (let i = 0; i < len; i++) {
            let y = num[0] * input[i];
            for (let j = 1; j < order; j++) {
                if (i - j >= 0) {
                    y += num[j] * input[i - j];
                    y -= den[j] * output[i - j];
                } else break;
            }
            output[i] = y;
        }
    } return output;
}

function filtfilt_change(input, num, den) {
    const out = filter(input, num, den, undefined, false);
    return filter(out, num, den, input, true);
}

class CQT {
    /**
     * 创建窗函数 幅度加起来为1
     * @param {number} N 
     * @returns {Float32Array}
     */
    static blackmanHarris(N) {
        let w = new Float32Array(N);
        const temp = 2 * Math.PI / (N - 1);
        let sum = 0;
        for (let n = 0; n < N; n++) {
            w[n] = 0.35875
                - 0.48829 * Math.cos(temp * n)
                + 0.14128 * Math.cos(temp * n * 2)
                - 0.01168 * Math.cos(temp * n * 3);
            sum += w[n];
        }
        // 归一化（幅度归一化，和矩形窗FFT除以N的效果一样）
        for (let n = 0; n < N; n++) w[n] /= sum;
        return w;
    }
    // butter(6, 0.5, 'low')
    static lowpass_num = [0.0295882236386608,0.177529341831965,0.443823354579911,0.591764472773215,0.443823354579911,0.177529341831965,0.0295882236386608];
    static lowpass_den = [1,0,0.777695961855673,0,0.114199425062434,0,0.00175092595618284];

    constructor(fs, fmin = 32.7, octaves = 7, bins_per_octave = 12) {
        const Q = 2 / (Math.pow(2, 0.5 / bins_per_octave) - Math.pow(2, -0.5 / bins_per_octave));
        [this.kernel_r, this.kernel_i] = CQT.iniKernel(Q, fs, fmin * Math.pow(2, octaves - 1), bins_per_octave);
        this.octaves = octaves;
        this.bins_per_octave = bins_per_octave;
    }
    /**
     * 计算CQT kernel 时域数据
     * @param {number} Q 
     * @param {number} fs 采样率
     * @param {number} fmin 最低音
     * @param {number} bins_per_octave 八度内的频率个数
     * @returns {Array<Float32Array>} [kernel_r, kernel_i]
     */
    static iniKernel(Q, fs, fmin, bins_per_octave = 12) {
        const kernel_r = Array(bins_per_octave);
        const kernel_i = Array(bins_per_octave);
        for(let i = 0; i < bins_per_octave; i++) {
            const freq = fmin * Math.pow(2, i / bins_per_octave);
            const len = Math.ceil(Q * fs / freq);
            const temp_kernel_r = new Float32Array(len);
            const temp_kernel_i = new Float32Array(len);
            const window = CQT.blackmanHarris(len);
            const omega = 2 * Math.PI * freq / fs;
            const half_len = len >> 1;
            for(let j = 0; j < len; j++) {
                const angle = omega * (j - half_len);   // 中心的相位为0
                temp_kernel_r[j] = Math.cos(angle) * window[j];
                temp_kernel_i[j] = Math.sin(angle) * window[j]; // 按DFT应该加负号，但是最后的结果是能量，加不加都一样
            }
            kernel_r[i] = temp_kernel_r;
            kernel_i[i] = temp_kernel_i;
        }
        return [kernel_r, kernel_i];
    }
    /**
     * 计算CQT
     * 是生成器函数，每次yield一个频率
     * @param {Float32Array} x 输入实数时序信号 会被改变！
     * @param {number} stride 
     * @returns {Array<Float32Array>} 第一维是频率，第二维是时间
     */
    *cqt(x, stride) {
        // 以每一帧的中心为基准。同时要是2的倍数，以便在降采样的时候对齐
        let offset = Math.pow(2, Math.round(Math.log2(stride >> 1)));
        const output = Array(this.octaves * this.bins_per_octave);
        let pointer = output.length - 1;
        for(let o = 0; o < this.octaves; o++) { // 每个八度
            for(let b = this.bins_per_octave - 1; b >= 0; b--) {    // 每个频率
                const kernel_r = this.kernel_r[b];
                const kernel_i = this.kernel_i[b];
                // 相关
                const energy = new Float32Array(Math.ceil((x.length - offset) / stride));
                let p = 0;
                for(let center = offset; center < x.length; center += stride) {
                    let real = 0, imag = 0;
                    const left = center - (kernel_r.length >> 1);
                    const right = Math.min(kernel_r.length, x.length - left);
                    for(let i = left >= 0 ? 0 : -left; i < right; i++) {
                        const index = left + i;
                        if(index >= x.length) break;
                        real += x[index] * kernel_r[i];
                        imag += x[index] * kernel_i[i];
                    }
                    energy[p++] = Math.sqrt(real * real + imag * imag) * 64;
                }
                output[pointer--] = energy;
                yield output.length - pointer - 1;  // 完成了几个音
            }
            // 降采样 改变x的数据
            filtfilt_change(x, CQT.lowpass_num, CQT.lowpass_den);
            for(let i = 1, ii = 2; ii < x.length; i++, ii += 2) x[i] = x[ii];
            x = x.subarray(0, x.length >> 1);   // 共享内存
            // 偏移量改变 但stride不变
            offset >>= 1;
        } return output;
    }

    align(cqt) {
        // 时间为第一维
        const output = Array.from(cqt[cqt.length - 1], () => new Float32Array(cqt.length));
        for(let o = 0; o < this.octaves; o++) {
            let interpolation = Math.pow(2, this.octaves - o - 1) | 0;
            let base = 0;
            for(let t = 0, turn = 0; t < output.length; t++) {
                let coe2 = turn / interpolation;
                let coe1 = 1 - coe2;
                // 一次插一个八度的值
                for(let b = 0, begin = this.bins_per_octave * o; b < this.bins_per_octave; b++, begin++) {
                    let y = cqt[begin][base] * coe1;
                    if(base + 1 < cqt[begin].length) y += cqt[begin][base + 1] * coe2;
                    output[t][begin] = y;
                }
                if(++turn == interpolation) {
                    turn = 0;
                    base++;
                }
            }
        } return output;
    }
}