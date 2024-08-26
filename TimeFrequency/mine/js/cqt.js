/**
 * 用定义计算CQT，时间复杂度很高，但是分析效果好
 */
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

    /**
     * 预计算CQT参数
     * @param {number} fs 采样率
     * @param {number} fmin 最低的频率(最低八度的C) 默认为C1
     * @param {number} octaves 要分析几个八度
     * @param {number} bins_per_octave 几平均律
     * @param {number} filter_scale Q的缩放倍数，默认3以达到较好的频率选择性
     */
    constructor(fs, fmin = 32.7, octaves = 7, bins_per_octave = 12, filter_scale = 3) {
        const Q = filter_scale / (Math.pow(2, 1 / bins_per_octave) - 1);
        [this.kernel_r, this.kernel_i] = CQT.iniKernel(
            Q, fs, fmin, bins_per_octave, bins_per_octave * octaves
        );
        this.octaves = octaves;
        this.bins_per_octave = bins_per_octave;
    }
    /**
     * 得到CQT kernel 时域数据
     * @param {number} Q 
     * @param {number} fs 采样率
     * @param {number} fmin 最低音
     * @param {number} bins_per_octave 八度内的频率个数
     * @param {number} binNum 一共多少个频率
     * @returns {Array<Float32Array>} [kernel_r, kernel_i]
     */
    static iniKernel(Q, fs, fmin, bins_per_octave = 12, binNum = 84) {
        const kernel_r = Array(binNum);
        const kernel_i = Array(binNum);
        for (let i = 0; i < binNum; i++) {
            const freq = fmin * Math.pow(2, i / bins_per_octave);
            const len = Math.ceil(Q * fs / freq);
            const temp_kernel_r = kernel_r[i] = new Float32Array(len);
            const temp_kernel_i = kernel_i[i] = new Float32Array(len);
            const window = CQT.blackmanHarris(len);
            const omega = 2 * Math.PI * freq / fs;
            const half_len = len >> 1;
            for (let j = 0; j < len; j++) {
                const angle = omega * (j - half_len);   // 中心的相位为0
                temp_kernel_r[j] = Math.cos(angle) * window[j];
                temp_kernel_i[j] = Math.sin(angle) * window[j]; // 按DFT应该加负号，但是最后的结果是能量，加不加都一样
                // 而且CQT1992继承自本类，用正相位增加的旋转因子可以让频域带宽在正频率上
            }
        } return [kernel_r, kernel_i];
    }
    /**
     * 计算CQT
     * 是生成器函数，每次yield一个频率
     * @param {Float32Array} x 输入实数时序信号 会被改变！
     * @param {number} stride 
     * @returns {Array<Float32Array>} 第一维是频率，第二维是时间
     */
    *cqt(x, stride) {
        const bins = this.bins_per_octave * this.octaves;
        // 以每一帧的中心为基准。同时要是2的倍数，以便在降采样的时候对齐(虽然现在没有降采样了)
        let offset = Math.pow(2, Math.log2(stride >> 1) | 0);
        const output = Array(Math.ceil((x.length - offset) / stride));
        let pointer = 0;
        for (let center = offset; center < x.length; center += stride) {
            const energy = output[pointer++] = new Float32Array(bins);
            for (let b = 0; b < bins; b++) {    // 每个频率
                const kernel_r = this.kernel_r[b];
                const kernel_i = this.kernel_i[b];
                let real = 0, imag = 0;
                const left = center - (kernel_r.length >> 1);
                const right = Math.min(kernel_r.length, x.length - left);
                for (let i = left >= 0 ? 0 : -left; i < right; i++) {
                    const index = left + i;
                    if (index >= x.length) break;
                    real += x[index] * kernel_r[i];
                    imag += x[index] * kernel_i[i];
                }
                energy[b] = Math.sqrt(real * real + imag * imag) * 32;
            } yield pointer / output.length;
        }
        return output;
    }
}

/**
 * 用论文《An efficient algorithm for the calculation of a constant Q transform》中的方法计算CQT
 * 利用了频域的稀疏性
 * 同时融合了时域方法，因为频域方法在高频时计算量过大
 * 依赖fft_i.js，计算复数核的FFT；依赖fft_real.js，计算音频的FFT
 */
