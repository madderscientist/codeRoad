import numpy as np

# 计算自相关
def autocorr(x, method='dot'):
    ac_values = []
    win_len = len(x)
    half_win = win_len // 2
    if method == 'diff':
        ac_values.append(0.)
        for diff_len in range(1, half_win):
            begin = (half_win - diff_len) // 2
            segment1 = x[begin : begin + half_win]
            segment2 = x[begin + diff_len : begin + diff_len + half_win]
            ac_value = np.sum((segment1 - segment2) ** 2)
            ac_values.append(ac_value)
        runningSum = 0
        for tau in range(1, half_win):
            runningSum += ac_values[tau]
            if runningSum == 0:
                ac_values[tau] = 1
            else:
                ac_values[tau] *= tau / runningSum
    elif method == 'dot':
        for diff_len in range(0, half_win):
            # begin = (half_win - diff_len) // 2
            # segment1 = x[begin : begin + half_win]
            # segment2 = x[begin + diff_len : begin + diff_len + half_win]
            segment1 = x[:win_len - diff_len]
            segment2 = x[diff_len:win_len]
            ac_value = np.dot(segment1, segment2) / len(segment1)
            if diff_len != 0:
                ac_value /= ac_values[0]
            ac_values.append(ac_value)
        ac_values[0] = 1.0
    else:
        raise ValueError("Unsupported method. Use 'diff' or 'dot'.")
    return np.array(ac_values)

def parabolic_interpolation(y, x):
    """
    对峰值点进行抛物线插值，y为序列，x为峰值索引
    返回插值后的峰值位置和幅值
    """
    if x <= 0 or x >= len(y) - 1:
        return x, y[x]
    xv = 0.5 * (y[x-1] - y[x+1]) / (y[x-1] - 2*y[x] + y[x+1]) + x
    yv = y[x] - 0.25 * (y[x-1] - y[x+1]) * (xv - x)
    return xv, yv

def getBPMFromCorr(idx, sr):
    """
    求误差最小的BPM值
    idx 是自相关峰值列表，已经抛物线插值
    """
    idx = np.array(idx)
    min_interval = idx[0]   # 和0的差值
    for i in range(1, len(idx)):
        inter = idx[i] - idx[i-1]
        if inter < min_interval:
            min_interval = inter

    coeff = np.empty_like(idx, dtype=int)
    for i in range(len(idx)):
        n = np.round(idx[i] / min_interval)
        coeff[i] = n
        min_interval = idx[i] / n  # 动态调整最小间隔 防止min带来的对n的偏大估计
    
    bpm = 60 * sr * np.dot(coeff, coeff) / np.dot(coeff, idx)
    return bpm, bpm/coeff