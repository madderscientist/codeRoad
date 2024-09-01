#include <cmath>
#include <cstdint>
#include <emscripten/val.h>
#include <emscripten/bind.h>

using namespace emscripten;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 输入是Float32Array
float* getFloatPtrFrom1XArray(val arr, uint32_t &len) {
    // as<uint64_t> 报错: Uncaught BindingError: emval::as has unknown type y
    // 应该返回一个Number，Number对应的类型不包括int64_t，请查看emscripten::val的文档↓
    // https://emscripten.org/docs/porting/connecting_cpp_and_javascript/embind.html?highlight=val#using-val-to-transliterate-javascript-to-c
    len = arr["length"].as<uint32_t>();
    float *ret = new float[len];
    uintptr_t ptr = reinterpret_cast<uintptr_t>(ret);
    val module = val::global("Module");
    module["HEAPF32"].call<void>("set", arr, val(ptr / sizeof(float)));
    return ret;
}
// typed_memory_view不会复制数据，转换成js Array很快，但是释放必须由C++完成
val get2XArray(float **arr, int d1_len, int d2_len){
	val ret = val::array();
	for(int i = 0; i < d1_len; i ++)
		ret.set(i, val(typed_memory_view(d2_len, arr[i])));
	return ret;
}

float* filt(float* x, uint32_t length, float* filter_num, float* filter_den, uint32_t filter_len, float* output = nullptr, bool reverse = false) {
    if(output == nullptr) output = new float[length];
    if(reverse) {
        for (uint32_t i = length - 1; i >= 0; i--) {
            float sum = filter_num[0] * x[i];
            for (uint32_t j = 1; j < filter_len; j++) {
                if (i + j >= length) break;
                sum += x[i + j] * filter_num[j] - output[i + j] * filter_den[j];
            }
            output[i] = sum;
        }
    } else {
        for (uint32_t i = 0; i < length; i++) {
            float sum = filter_num[0] * x[i];
            for (int32_t j = 1; j < filter_len; j++) {
                if (int32_t(i - j) < 0) break;
                sum += x[i + j] * filter_num[j] - output[i + j] * filter_den[j];
            }
            output[i] = sum;
        }
    } return output;
}

// 会改变x的值
float* filtfilt(float* x, uint32_t length, float* filter_num, float* filter_den, uint32_t filter_len) {
    float* temp = filt(x, length, filter_num, filter_den, filter_len, nullptr, false);
    return filt(temp, length, filter_num, filter_den, filter_len, x, false);
}

// 之所以是降4，是因为以44100采样率来看，最高的音符是4186.01Hz，降采样到44100/4=11025Hz，还是可以保留最高音符
void down_4(float* &x, uint32_t &length) {
    uint32_t new_length = ceil(length >> 2);
    float* temp = new float[new_length];
    for (uint32_t i = 0, j = 0; j < new_length; i += 4, j++) temp[j] = x[i];
    delete[] x;
    x = temp;
    length = new_length;
}

const float down4_num[7] = {0.00105164679630761,0.00630988077784564,0.0157747019446141,0.0210329359261521,0.0157747019446141,0.00630988077784564,0.00105164679630761};
const float down4_den[7] = {1,-2.97852992612413,4.13608099825748,-3.25976427975097,1.51727884474047,-0.391117230593913,0.0433569884347560};

/*
常数Q变换 CQT 类
*/
class CQT {
private:
    uint16_t hop;
    uint8_t notes;
    uint8_t bins_per_octave;
    float** kernel_r;
    float** kernel_i;
    uint32_t* kernel_len;
    bool down4;
    // 输出有关
    float** output;
    uint32_t output_length;
public:
    CQT(uint16_t fs = 44100, uint16_t hop = 2205, float fmin = 32.7, uint8_t notes = 84, uint8_t bins_per_octave = 12, float filter_scale = 3):
        hop(hop), notes(notes), bins_per_octave(bins_per_octave), output(nullptr), output_length(0) {
        float Q = filter_scale / (pow(2, 1.0 / bins_per_octave) - 1);
        float fmax = fmin * pow(2, float(notes) / bins_per_octave);
        this->down4 = fs >> 3 > fmax;
        this->iniKernel(Q, this->down4 ? fs >> 2 : fs, fmin);
    }