class CQThybrid extends CQT {
    /**
     * 
     * @param {number} hop 
     * @param {number} fs 
     * @param {number} fmin 
     * @param {number} octaves 
     * @param {number} bins_per_octave 
     * @param {number} filter_scale 最大2.88，因为FFT最大只支持16点的
     */
    constructor(hop, fs, fmin = 32.7, octaves = 7, bins_per_octave = 12, filter_scale = 2.88) {
        super(fs, fmin, octaves, bins_per_octave, filter_scale);
        this.hop = hop;
        this.timeOffset = hop >> 1; // 指的是第一个时域窗的中心
        // 准备频域方法
        this.hopKernel();
        this.FFT = new realFFT(this.fftSize);   // 用于计算STFT
        /*
        this.hopNum: 一次FFT可以算几次时间平移
        this.freqStride: STFT的分析时间步长
        this.timeOffset: 第一个时域窗的中心
        this.fftSize: 需要的FFT大小
        this.freqOffset: STFT起点
        this.frequencyKernel: 频域核
        this.kernel_r: 时域核的实部
        this.kernel_i: 时域核的虚部
        this.FFT: realFFT类，用于计算STFT
         */
    }
    hopKernel(threshold = 5e-4) {   // 在65536点时观察到的阈值
        const binNum = this.kernel_r.length;
        const maxWindow = this.kernel_r[0].length;
        // 比最大长度还长的2的幂
        const fftSize = this.fftSize = Math.pow(2, Math.ceil(Math.log2(maxWindow)));
        if(fftSize > 65536) throw new Error('FFT size is too large');   // FFT只支持16位的
        const hopNum = this.hopNum = (((fftSize - maxWindow) / this.hop) | 0) + 1;    // 在一个fft窗口内可以放几个kernel
        // 时域的平移等于频域乘一个因子，预计算
        const rot_r = new Float32Array(fftSize);
        const rot_i = new Float32Array(fftSize);
        const rotAngle = 2 * Math.PI * hopNum / fftSize;
        for(let i = 0; i < fftSize; i++) {
            rot_r[i] = Math.cos(rotAngle * i);
            rot_i[i] = Math.sin(rotAngle * i);  // 和CQT类保持一致，不加负号
        }
        // 第一维是频率，第二维是时间，第三维是实部和虚部
        const frequencyKernel = Array(binNum);
        const fft = new FFT(fftSize);
        for (let note = 0; note < binNum; note++) {
            // 中心对齐最长的窗，最长的窗靠最左边
            const timeLen = this.kernel_r[note].length;
            const [real, imag] = fft.fft(
                this.kernel_r[note], this.kernel_i[note],
                -((maxWindow - timeLen) >> 1)
            );  // real和imag都是引用的
            // 计算实际长度
            let begin = 1;
            let energy = real[begin] * real[begin] + imag[begin] * imag[begin];
            while(energy < threshold) {
                begin++;
                energy = real[begin] * real[begin] + imag[begin] * imag[begin];
            }
            let end = begin + 1;
            energy = real[end] * real[end] + imag[end] * imag[end];
            while(energy >= threshold) {
                end++;
                energy = real[end] * real[end] + imag[end] * imag[end];
            }
            // 选择时域和频域 时域方法是实数和复数相乘，只有频域方法的一半计算量
            const freqLen = end - begin;
            if(freqLen > timeLen >> 1) {    // 后面的频点在时域上计算量更少
                frequencyKernel.length = note;
                break;
            }
            // 创建滤波器组
            const thisBin = frequencyKernel[note] = Array(hopNum);
            thisBin.begin = begin;
            let firstKernel_r = real.slice(begin, end);
            let firstKernel_i = imag.slice(begin, end);
            thisBin[0] = [firstKernel_r, firstKernel_i];
            for(let i = 1; i < hopNum; i++) {
                const kernel_r = new Float32Array(freqLen);
                const kernel_i = new Float32Array(freqLen);
                for(let j = 0; j < freqLen; j++) {
                    kernel_r[j] = firstKernel_r[j] * rot_r[begin + j] - firstKernel_i[j] * rot_i[begin + j];
                    kernel_i[j] = firstKernel_r[j] * rot_i[begin + j] + firstKernel_i[j] * rot_r[begin + j];
                }
                thisBin[i] = [kernel_r, kernel_i];
                // 下一个在上一个的基础上平移
                firstKernel_r = kernel_r;
                firstKernel_i = kernel_i;
            }
        } this.frequencyKernel = frequencyKernel;
        this.freqOffset = (this.hop - maxWindow) >> 1;  // 保证第一个时域窗的中心在第一帧的中心
        this.freqStride = this.hop * hopNum;
    }
    *cqt(x) {
        const stride = this.hop;
        const output = Array(Math.ceil((x.length - this.timeOffset) / stride));
        // 时域方法的参数
        const time_r = this.kernel_r;
        const time_i = this.kernel_i;
        let timeCenter = this.timeOffset;
        // 频域方法的参数
        const fk = this.frequencyKernel;
        let turn = 0;
        let offset_freq = this.freqOffset;
        let [stft_r, stft_i] = this.FFT.fft(x, this.freqOffset);
        for (let t = 0; t < output.length; t++) {
            const energy = output[t] = new Float32Array(time_r.length);
            // 频域方法
            let note = 0;
            for (; note < fk.length; note++) {
                const [kernel_r, kernel_i] = fk[note][turn];
                const begin = fk[note].begin;
                let real = 0, imag = 0;
                for (let j = 0; j < kernel_r.length; j++) {
                    real += stft_r[begin + j] * kernel_r[j] - stft_i[begin + j] * kernel_i[j];
                    imag += stft_r[begin + j] * kernel_i[j] + stft_i[begin + j] * kernel_r[j];
                }
                energy[note] = Math.sqrt(real * real + imag * imag) * 32 / this.fftSize;
            }
            if(++turn == this.hopNum) {
                turn = 0;
                [stft_r, stft_i] = this.FFT.fft(x, offset_freq += this.freqStride);
            }
            // 时域方法
            for (; note < time_r.length; note++) {
                const kernel_r = time_r[note];
                const kernel_i = time_i[note];
                let real = 0, imag = 0;
                const left = timeCenter - (kernel_r.length >> 1);
                const right = Math.min(kernel_r.length, x.length - left);
                for (let i = left >= 0 ? 0 : -left; i < right; i++) {
                    const index = left + i;
                    if (index >= x.length) break;
                    real += x[index] * kernel_r[i];
                    imag += x[index] * kernel_i[i];
                }
                energy[note] = Math.sqrt(real * real + imag * imag) * 32;
            } timeCenter += stride;
            yield t / output.length;
        }
        return output;
    }
}