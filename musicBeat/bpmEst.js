class SIGNAL {
    /**
     * 峰值检测
     * @param {Float32Array} arr 输入数据
     * @param {Number} prominence 显著性阈值
     * @returns {Array} 找到的峰值索引数组 从小到大
     */
    static findPeaks(arr, prominence = 0) {
        const len = arr.length;
        const outBuffer = [];
        for (let i = 1; i < len - 1; i++) {
            const current = arr[i];
            if (current <= arr[i - 1] || current <= arr[i + 1]) continue;
            // 查找左侧基准
            let l = i - 1;
            while (l > 0 && arr[l - 1] <= arr[l]) l--;
            // 查找右侧基准
            let r = i + 1;
            while (r < len - 1 && arr[r + 1] <= arr[r]) r++;
            // 计算显著性
            const leftMin = arr[l];
            const rightMin = arr[r];
            const maxBase = leftMin > rightMin ? leftMin : rightMin;
            if (current - maxBase >= prominence) outBuffer.push(i);
        } return outBuffer;
    }

    /**
     * 抛物线插值，返回相对于x2的偏移量
     * @param {Number} y1 
     * @param {Number} y2 极值点
     * @param {Number} y3 
     * @returns {Array<Number>} [峰值相对于x2的偏移量, 插值后的y值]
     */
    static parabolicInterpolation(y1, y2, y3) {
        const a = y1 + y3 - 2 * y2;
        const b = y1 - y3;
        if (a === 0) return [0, y2];
        const dx = b / (2 * a);
        const y = y2 - b * dx * 0.25;
        return [dx, y];
    }

    /**
     * IIR滤波器实现（直接型II结构，支持任意阶数）
     * @param {Float32Array} arr 输入信号
     * @param {Array} b 分子系数（b[0], b[1], ..., b[M]）
     * @param {Array} a 分母系数（a[0], a[1], ..., a[N]），a[0]通常为1，会自动归一化
     * @param {Boolean} inplace 是否就地滤波（修改输入数组）
     * @param {Boolean} reverse 是否反向滤波（用于filtfilt）
     * @returns {Float32Array} 滤波后信号
     * @example 二阶高通滤波器
     * 差分方程：
     *   y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
     * 对应传参：
     *   b = [b0, b1, b2]
     *   a = [1, a1, a2]
     */
    static filter(arr, b, a, inplace = false, reverse = false) {
        const order = Math.max(b.length, a.length);
        const result = inplace ? arr : (new Float32Array(arr.length));
        const xHist = new Float32Array(order);
        const yHist = new Float32Array(order);
        let xPtr = 0, yPtr = 0;
        const processSample = (n) => {
            xHist[xPtr] = arr[n];
            let y = 0;
            for (let i = 0; i < b.length; i++)
                y += b[i] * xHist[(xPtr - i + order) % order];
            for (let i = 1; i < a.length; i++)
                y -= a[i] * yHist[(yPtr - i + order) % order];
            y /= a[0];
            yHist[yPtr] = y;
            result[n] = y;
            // 不用取余 加速
            if (++xPtr >= order) xPtr = 0;
            if (++yPtr >= order) yPtr = 0;
        }
        if (reverse) {
            for (let n = arr.length - 1; n >= 0; n--) processSample(n);
        } else {
            for (let n = 0; n < arr.length; n++) processSample(n);
        } return result;
    }

    /**
     * 自相关函数 点积，进行了幅度补偿和归一化
     * 通常用于较长序列
     * @param {Float32Array} arr 输入一维时序信号
     * @param {Number} points 偏移点数
     * @param {Float32Array} result 可选的输出数组
     * @returns {Float32Array} 有效长度为 points
     */
    static autoCorrDiff(arr, points, result = undefined) {
        const L = arr.length;
        if (result === undefined || result.length < points) result = new Float32Array(points);
        // 先计算直流量用于幅度补偿
        let mean = 0;
        for (let i = 0; i < L; i++) mean += arr[i] * arr[i];
        mean = L / mean;
        result[0] = 1;
        // 计算各个tau的自相关值
        for (let tau = 1; tau <= points; tau++) {
            let ac = 0;
            for (let n = L - tau; n >= 0; n--)
                ac += arr[n] * arr[n + tau];
            result[tau] = ac * mean / (L - tau);
        } return result;
    }

    /**
     * 高效的分帧自相关
     * @param {Float32Array} arr onset envelop
     * @param {Number} points 进行多少点的自相关
     * @param {Number} winLen 窗长
     * @param {Number} hop 每次移动多少
     */
    static autoCorrSeg(arr, points, winLen, hop = 1) {
        const len = arr.length;
        if (hop >= winLen || winLen % hop !== 0) throw new Error("Invalid hop/winLen");

        const hopInWin = (winLen / hop) | 0;
        const numFrames = Math.floor((len - winLen - points) / hop);
        if (numFrames < 1) throw new Error("Input array too short");

        const frames = Array(numFrames);
        for (let i = 0; i < numFrames; i++) {
            frames[i] = new Float32Array(points);
        }

        // 预分配一个通用的 bins 数组，避免在 tau 循环中重复创建
        const maxBinCount = Math.ceil(len / hop);
        const bins = new Float32Array(maxBinCount);

        for (let tau = 0; tau < points; tau++) {
            // 预计算当前 tau 下的所有 bins
            const currentBinCount = Math.floor((len - tau) / hop);
            for (let b = 0, start = 0; b < currentBinCount; b++) {
                let binSum = 0;
                // 内部小循环计算一个 hop 长度的乘积和
                for (let n = 0; n < hop; n++, start++) {
                    binSum += arr[start] * arr[start + tau];
                } bins[b] = binSum;
            }

            // 初始窗口的和 (第一个 frame)
            let running = 0;
            for (let f = 0; f < hopInWin; f++) {
                running += bins[f];
            } frames[0][tau] = running;

            // 滑动更新后续 frame
            for (let frameIdx = 1; frameIdx < numFrames; frameIdx++) {
                running += bins[frameIdx + hopInWin - 1] - bins[frameIdx - 1];
                frames[frameIdx][tau] = running;
            }
        }

        // 归一化
        for (let f = 0; f < numFrames; f++) {
            const frame = frames[f];
            const energy = frame[0];
            if (energy < 1e-10) continue;
            const invEnergy = 1 / energy;
            frame[0] = 1;
            for (let tau = 1; tau < points; tau++) {
                frame[tau] *= invEnergy;
            }
        }

        return frames;
    }
}

