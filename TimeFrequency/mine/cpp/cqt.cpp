#include "fft.cpp"

#ifndef __CQT__
#define __CQT__

float* blackmanHarris(uint16_t N) {
    float* window = new float[N];
    const double temp = 2 * M_PI / (N - 1);
    const double a0 = 0.35875;
    const double a1 = 0.48829;
    const double a2 = 0.14128;
    const double a3 = 0.01168;
    for (uint16_t n = 0; n < N; n++) {
        window[n] = a0
            - a1 * cos(temp * n)
            + a2 * cos(temp * 2 * n)
            - a3 * cos(temp * 3 * n);
    } return window;
}

class ParseKernel {
public:
    uint16_t begin;
    uint16_t end;
    float* real;
    float* imag;
    ParseKernel(float* r, float* i, uint16_t N, float threshold = 0.1) {

    }
};

class CQT {
private:
    uint16_t fs;
    uint16_t hop;
    uint8_t octaves;
    uint8_t bins_per_octave;
public:
    CQT(uint16_t fs = 44100, uint16_t hop = 1, float fmin = 32.7, uint8_t octaves = 7, uint8_t bins_per_octave = 12):
        fs(fs), hop(hop), octaves(octaves), bins_per_octave(bins_per_octave)
    {   
        float Q = 1 / pow(2, 1.0 / bins_per_octave);
        // 确定最高八度的频率
    }
    ~CQT() {

    }
    void cqt() {

    }
    void iniKernel(float Q, uint16_t fs, float fmin, uint8_t bins_per_octave = 12) {
        uint16_t fftLen = Q * fs / fmin;    // 用最低的频率算出最长的时域窗长
        fftLen = pow(2, ceil(log2(fftLen))); // 取大于等于fftLen的最小2的幂
        // 需要知道每个频点
    }
};
#endif