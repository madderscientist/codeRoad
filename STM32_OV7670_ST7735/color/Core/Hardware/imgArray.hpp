#ifndef __IMGARRAY_HPP__
#define __IMGARRAY_HPP__

#include "stdint.h"

#ifndef ABS
#define ABS(x) ((x) > 0 ? (x) : -(x))
#endif

#ifndef SWAP
#define SWAP(a, b)     \
    {                  \
        auto temp = a; \
        a = b;         \
        b = temp;      \
    }
#endif

inline uint32_t ROUND(float num) {
    return (num > 0.0) ? (num + 0.5) : (num - 0.5);
}

class ImgArray {
public:
    static uint8_t rgbBuffer[3];
    uint8_t* array;
    uint16_t width;
    uint16_t height;
    ImgArray(uint16_t W, uint16_t H, uint8_t* parray = nullptr)
        : width(W), height(H), array(parray) {}
    // 浅拷贝
    ImgArray(const ImgArray& other) {
        width = other.width;
        height = other.height;
        array = other.array;
    }
    ~ImgArray() {
        if (array != nullptr) {
            delete[] array;
            array = nullptr;
        }
    }
    inline uint32_t size() {
        return width * height;
    }
    /**
     * @brief  限制范围
     * @param  x 数值
     * @param  low 下限
     * @param  high 上限
     * @retval 限制后的数值
    */
    static inline int16_t clamp(int16_t x, uint16_t low, uint16_t high) {
        return x < low ? low : (x > high ? high : x);
    }
    /**
     * @brief  rgb888转uint16(rgb565)
     * @param  r 红色
     * @param  g 绿色
     * @param  b 蓝色
     * @retval 16位rgb565
    */
    static uint16_t rgb2uint(uint8_t r, uint8_t g, uint8_t b) {
        r = 31 * float(r) / 255 + 0.5;
        g = 63 * float(g) / 255 + 0.5;
        b = 31 * float(b) / 255 + 0.5;
        return uint16_t(r << 11) | uint16_t(g << 5) | b;
    }
    /**
     * @brief  uint16(rgb565)转rgb888
     * @param  color 16位rgb565 
     * @retval 24位rgb888数组
     */
    static uint8_t* uint2rgb(uint16_t color) {
        ImgArray::rgbBuffer[0] = 255 * float(color >> 11) / 31 + 0.5;
        ImgArray::rgbBuffer[1] = 255 * float((color & 0b11111100000) >> 5) / 63 + 0.5;
        ImgArray::rgbBuffer[2] = 255 * float(color & 0b11111) / 31 + 0.5;
        return ImgArray::rgbBuffer;
    }
    /**
     * @brief  uint16(rgb565)转灰度
     * @param  rgb565 16位rgb565
     * @retval 灰度 0~255
     * @note   Gray = round(R*0.299 + G*0.587 + B*0.114)
     *              = floor((r*255/31) * (76/256) + (g*255/63) * (150/256) + (b*255/31) * (30/256) + (128/256))
    */
    static uint8_t uint2grey(uint16_t rgb565) {
        uint8_t r = rgb565 >> 11;
        uint8_t g = (rgb565 & 0b11111100000) >> 5;
        uint8_t b = rgb565 & 0b11111;
        return (r * 625 + g * 607 + b * 247 + 128) >> 8;
    }
    /**
     * @brief  灰度转uint16(rgb565)
     * @param  grey 灰度 0~255
     * @retval 16位rgb565
    */
    static inline uint16_t grey2uint(uint8_t grey) {
        return rgb2uint(grey, grey, grey);
    }
};
uint8_t ImgArray::rgbBuffer[3] = {0};
#endif