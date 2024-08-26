#include <cmath>
#include <cstdint>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef __FFT__
#define __FFT__

inline void complexMul(float ar, float ai, float br, float bi, float& cr, float& ci) {
    cr = ar * br - ai * bi;
    ci = ar * bi + ai * br;
}

void complexAbs(float* r, float* i, float* l, uint32_t len) {
    for(uint32_t j = 0; j < len; j++)
        l[j] = sqrt(r[j] * r[j] + i[j] * i[j]);
}

// 完全体复数FFT
// 用于计算CQT中的窗频域
class FFT {
private:
    uint16_t N; // 实际的FFT长度
    uint16_t *bitReversed;  // 位翻转表
    float *Wr;  // 旋转因子实部
    float *Wi;  // 旋转因子虚部
    float *buffer_r;
    float *buffer_i;
    float *Xr;
    float *Xi;
public:
    FFT(uint32_t N) {
        N = pow(2, ceil(log2(N)));
        this->N = N;
        this->bitReversed = FFT::reverseBits(N);
        this->buffer_r = new float[N];
        this->buffer_i = new float[N];
        this->Xr = new float[N];
        this->Xi = new float[N];
        // 预计算旋转因子
        N >>= 1;  // 以N为底，但是只用算前一半
        float temp = M_PI / N;
        this->Wr = new float[N];
        this->Wi = new float[N];
        float angle = 0;
        for(int i = 0; i < N; i++, angle += temp) {
            this->Wr[i] = cos(angle);
            this->Wi[i] = -sin(angle);
        }
    }
    ~FFT() {
        delete[] bitReversed;
        delete[] Wr;
        delete[] Wi;
        delete[] buffer_r;
        delete[] buffer_i;
        delete[] Xr;
        delete[] Xi;
    }
    void fft(float* input_r, float* input_i) {
        // 重排数据并计算第一层
        // 由于复数FFT用于CQT中计算窗的频域，因此不做越界检查
        for(uint16_t i = 0, ii = 1; i < this->N; i += 2, ii += 2) {
            uint16_t k1 = this->bitReversed[i];
            uint16_t k2 = this->bitReversed[ii];
            float xr1 = input_r[k1];
            float xi1 = input_i[k1];
            float xr2 = input_r[k2];
            float xi2 = input_i[k2];
            this->buffer_r[i] = xr1 + xr2;
            this->buffer_i[i] = xi1 + xi2;
            this->buffer_r[ii] = xr1 - xr2;
            this->buffer_i[ii] = xi1 - xi2;
        }
        for(uint16_t groupNum = this->N >> 2, groupMem = 2; groupNum; groupNum >>= 1) {
            // groupNum: 组数；groupMem：一组里有几个蝶形结构，同时也是一个蝶形结构两个元素的序号差值
            // groupNum: N/4,  N/8, ...,    1
            // groupMem: 2,    4,   ...,    N/2
            // W's base: 4,    8,   ...,    N
            // offset between groups: 4, 8, ..., N
            uint16_t groupOffset = groupMem << 1;
            for(uint16_t mem = 0, k = 0; mem < groupMem; mem++, k += groupNum) {
                float Wr = this->Wr[k];
                float Wi = this->Wi[k];
                float gwr, gwi;
                for(uint16_t gn = mem; gn < this->N; gn += groupOffset) {
                    uint16_t gn2 = gn + groupMem;
                    complexMul(this->buffer_r[gn2], this->buffer_i[gn2], Wr, Wi, gwr, gwi);
                    this->Xr[gn] = this->buffer_r[gn] + gwr;
                    this->Xi[gn] = this->buffer_i[gn] + gwi;
                    this->Xr[gn2] = this->buffer_r[gn] - gwr;
                    this->Xi[gn2] = this->buffer_i[gn] - gwi;
                }
            }
            // 交换buffer和X
            float* temp = this->buffer_r;
            this->buffer_r = this->Xr;
            this->Xr = temp;
            temp = this->buffer_i;
            this->buffer_i = this->Xi;
            this->Xi = temp;
            // 迭代
            groupMem = groupOffset;
        }
    }

    void output(float* &output_r, float* &output_i) {
        output_r = this->buffer_r;
        output_i = this->buffer_i;
    }

    static uint16_t* reverseBits(unsigned int N) {
        uint16_t *bitReversed = new uint16_t[N];
        bitReversed[0] = 0;
        // 计算位数
        uint8_t bits = 15;
        while((1 << bits) > N) bits--;
        bits = 16 - bits;
        for (uint16_t i = 1; i < N; i++) {
            // 基于二分法的位翻转
            uint16_t r = ((i & 0xaaaa) >> 1) | ((i & 0x5555) << 1);
            r = ((r & 0xcccc) >> 2) | ((r & 0x3333) << 2);
            r = ((r & 0xf0f0) >> 4) | ((r & 0x0f0f) << 4);
            bitReversed[i] = ((r >> 8) | (r << 8)) >> bits;
        } return bitReversed;
    }
};

