class SparseTransition {
    /**
     * @param {Array} triples 三元组数组，每个元素为 [from, to, prob] 状态idx不超过65536
     * @param {number} num_states 状态总数 如果不传则自动推断
     */
    constructor(triples, num_states, log = true) {
        triples.sort((a, b) => a[0] - b[0]);
        if (num_states == undefined) {
            num_states = 0;
            for (const [from, to, _] of triples) {
                if (from + 1 > num_states) num_states = from + 1;
                if (to + 1 > num_states) num_states = to + 1;
            }
        }
        if (num_states > 65536) {
            throw new Error("状态数超过65536，无法使用Uint16Array存储索引");
        }
        this.indices = new Uint16Array(triples.length);
        this.data = new Float32Array(triples.length);
        this.pointers = new Uint32Array(num_states + 1);

        let cur_from = 0, ptr = 0;
        for (let i = 0; i < triples.length; i++) {
            const [from, to, p] = triples[i];
            // 填充pointers直到当前from
            while (cur_from <= from) {
                this.pointers[cur_from] = ptr;
                cur_from++;
            }
            this.indices[ptr] = to;
            this.data[ptr] = log ? Math.log(p) : p;
            ptr++;
        }
        // 填充剩余pointers
        while (cur_from <= num_states) {
            this.pointers[cur_from] = ptr;
            cur_from++;
        }
    }

    get numStates() {return this.pointers.length - 1;}
    get numTransitions() {return this.data.length;}

    /**
     * 获取from状态所有可达的to状态及概率
     * @param {number} from
     * @returns {{to: Uint16Array, prob: Float32Array}}
     */
    getTransitions(from) {
        const start = this.pointers[from];
        const end = this.pointers[from + 1];
        return {
            to: this.indices.subarray(start, end),
            prob: this.data.subarray(start, end)
        };
    }
}

class DBNBeatTracker {
    static EPS = 2.221e-16;
    static MAX_INTERVAL_NUM = 32;
    /**
     * 采用均匀间隔 fs=20; bpm in [40, 300] 时状态数为 459 无需使用对数间隔
     * @param {Number} min_interval 最小间隔
     * @param {Number} max_interval 最大间隔
     * @return {Uint8Array}
     */
    static uniIntervals(min_interval, max_interval) {
        // 用uint8最大值255 在min_bpm为30的情况下 fs最大取127.5 一般都比这个小
        const framepbeat = new Uint8Array(max_interval - min_interval + 1);
        for (let i = min_interval, j = 0; i <= max_interval; i++, j++) framepbeat[j] = i;
        return framepbeat;
    }

    /**
     * 采用论文的对数间隔 
     * 若使用均匀间隔 fs=44100/512; bpm in [40, 300] 时状态数 8379 太多了
     * @param {Number} min_interval 最小间隔
     * @param {Number} max_interval 最大间隔
     * @param {Number} num_intervals 间隔类别数
     * @return {Uint8Array}
     */
    static logIntervals(min_interval, max_interval, num_intervals) {
        const framepbeat = new Uint8Array(num_intervals);
        framepbeat[0] = min_interval;
        let log_num_intervals = num_intervals - 1;
        const log_min = Math.log2(min_interval);
        const log_max = Math.log2(max_interval);
        let j = 1;
        while (j < num_intervals) {
            j = 1;
            const factor = (log_max - log_min) / log_num_intervals;
            log_num_intervals++;
            for (let i = 1; i < log_num_intervals && j < num_intervals; i++) {
                framepbeat[j] = Math.round(Math.pow(2, log_min + factor * i));
                if (framepbeat[j] !== framepbeat[j - 1]) j++;
            }
        }
        return framepbeat;
    }

    /**
     * @type {Uint8Array}  每个beat包含多少帧 的所有可能
     * 对应python版本的intervals
     */
    framepbeat;
    /**
     * @type {Float32Array} 所有相位状态
     * 对应python版本的state_positions
     */
    states;
    /**
     * @type {Uint8Array} 每个状态对应的 “每个beat包含多少帧”，相当于角速度，便于索引
     * 对应python版本的state_intervals
     */
    stateIntervals;
    /**
     * @type {Uint16Array} 每个“每个beat包含多少帧”对应的状态在states中的起始索引
     * 对应python版本的first_states
     * 长度比framepbeat多1，表示最后一个interval的结束位置+1
     */
    stateOffset;
    /**
     * @type {SparseTransition} 转移矩阵
     */
    transitionModel;
    /**
     * @type {Uint8Array} 指示每个状态是否在状态边界附近
     */
    downFlag;
    /**
     * @type {Float32Array} 初始分布，已经对数化
     */
    initialDistribution;

