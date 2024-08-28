#include <cmath>
#include <cstdint>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef __CQT__
#define __CQT__

class CQT {
private:
    uint16_t hop;
    uint8_t notes;
    uint8_t bins_per_octave;
    float** kernel_r;
    float** kernel_i;
    uint32_t* kernel_len;

public:
    CQT(uint16_t fs = 44100, uint16_t hop = 2205, float fmin = 32.7, uint8_t notes = 84, uint8_t bins_per_octave = 12, float filter_scale = 3) : hop(hop), notes(notes), bins_per_octave(bins_per_octave) {
        float Q = filter_scale / (pow(2, 1.0 / bins_per_octave) - 1);
        this->iniKernel(Q, fs, fmin);
    }
    ~CQT() {
        for (uint16_t i = 0; i < this->notes; i++) {
            delete[] this->kernel_r[i];
            delete[] this->kernel_i[i];
        }
        delete[] this->kernel_r;
        delete[] this->kernel_i;
        delete[] this->kernel_len;
    }
    static float* blackmanHarris(uint16_t N) {
        float* window = new float[N];
        const double temp = 2 * M_PI / (N - 1);
        float sum = 0;
        for (uint16_t n = 0; n < N; n++) {
            window[n] = 0.35875 - 0.48829 * cos(temp * n) + 0.14128 * cos(temp * 2 * n) - 0.01168 * cos(temp * 3 * n);
            sum += window[n];
        }
        for (uint16_t n = 0; n < N; n++) window[n] /= sum;
        return window;
    }
    void iniKernel(float Q, uint16_t fs, float fmin) {
        float** kernel_r = this->kernel_r = new float*[this->notes];
        float** kernel_i = this->kernel_i = new float*[this->notes];
        uint32_t* kernel_len = this->kernel_len = new uint32_t[this->notes];
        for (uint8_t i = 0; i < this->notes; i++) {
            const float freq = fmin * pow(2, float(i) / this->bins_per_octave);
            uint32_t len = kernel_len[i] = ceil(Q * fs / freq);
            if (len < this->hop) len = this->hop;
            float* temp_kernel_r = kernel_r[i] = new float[len];
            float* temp_kernel_i = kernel_i[i] = new float[len];
            float* window = CQT::blackmanHarris(len);
            const float omega = 2 * M_PI * freq / fs;
            const int64_t half_len = len >> 1;
            for (int64_t j = 0; j < len; j++) {
                const float angle = omega * (j - half_len);
                temp_kernel_r[j] = window[j] * cos(angle);
                temp_kernel_i[j] = window[j] * sin(angle);
            }
            delete[] window;
        }
    }
    void cqt(float* x, uint64_t length, float** output, uint64_t& output_length) {
        uint64_t offset = this->hop >> 1;
        output_length = ceil((length - offset) / this->hop);
        output = new float*[output_length];
        uint64_t pointer = 0;
        for (; offset < length; offset += this->hop) {
            float* energy = output[pointer++] = new float[this->notes];
            for (uint16_t note = 0; note < this->notes; note++) {
                const float* kernel_r = this->kernel_r[note];
                const float* kernel_i = this->kernel_i[note];
                const uint64_t kernel_len = this->kernel_len[note];
                float sum_r = 0; float sum_i = 0;
                const uint64_t left = offset - (kernel_len >> 1);
                uint64_t right = length - left;
                if (right > kernel_len) right = kernel_len;
                for (uint64_t i = left >= 0 ? 0 : -left; i < right; i++) {
                    const uint64_t index = i + left;
                    if (index >= length) break;
                    sum_r += x[index] * kernel_r[i];
                    sum_i += x[index] * kernel_i[i];
                }
                energy[note] = sqrt(sum_r * sum_r + sum_i * sum_i) * 32;
            }
        }
    }
};
#endif