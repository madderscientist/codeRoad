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
     * @param {number} hop 一帧的长度
     * @param {number} fmin 最低的频率(最低八度的C) 默认为C1
     * @param {number} octaves 要分析几个八度
     * @param {number} bins_per_octave 几平均律
     * @param {number} filter_scale Q的缩放倍数，默认3以达到较好的频率选择性
     */
    constructor(fs, hop, fmin = 32.7, octaves = 7, bins_per_octave = 12, filter_scale = 2.5) {
        const Q = filter_scale / (Math.pow(2, 1 / bins_per_octave) - 1);
        [this.kernel_r, this.kernel_i] = CQT.iniKernel(
            Q, fs, hop * filter_scale, fmin, bins_per_octave, bins_per_octave * octaves
        );
        this.hop = hop;
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
    static iniKernel(Q, fs, hop, fmin, bins_per_octave = 12, binNum = 84) {
        const kernel_r = Array(binNum);
        const kernel_i = Array(binNum);
        for (let i = 0; i < binNum; i++) {
            const freq = fmin * Math.pow(2, i / bins_per_octave);
            const len = Math.max(hop, Math.ceil(Q * fs / freq));
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
     * @param {Float32Array} x 输入实数时序信号
     * @returns {Array<Float32Array>} 第一维是频率，第二维是时间
     */
    *cqt(x) {
        const stride = this.hop;
        const bins = this.bins_per_octave * this.octaves;
        let offset = stride >> 1;
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
        } return output;
    }
}
/*
analyse_CQT: async (audioBuffer, tNum = 20, A4 = 440, channel = -1) => {
    this.dt = 1000 / tNum;
    this.TperP = this.dt / this._width; this.PperT = this._width / this.dt;
    let dN = Math.round(audioBuffer.sampleRate / tNum);
    if (this.Keyboard.freqTable.A4 != A4) this.Keyboard.freqTable.A4 = A4;   // 更新频率表
    let progressTrans = (x) => x;

    // 创建分析工具
    var cqt = new CQT(audioBuffer.sampleRate, dN, this.Keyboard.freqTable[0], 7, 12);

    const a = async (t) => { // 对t执行CQT，并整理为时频谱
        const cqtProcess = cqt.cqt(t);
        let result = cqtProcess.next();
        while (!result.done) {
            let tnow = performance.now();
            if (tnow - lastFrame > 250) {
                lastFrame = tnow;
                // 打断分析 更新UI 等待下一周期
                this.event.dispatchEvent(new CustomEvent("progress", {
                    detail: progressTrans(result.value)
                }));
                await new Promise(resolve => setTimeout(resolve, 0));
            } result = cqtProcess.next();
        }
        return result.value
    }

    await new Promise(resolve => setTimeout(resolve, 0));   // 等待UI
    var lastFrame = performance.now();
    switch (channel) {
        case 0: return await a(audioBuffer.getChannelData(0));
        case 1: return await a(audioBuffer.getChannelData(audioBuffer.numberOfChannels - 1));
        case 2: {   // L+R
            let length = audioBuffer.length;
            const timeDomain = new Float32Array(audioBuffer.getChannelData(0));
            if (audioBuffer.numberOfChannels > 1) {
                let channelData = audioBuffer.getChannelData(1);
                for (let i = 0; i < length; i++) timeDomain[i] = (timeDomain[i] + channelData[i]) * 0.5;
            } return await a(timeDomain);
        }
        case 3: {   // L-R
            let length = audioBuffer.length;
            const timeDomain = new Float32Array(audioBuffer.getChannelData(0));
            if (audioBuffer.numberOfChannels > 1) {
                let channelData = audioBuffer.getChannelData(1);
                for (let i = 0; i < length; i++) timeDomain[i] = (timeDomain[i] - channelData[i]) * 0.5;
            } return await a(timeDomain);
        }
        default: {  // fft(L) + fft(R)
            if (audioBuffer.numberOfChannels > 1) {
                progressTrans = (x) => x / 2;
                const l = await a(audioBuffer.getChannelData(0));
                progressTrans = (x) => 0.5 + x / 2;
                const r = await a(audioBuffer.getChannelData(1));
                for (let i = 0; i < l.length; i++) {
                    const li = l[i];
                    for (let j = 0; j < li.length; j++)
                        li[j] = (li[j] + r[i][j]) * 0.5;
                } return l;
            } else {
                progressTrans = (x) => x;
                return await a(audioBuffer.getChannelData(0));
            }
        }
    }
},
*/