    ~CQT() {
        this->clearOutput();
        for (uint16_t i = 0; i < this->notes; i++) {
            delete[] this->kernel_r[i];
            delete[] this->kernel_i[i];
        }
        delete[] this->kernel_r;
        delete[] this->kernel_i;
        delete[] this->kernel_len;
    }

    void clearOutput() {
        if(this->output != nullptr){
            for(uint32_t i = 0; i < this->output_length; i++)
                delete[] this->output[i];
            delete[] this->output;
        }
        this->output = nullptr;
        this->output_length = 0;
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
        uint16_t hop = this->down4 ? this->hop >> 2 : this->hop;
        for (uint8_t i = 0; i < this->notes; i++) {
            const float freq = fmin * pow(2, float(i) / this->bins_per_octave);
            uint32_t len = kernel_len[i] = ceil(Q * fs / freq);
            if (len < hop) len = hop;
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

    /**
     * @param x 输入的音频信号
     * @param length 输入的音频信号长度
     * @param output 输出地址
     * @param output_length output的长度
     * @return 本次CQT时间长度
     * 吐过output为空，则output_length和返回值一样
     * 如果output不为空，说明output已经有内容了，在后面追加，返回值是追加的长度，output_length是总长度
     */
    uint32_t _cqt(float* x, uint32_t length, float** &output, uint32_t &output_length) {
        float offset = this->hop >> 1;
        float hop = this->hop;
        if (this->down4) {
            offset /= 4;
            hop /= 4;
        }
        uint32_t _output_length = ceil(float(length - offset) / hop);
        float** _output = output;

        if (output != nullptr && output_length != 0) {
            // 说明output已经有内容了，在后面追加
            output = new float*[output_length + _output_length];
            for (uint32_t i = 0; i < output_length; i++) output[i] = _output[i];
            delete[] _output;
            _output = output + output_length;
            output_length += _output_length;
        } else {
            _output = output = new float*[_output_length];
            output_length = _output_length;
        }
        
        uint32_t pointer = 0;
        for (; offset < length; offset += hop) {
            uint32_t _offset = round(offset);
            float* energy = _output[pointer++] = new float[this->notes];
            for (uint8_t note = 0; note < this->notes; note++) {
                const float* kernel_r = this->kernel_r[note];
                const float* kernel_i = this->kernel_i[note];
                const uint32_t kernel_len = this->kernel_len[note];
                float sum_r = 0; float sum_i = 0;
                const int32_t left = _offset - (kernel_len >> 1);
                uint32_t right = length - left;
                if (right > kernel_len) right = kernel_len;
                for (uint32_t i = left >= 0 ? 0 : -left; i < right; i++) {
                    const uint32_t index = i + left;
                    if (index >= length) break;
                    sum_r += x[index] * kernel_r[i];
                    sum_i += x[index] * kernel_i[i];
                } energy[note] = sqrt(sum_r * sum_r + sum_i * sum_i) * 32;  // 和STFT保持一致
            }
        }
        return _output_length;
    }
    // 暴露给js的cqt接口
    val cqt(val input) {
        // 将输入的js数组转换为c++数组
        uint32_t length = 0;
        float* x = getFloatPtrFrom1XArray(input, length);
        if(this->down4) {   // 降采样
            filtfilt(x, length, (float*)down4_num, (float*)down4_den, 7);
            down_4(x, length);
        }
        // 计算CQT 不清空之前的结果，在后面追加
        // 返回结果中，output已经被延长或分配内存了
        uint32_t cqt_length = this->_cqt(x, length, this->output, this->output_length);
        delete[] x;
        return get2XArray(this->output + this->output_length - cqt_length, cqt_length, this->notes);
    }
};

EMSCRIPTEN_BINDINGS(module) {
    emscripten::class_<CQT>("CQT")
        .constructor<uint16_t, uint16_t, float, uint8_t, uint8_t, float>()
        .function("cqt", &CQT::cqt, emscripten::allow_raw_pointers())
        .function("clearOutput", &CQT::clearOutput, emscripten::allow_raw_pointers());
}