// 实数FFT，计算N点，返回前N/2个数据
// 用于计算STFT
class realFFT {
private:
    uint16_t N; // 实际的FFT长度
    uint16_t *bitReversed;  // 位翻转表
    float *Wr;  // 旋转因子实部 以2N为底，只有N项
    float *Wi;  // 旋转因子虚部
    float *buffer_r;
    float *buffer_i;
    float *Xr;
    float *Xi;
public:
    realFFT(uint32_t N) {
        N = pow(2, ceil(log2(N)) - 1);  // 实际的FFT长度只有一半
        this->N = N;
        this->bitReversed = realFFT::reverseBits(N);
        this->buffer_r = new float[N];
        this->buffer_i = new float[N];
        this->Xr = new float[N];
        this->Xi = new float[N];
        // 预计算旋转因子
        float temp = M_PI / N;  // 以2N为底，但是只用算前一半
        this->Wr = new float[N];
        this->Wi = new float[N];
        float angle = 0;
        for(int i = 0; i < N; i++, angle += temp) {
            this->Wr[i] = cos(angle);
            this->Wi[i] = -sin(angle);
        }
    }
    ~realFFT() {
        delete[] bitReversed;
        delete[] Wr;
        delete[] Wi;
        delete[] buffer_r;
        delete[] buffer_i;
        delete[] Xr;
        delete[] Xi;
    }
    void fft(float* input, uint32_t len, int32_t offset = 0) {
        // 偶数次和奇数次组合并计算第一层
        for(uint16_t i = 0, ii = 1; i < this->N; i += 2, ii += 2) {
            // 第一个复数
            int32_t temp = this->bitReversed[i] + offset;
            float xr1 = (temp < len && temp >= 0) ? input[temp] : 0;
            temp++;
            float xi1 = (temp < len && temp >= 0) ? input[temp] : 0;
            // 第二个复数
            temp = this->bitReversed[ii] + offset;
            float xr2 = (temp < len && temp >= 0) ? input[temp] : 0;
            temp++;
            float xi2 = (temp < len && temp >= 0) ? input[temp] : 0;
            // 计算第一层
            this->buffer_r[i] = xr1 + xr2;
            this->buffer_i[i] = xi1 + xi2;
            this->buffer_r[ii] = xr1 - xr2;
            this->buffer_i[ii] = xi1 - xi2;
        }
        for(uint16_t groupNum = this->N >> 2, groupMem = 2; groupNum; groupNum >>= 1) {
            // groupNum: 组数；groupMem：一组里有几个蝶形结构，同时也是一个蝶形结构两个元素的序号差值
            // groupNum: N/4,  N/8, ...,    1
            // groupMem: 2,    4,   ...,    N/2
            // W's base: 4,    8,   ...,    N
            // W's base desired: 2N
            // times to k: N/2, N/4 --> equals to 2*groupNum (W_base*k_times=W_base_desired)
            // offset between groups: 4, 8, ..., N --> equals to 2*groupMem
            uint16_t groupOffset = groupMem << 1;
            for(uint16_t mem = 0, k = 0, dk = groupNum << 1; mem < groupMem; mem++, k += dk) {
                float Wr = this->Wr[k];
                float Wi = this->Wi[k];
                float gwr, gwi;
                for(uint16_t gn = mem; gn < this->N; gn += groupOffset) {
                    uint16_t gn2 = gn + groupMem;
                    complexMul(this->buffer_r[gn2], this->buffer_i[gn2], Wr, Wi, gwr, gwi);
                    this->Xr[gn] = this->buffer_r[gn] + gwr;
                    this->Xi[gn] = this->buffer_i[gn] + gwi;
                    this->Xr[gn2] = this->buffer_r[gn] - gwr;
                    this->Xi[gn2] = this->buffer_i[gn] - gwi;
                }
            }
            // 交换buffer和X
            float* temp = this->buffer_r;
            this->buffer_r = this->Xr;
            this->Xr = temp;
            temp = this->buffer_i;
            this->buffer_i = this->Xi;
            this->Xi = temp;
            // 迭代
            groupMem = groupOffset;
        }
        // 合并为实数FFT的结果
        this->Xr[0] = this->buffer_r[0] + this->buffer_i[0];
        this->Xi[0] = 0;    // 第一个手动算
        for(uint16_t k = 1, Nk = this->N - 1; Nk; k++, Nk--) {
            float Ir, Ii;
            complexMul(
                this->buffer_i[k] + this->buffer_i[Nk],
                this->buffer_r[Nk] - this->buffer_r[k],
                this->Wr[k], this->Wi[k], Ir, Ii
            );
            this->Xr[k] = (this->buffer_r[k] + this->buffer_r[Nk] + Ir) * 0.5;
            this->Xi[k] = (this->buffer_i[k] - this->buffer_i[Nk] + Ii) * 0.5;
        }
    }

    void output(float* &output_r, float* &output_i) {
        output_r = this->Xr;
        output_i = this->Xi;
    }

    static uint16_t* reverseBits(unsigned int N) {
        uint16_t *bitReversed = new uint16_t[N];
        bitReversed[0] = 0;
        // 计算位数
        uint8_t bits = 15;
        while((1 << bits) > N) bits--;
        // 由于是实数FFT，偶次为实部，奇次为虚部，故最终结果要乘2，所以不是16-bits
        bits = 15 - bits;
        for (uint16_t i = 1; i < N; i++) {
            // 基于二分法的位翻转
            uint16_t r = ((i & 0xaaaa) >> 1) | ((i & 0x5555) << 1);
            r = ((r & 0xcccc) >> 2) | ((r & 0x3333) << 2);
            r = ((r & 0xf0f0) >> 4) | ((r & 0x0f0f) << 4);
            bitReversed[i] = ((r >> 8) | (r << 8)) >> bits;
        } return bitReversed;
    }
};
#endif