class BPMEst {
    /**
     * 根据采样率得到适合的FFT长度
     * @param {Number} fs onset的采样率
     * @param {Number} sec 音频长度（秒）
     * @returns {Number} FFT的大小
     */
    static fs2FFTN(fs, sec = 50) {
        let n = fs * sec;   // 用50秒的音频，分辨率大概有1.2BPM
        return 1 << Math.round(Math.log2(n));
    }

    constructor(onset_sr) {
        this.onset_sr = onset_sr;
        this.signalN = BPMEst.fs2FFTN(onset_sr);    // 后续分析需要的信号长度
        this.fft = new realFFT(this.signalN);
    }

    /**
     * 压缩异常大的值 原位操作
     * @param {Float32Array} onsetEnv 
     * @param {Number} percent 
     * @param {Number} margin_ratio 
     * @returns {Float32Array} onsetEnv 返回同一个引用
     */
    static compressOutliers(onsetEnv, percent = 0.99, margin_ratio = 1.3) {
        // 理论上可以用堆排序只找后1%，但是sort底层是C++实现的，性能已经足够好了
        const sorted = Array.from(onsetEnv).sort((a, b) => a - b);
        const margin = sorted[(sorted.length * percent) | 0];   // floor会使得索引一定存在
        const marginMax = margin * margin_ratio;
        const actualMax = sorted[sorted.length - 1];
        if (actualMax <= marginMax) return onsetEnv;
        // 用三次函数压缩
        const x0 = actualMax - margin;
        const y0 = marginMax - margin;
        const a = (x0 - 2 * y0) / (x0 * x0 * x0);
        const b = (3 * y0 - 2 * x0) / (x0 * x0);
        const trans = (x) => x * (a * x * x + b * x + 1);
        for (let i = 0; i < onsetEnv.length; i++) {
            if (onsetEnv[i] > margin) onsetEnv[i] = trans(onsetEnv[i] - margin) + margin;
        } return onsetEnv;
    }

    /**
     * 高通滤波去除趋势 原位操作
     * @param {Float32Array} onsetEnv 
     * @returns {Float32Array} 去趋势后的onsetEnv 同一个引用
     */
    static detrend(onsetEnv) {
        // 对于20Hz采样的频谱，用0.96对低频的压制较好 0.9低频压制太多 0.99低频压制太少
        const b = [1, -1];
        const a = [1, -0.96];
        // filtfilt
        onsetEnv = SIGNAL.filter(onsetEnv, b, a, false);
        onsetEnv = SIGNAL.filter(onsetEnv, b, a, true);
        return onsetEnv;
    }