    /**
     * @param {Number} fs 一维数据的采样率
     * @param {Number} transition_lambda 转移矩阵的lambda参数
     * @param {Number} observation_lambda 观测矩阵的lambda参数
     * @param {Number} min_bpm 
     * @param {Number} max_bpm 
     */
    constructor(fs = 20, transition_lambda = 100, observation_lambda = 16, min_bpm = 40, max_bpm = 300, threshold = DBNBeatTracker.EPS) {
        this.fs = fs;
        this.transition_lambda = transition_lambda;
        this.observation_lambda = observation_lambda;
        this.threshold = threshold;
        // state相关变量初始化
        this.initState(
            Math.floor(60 * fs / max_bpm),  // 1 beat包含多少帧
            Math.ceil(60 * fs / min_bpm)
        );
        // transition model
        this.initTransitionModel();
        // observation model
        this.initObservationModel();
        // 初始分布 并对数化
        this.initialDistribution = new Float32Array(this.states.length);
        this.initialDistribution.fill(-Math.log(this.states.length));
    }

    /**
     * 初始化状态相关变量
     * @param {Number} min_interval 最小beat长度 (单位:帧数)
     * @param {Number} max_interval 最大beat长度 (单位:帧数)
     */
    initState(min_interval, max_interval) {
        // 可能的beat长度
        const _bpm_num = max_interval - min_interval + 1;
        const framepbeat = this.framepbeat = (_bpm_num < DBNBeatTracker.MAX_INTERVAL_NUM) ?
            DBNBeatTracker.uniIntervals(min_interval, max_interval) :
            DBNBeatTracker.logIntervals(min_interval, max_interval, DBNBeatTracker.MAX_INTERVAL_NUM);
        // 一个beat中的状态数 比如framepbeat[i]对应一个beat中有framepbeat[i]个状态
        let numStates = 0;
        for (const fpb of framepbeat) numStates += fpb;
        // 下面将所有状态变为一维索引
        const states = this.states = new Float32Array(numStates);
        const stateIntervals = this.stateIntervals = new Uint8Array(numStates);
        const stateOffset = this.stateOffset = new Uint16Array(framepbeat.length + 1);
        let idx = 0;
        let offset_idx = 0;
        for (const fpb of framepbeat) {
            stateOffset[offset_idx++] = idx;
            // beat长度为fpb时在对应的相位
            for (let i = 0; i < fpb; i++, idx++) {
                states[idx] = i / fpb;
                stateIntervals[idx] = fpb;
            }
        }
        stateOffset[offset_idx] = idx; // 最后一个位置
    }

    /**
     * 初始化转移矩阵
     */
    initTransitionModel() {
        // 先建模“最后一帧到下一beat第一帧的转移” 此时考虑interval的切换
        const transition = this.exponential_transition();
        // 两个state索引表示：每个interval中的状态转移对(from_idx, to_idx)概率都是1，意思是beat的中间过程保持interval不变
        const in_interval_num = this.states.length - this.stateOffset.length;
        // 得到interval内的转移对 beat内不转移
        for (let i = 0, j = 1, state_idx = 1; i < in_interval_num;) {
            const to = this.stateOffset[j];
            while (state_idx < to) {
                transition.push([state_idx - 1, state_idx, 1]);
                i++; state_idx++;
            } state_idx++;  // 跳过to
        }
        // 现在已经得到了完整的转移三元组表示 转为稀疏存储
        this.transitionModel = new SparseTransition(transition, this.states.length);
    }

    /**
     * 用论文的概率方法构建转移矩阵
     * @param {Number} threshold 用于稀疏化的阈值 当lambda较大的时候会有很多接近0的概率
     * @returns {Array} [from_state_idx, to_state_idx, trans_prob]
     */
    exponential_transition(threshold = DBNBeatTracker.EPS) {
        const trans = [];
        const fix_from_prob = new Float32Array(this.framepbeat.length); // 固定from，改变to
        const stateOffset = this.stateOffset;
        // 下面的from和to表示当前interval的索引
        for (let from = 0; from < this.framepbeat.length; from++) {
            let sum = 0;
            for (let to = 0; to < this.framepbeat.length; to++) {
                const value = Math.exp(-this.transition_lambda * Math.abs(this.framepbeat[from] / this.framepbeat[to] - 1));
                if (value < threshold) {
                    fix_from_prob[to] = 0;
                    continue;
                }
                fix_from_prob[to] = value;
                sum += value;
            }
            const from_state_idx = stateOffset[from + 1] - 1; // from interval的最后一个状态
            for (let to = 0; to < this.framepbeat.length; to++) {
                if (fix_from_prob[to] === 0) continue;
                // 三元组的from和to指的是状态索引
                trans.push([
                    from_state_idx,
                    stateOffset[to],   // to interval的第一个状态
                    fix_from_prob[to] / sum // 归一化
                ]);
            }
        }
        return trans;
    }

    /**
     * 初始化观测模型
     */
    initObservationModel() {
        const border = 1 / this.observation_lambda;
        const pointers = this.downFlag = new Uint8Array(this.states.length);
        for (let i = 0; i < this.states.length; i++) {
            if (this.states[i] < border) {  // 论文只管了前半部分没有后半部分
                pointers[i] = 1;
            }
        }
        // 临时变量
        this._tempDensities = new Float32Array(2);
        this.log_norm_factor = Math.log(this.observation_lambda - 1);
    }

    /**
     * 计算单帧的对数密度
     * 为了内存优化，不一次性生成 (N, 2) 矩阵，而是按帧计算
     * @param {number} observation 当前帧的beat激活值
     * @param {Float32Array} result 用于存储结果的数组，长度至少为2 [log_nobeat, log_beat]
     */
    computeFrameLogDensities(observation, result) {
        const EPSILON = DBNBeatTracker.EPS;
        let obs = observation;
        if (obs < EPSILON) obs = EPSILON;
        if (obs > 1 - EPSILON) obs = 1 - EPSILON;
        result[0] = Math.log(1 - obs) - this.log_norm_factor;
        result[1] = Math.log(obs);
    }

    /**
     * Viterbi解码
     * @param {Float32Array} observations 每一帧的beat激活值 (概率)
     * @returns {Uint32Array} 最优状态路径
     */
    viterbi(observations) {
        const L = observations.length;
        const N = this.states.length;

        // 使用双缓冲通过交换引用来更新，避免频繁GC
        let prevLogDelta = new Float32Array(N);
        let currLogDelta = new Float32Array(N);
        prevLogDelta.set(this.initialDistribution);

        // 回溯矩阵：N * L 展平为一维数组
        const psi = new Uint16Array(L * N);

        const tm_indices = this.transitionModel.indices;
        const tm_data = this.transitionModel.data;
        const tm_pointers = this.transitionModel.pointers;
        const downFlag = this.downFlag;

        // 缓存常量
        const NEG_INF = -Infinity;

        const logdens = new Float32Array(2);
        for (let t = 0; t < L; t++) {
            // 初始化当前帧概率
            currLogDelta.fill(NEG_INF);
            const rowOffset = t * N; // 回溯矩阵的行偏移量

            // 状态转移 (Push模式: 从上一帧状态推导当前帧) 比查找前驱更适合稀疏矩阵结构
            for (let fromState = 0; fromState < N; fromState++) {
                const prevProb = prevLogDelta[fromState];
                if (prevProb === NEG_INF) continue;

                // 获取该状态出发的所有转移
                const start = tm_pointers[fromState];
                const end = tm_pointers[fromState + 1];

                for (let k = start; k < end; k++) {
                    const toState = tm_indices[k];
                    const transProb = tm_data[k];
                    const newProb = prevProb + transProb;
                    if (newProb > currLogDelta[toState]) {
                        currLogDelta[toState] = newProb;
                        psi[rowOffset + toState] = fromState;
                    }
                }
            }
            // 应用发射概率 由于状态确定后这个概率是固定的，可以在这里统一加上而不必参与转移计算
            this.computeFrameLogDensities(observations[t], logdens);
            for (let s = 0; s < N; s++) {
                if (currLogDelta[s] !== NEG_INF) {
                    // downFlag[s] 为 1 表示 beat 状态，使用 obs_beat
                    currLogDelta[s] += downFlag[s] ? logdens[1] : logdens[0];
                }
            }
            // 交换缓冲区
            [prevLogDelta, currLogDelta] = [currLogDelta, prevLogDelta];
        }
        // 循环结束后，prevLogDelta 持有最后一帧的概率

        // 回溯
        // 找到最后一帧概率最大的状态
        let bestState = 0;
        let maxProb = NEG_INF;
        for (let i = 0; i < N; i++) {
            if (prevLogDelta[i] > maxProb) {
                maxProb = prevLogDelta[i];
                bestState = i;
            }
        }

        // 构建路径
        const path = new Uint32Array(L);
        path[L - 1] = bestState;
        for (let t = L - 1; t > 0; t--) {
            // 使用 rowOffset = t * N 访问上一时刻记录的来源
            bestState = psi[t * N + bestState];
            path[t - 1] = bestState;
        }

        return path;
    }