    /**
     * 【第一步：提取onset】
     * 对数谱差分法提取 onset envelope
     * 会进行抑制峰值和去趋势处理
     * @param {Array<Float32Array>} spectrogram 幅度谱
     * @returns {Float32Array} onset envelope
     */
    static extractOnset(spectrogram) {
        const onsetEnv = new Float32Array(spectrogram.length);
        const prevFrame = new Float32Array(spectrogram[0].length);
        prevFrame.fill(Math.log(1e-8));
        for (let i = 0; i < spectrogram.length; i++) {
            const frame = spectrogram[i];
            let diff = 0;
            for (let j = 0; j < frame.length; j++) {
                const logedValue = Math.log(frame[j] + 1e-8);
                const delta = logedValue - prevFrame[j];
                if (delta > 0) diff += delta;
                prevFrame[j] = logedValue;
            } onsetEnv[i] = diff;
        }
        // 抑制峰值
        BPMEst.compressOutliers(onsetEnv, 0.99, 1.3);
        // 去趋势
        return BPMEst.detrend(onsetEnv);
    }

    /**
     * 浮点数最大公因数
     * @param {Float32Array} idx 峰值序号数组
     * @param {Uint8Array} N 可选的倍数数组
     * @returns {Number} 最大公因数
     */
    static floatGCD(idx, N) {
        // 用最小间隔估计一个初始值 这里假设一次差分就能获取
        let minInterval = idx[0];
        for (let i = 1; i < idx.length; i++) {
            let inter = idx[i] - idx[i - 1];
            if (inter < minInterval) minInterval = inter;
        }
        // 计算各个idx对应的倍数 并动态修正 minInterval
        if (N === undefined) N = new Uint8Array(idx.length);
        for (let i = 0; i < idx.length; i++) {
            const n = Math.round(idx[i] / minInterval);
            N[i] = n;
            // 修正 minInterval 因为距离越远误差越小
            minInterval = (minInterval + idx[i] / n) * 0.5;
        }
        // MSE求最佳 minInterval
        let a = 0, b = 0;
        for (let i = 0; i < idx.length; i++) {
            a += N[i] * N[i];
            b += N[i] * idx[i];
        }
        return b / a;
    }

    /**
     * 【第三步】对自相关结果进行BPM估计
     * 从自相关结果中找到BPM峰值
     * @param {Float32Array} corr 自相关结果
     * @param {Number} sr 采样率
     * @param {Number} BPMstd BPM的标准差 用于高斯加权
     * @param {Number} BPMu 期望的BPM值 用于高斯加权
     * @returns {Number} 估计的BPM值
     */
    static corrBPM(corr, sr, BPMstd = 1, BPMu = 100) {
        if (corr.length < 3) throw new Error("Correlation array too short");
        const maxInterval = Math.ceil(60 * sr / 35) + 1; // 35 BPM对应的最大间隔 更低的不管
        if (corr.length > maxInterval) corr = corr.subarray(0, maxInterval);
        // 峰值插值
        const peakIdx = SIGNAL.findPeaks(corr, 0.02);
        const peak = new Float32Array(peakIdx.length);
        for (let i = 0; i < peakIdx.length; i++) {
            const idx = peakIdx[i];
            const [dx, y] = SIGNAL.parabolicInterpolation(
                corr[idx - 1],
                corr[idx],
                corr[idx + 1]
            );
            peakIdx[i] = idx + dx;
            peak[i] = y;
        }
        // 得到候选BPM
        const N = new Uint8Array(peakIdx.length);
        const nBPM = 60 * sr / BPMEst.floatGCD(peakIdx, N); // 一定是整数倍
        // 选择最显著的BPM 用高斯权重
        let bestBPM = nBPM, maxVal = -1;
        BPMu = Math.log2(BPMu);
        for (let i = 0; i < peakIdx.length; i++) {
            const bpm = nBPM / N[i];  // 候选BPM
            const k = (Math.log2(bpm) - BPMu) / BPMstd;
            const q = peak[i] * Math.exp(-0.5 * k * k);
            if (q > maxVal) {
                maxVal = q;
                bestBPM = bpm;
            }
        } return bestBPM;
    }
}