    /**
     * 处理激活信号并提取Beat时刻
     * 对应 features/beats.py 中的 process_offline
     * @param {Float32Array} activations 神经网络输出的beat激活值
     * @param {boolean} correct 是否校正beat位置到局部最大值
     * @returns {Float32Array} beat的时间(秒)
     */
    process(activations, correct = true) {
        // 1. 阈值与切片处理
        let first = 0;
        let last = activations.length;
        while (first < last && activations[first] < this.threshold) first++;
        while (last > first && activations[last - 1] < this.threshold) last--;
        if (first >= last) return new Float32Array(0);
        const act_slice = activations.subarray(first, last);
        
        // 2. Viterbi 解码
        // path 存储的是每一帧的最优状态索引
        const path = this.viterbi(act_slice);

        // 3. Beat 时间提取与校正
        const beats = [];
        if (correct) {
            // 对应 Python: if self.correct: 寻找观测指针为1的连续区域，取区域内激活值最大值
            // 这能修正由于状态离散化导致的时间偏差
            let inRange = false;
            let rangeStart = 0;
            for (let i = 0; i < path.length; i++) {
                const state = path[i];
                if (this.downFlag[state] === 1) {
                    if (!inRange) {
                        // 进入 Beat 区域
                        rangeStart = i;
                        inRange = true;
                    }
                } else {
                    if (inRange) {
                        // 刚刚离开 Beat 区域，处理这种 [rangeStart, i) 的区间
                        // 寻找区间内 activation 的最大值位置
                        let maxVal = -Infinity;
                        let maxIdx = rangeStart;
                        for (let k = rangeStart; k < i; k++) {
                            if (act_slice[k] > maxVal) {
                                maxVal = act_slice[k];
                                maxIdx = k;
                            }
                        }
                        beats.push((first + maxIdx) / this.fs);
                        inRange = false;
                    }
                }
            }
            // 处理由 Beat 区域结尾的情况 (即最后一个区间延伸到了数组末尾)
            if (inRange) {
                let maxVal = -Infinity;
                let maxIdx = rangeStart;
                for (let k = rangeStart; k < path.length; k++) {
                    if (act_slice[k] > maxVal) {
                        maxVal = act_slice[k];
                        maxIdx = k;
                    }
                }
                beats.push((first + maxIdx) / this.fs);
            }
        } else {
            // 如果不校正，直接取相位最接近 0 且被标记为 beat 的位置
            // 这里简化处理：寻找 downFlag 为 1 的区域中，相位值最小的点（通常接近0）
            for (let i = 0; i < path.length; i++) {
                const stateIdx = path[i];
                if (this.downFlag[stateIdx] !== 1) continue;
                // 检测是否是局部相位极小值 (处理 wrap around 0 的情况)
                // 简单的检测方法：如果是周期的开始 (state 0) 或者比前一帧相位小
                const currentPhase = this.states[stateIdx];
                // 检查前一帧
                if (i > 0) {
                    const prevPhase = this.states[path[i-1]];
                    // 如果当前相位比前一帧小，说明刚刚跨过了 1->0 的边界
                    if (currentPhase < prevPhase) {
                            beats.push((first + i) / this.fs);
                    }
                } else if (currentPhase < 0.05) { // 第一帧就是beat开始
                    beats.push((first + i) / this.fs);
                }
            }
        }
        return new Float32Array(beats);
    }
}