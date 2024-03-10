#ifndef __CHANNEL_HPP__
#define __CHANNEL_HPP__

#include "imgArray.hpp"

// 单通道8位图像
class Channel : public ImgArray {
private:
    uint8_t* rgb565Buffer;  // 输出到屏幕的缓冲区
public:
    Channel(uint16_t W, uint16_t H, uint8_t* parray = nullptr)
        : ImgArray(W, H, parray) {
        if (array == nullptr) {
            array = new uint8_t[width * height];
        }
        rgb565Buffer = new uint8_t[width << 1];
    }
    Channel(const Channel& other) : ImgArray(other) {
        uint32_t size = width * height;
        array = new uint8_t[size];
        for (uint16_t i = 0; i < size; i++)
            array[i] = other.array[i];
        rgb565Buffer = other.rgb565Buffer;
    }
    ~Channel() {
        if (rgb565Buffer != nullptr) {
            delete[] rgb565Buffer;
            rgb565Buffer = nullptr;
        }
    }
    void clear(uint8_t color = 0) {
        uint32_t size = width * height;
        for (uint32_t i = 0; i < size; i++) array[i] = color;
    }
    inline uint8_t getPixel(uint16_t x, uint16_t y) {
        return array[y * width + x];
    }
    inline void setPixel(uint16_t x, uint16_t y, uint8_t color = 255) {
        array[y * width + x] = color;
    }
    // 重载运算符()，使之可以用(x,y)访问并修改像素
    uint8_t& operator()(uint16_t x, uint16_t y) {
        return array[y * width + x];
    }
    const uint8_t& operator()(uint16_t x, uint16_t y) const {
        return array[y * width + x];
    }
    bool point(int32_t x, int32_t y, uint8_t color = 255) {
        if (x < 0 || x >= width || y < 0 || y >= height)
            return false;
        setPixel(x, y, color);
        return true;
    }
    /**
     * @brief  从RGB565图像中提取单通道
     * @param  rgb565: RGB565图像
     * @param  channel: 通道选择，0-R, 1-G, 3-B, 4-GRAY
    */
    void fromRGB565(const ImgArray& rgb565, uint8_t channel) {
        uint16_t w = rgb565.width > width ? width : rgb565.width;
        uint16_t h = rgb565.height > height ? height : rgb565.height;
        switch (channel) {
            case 0:  // R
                for (uint16_t i = 0; i < h; i++) {
                    uint32_t index = i * rgb565.width << 1;
                    uint32_t index_self = i * width;
                    for (uint16_t j = 0; j < w; j++) {
                        array[index_self++] = rgb565.array[index] >> 3;
                        index += 2;
                    }
                }
                break;
            case 1:  // G
                for (uint16_t i = 0; i < h; i++) {
                    uint32_t index = i * rgb565.width << 1;
                    uint32_t index_self = i * width;
                    for (uint16_t j = 0; j < w; j++) {
                        array[index_self++] = ((rgb565.array[index] & 0b111) << 3) | ((rgb565.array[index + 1] >> 5));
                        index += 2;
                    }
                }
                break;
            case 3:  // B
                for (uint16_t i = 0; i < h; i++) {
                    uint32_t index = (i * rgb565.width << 1) + 1;
                    uint32_t index_self = i * width;
                    for (uint16_t j = 0; j < w; j++) {
                        array[index_self++] = rgb565.array[index] & 0b11111;
                        index += 2;
                    }
                }
                break;
            case 4:  // GRAY
                for (uint16_t i = 0; i < h; i++) {
                    uint32_t index = i * rgb565.width << 1;
                    uint32_t index_self = i * width;
                    for (uint16_t j = 0; j < w; j++, index += 2)
                        array[index_self++] = ImgArray::uint2grey((rgb565.array[index] << 8) | rgb565.array[index + 1]);
                }
                break;
        }
    }
    /**
     * @brief  大津法获取二值化阈值
     * @retval 二值化阈值
    */
    uint8_t OTSU() {
        uint32_t histogram[256] = {0};
        uint32_t size = width * height;
        for (uint32_t i = 0; i < size; i++) {
            histogram[array[i]]++;
        }
        float sum = 0;
        for (uint32_t i = 0; i < 256; i++) {
            sum += i * histogram[i];
        }
        float sumB = 0;
        uint32_t wB = 0;
        uint32_t wF = 0;
        float varMax = 0;
        uint8_t threshold = 0;
        for (uint32_t i = 0; i < 256; i++) {
            wB += histogram[i];
            if (wB == 0) continue;
            wF = size - wB;
            if (wF == 0) break;
            sumB += i * histogram[i];
            float mB = sumB / wB;
            float mF = (sum - sumB) / wF;
            float varBetween = wB * wF * (mB - mF) * (mB - mF);
            if (varBetween > varMax) {
                varMax = varBetween;
                threshold = i;
            }
        }
        return threshold;
    }
    /**
     * @brief  二值化
     * @param  threshold: 二值化阈值
    */
    void threshold(uint8_t threshold) {
        for (uint32_t i = 0; i < size(); i++) {
            array[i] = array[i] > threshold ? 255 : 0;
        }
    }

    /**
     * @brief  卷积 改变此数组 保留原尺寸
     * @note   对堆的大小有要求！https://www.21ic.com/app/mcu/201810/781085.htm 可用堆只有设置的一半。我设置了0x300才行
     * @param  kernel: 卷积核
     * @param  kernel_size: 卷积核尺寸，指边长
     * @param  fill: 填充值
    */
    void conv2d(int8_t* kernel, uint8_t kernel_size, uint8_t fill = 0) {
        uint8_t padding = kernel_size >> 1;

        // 初始化缓冲区
        uint16_t bufferRow = width + (padding << 1);  // buffer一行的长度
        uint16_t bufferLine = padding + 1;  // buffer的行数
        uint8_t** buffer = new uint8_t*[bufferLine];  // buffer
        uint8_t* temp = nullptr;
        for (uint16_t i = 0; i < bufferLine; i++) {
            temp = new uint8_t[bufferRow];
            for (uint16_t j = 0; j < bufferRow; j++) temp[j] = fill;
            buffer[i] = temp;
        }
        buffer[0] = temp;

        uint32_t index = 0;
        for (int16_t h = 0; h < height; h++) {  // 对每一行
            //== 初始化缓冲区 ==//
            //1. 上移一行
            temp = buffer[0];
            for (uint16_t i = 1; i < bufferLine; i++) buffer[i - 1] = buffer[i];
            buffer[bufferLine - 1] = temp;
            //2. 读取新的一行
            for (uint16_t j = 0; j < width; j++) temp[j + padding] = array[index + j];

            for (uint16_t w = 0; w < width; w++) {  // 对每一个值
                int16_t sum = 0;
                uint8_t m = 0;
                uint8_t kerneli = 0;
                // 对卷积核覆盖了buffer的每一行
                for (; m < bufferLine; m++) {
                    for (uint8_t n = 0; n < kernel_size; n++, kerneli++) {
                        sum += buffer[m][w + n] * kernel[kerneli];
                    }
                }
                // 对不在buffer中的
                for (; m < kernel_size; m++) {
                    uint16_t ynow = m - padding + h;
                    bool inHeight = ynow < height;
                    for (uint8_t n = 0; n < kernel_size; n++, kerneli++) {
                        int16_t xnow = w + n - padding;
                        bool inWidth = xnow >= 0 && xnow < width;
                        if (inHeight && inWidth)
                            sum += array[ynow * width + xnow] * kernel[kerneli];
                        else
                            sum += fill * kernel[kerneli];
                    }
                }
                array[index++] = clamp(sum, 0, 255);
            }
        }
        for (uint16_t i = 0; i < bufferLine; i++) delete[] buffer[i];
        delete[] buffer;
    }
    
    //== 以下为输入到rgb565的屏幕服务 ==//
    inline uint8_t* getBuffer() {
        return rgb565Buffer;
    }
    /**
     * @brief  从数组中获取下一行RGB565数据
     * @retval 下一行的行号 返回0表示已经到底
     * @example
     *  uint8_t* buffer = drawer.getBuffer();
        ST7735_BeginWrite();
        uint8_t line = drawer.nextBuffer();
        while(line != 0) {
            HAL_SPI_Transmit(&ST7735_SPI_INSTANCE, buffer, drawer.width << 1, HAL_MAX_DELAY);
            line = drawer.nextBuffer();
        }
        ST7735_EndWrite();
    */
    uint16_t nextBuffer() {
        static uint16_t line = 0;
        static uint16_t index = 0;
        if (line >= height) {
            line = index = 0;
            return 0;
        }
        uint16_t rgbindex = 0;
        for (uint16_t i = 0; i < width; i++) {
            uint16_t gray = ImgArray::grey2uint(array[index++]);
            rgb565Buffer[rgbindex++] = gray >> 8;
            rgb565Buffer[rgbindex++] = gray & 0xff;
        }
        return ++line;
    }
};

#ifdef OV7670_H
void getCameraData_grey(uint8_t* channel) {
    __disable_irq();
    HAL_GPIO_WritePin(CAM_RRST_GPIO_Port, CAM_RRST_Pin, GPIO_PIN_RESET);  // OV7670_RRST = 0; // 开始复位读指针
    HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_RESET);  // OV7670_RCK_L;
    HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_SET);  // OV7670_RCK_H;
    HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_RESET);  // OV7670_RCK_L;
    HAL_GPIO_WritePin(CAM_RRST_GPIO_Port, CAM_RRST_Pin, GPIO_PIN_SET);  // OV7670_RRST = 1; // 复位读指针结束
    HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_SET);  // OV7670_RCK_H;
    uint16_t index = 0;
    for (uint16_t i = 0; i < 120; i++) {  // 对每一行
        for (uint16_t j = 0; j < 160; j++) {  // 对每一行的每一个像素
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_RESET);  // OV7670_RCK_L;
            uint16_t rgb565 = camera_readData() << 8;  // 读数据
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_SET);  // OV7670_RCK_H;
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_RESET);  // OV7670_RCK_L;
            rgb565 |= camera_readData();  // 读数据
            channel[index++] = ImgArray::uint2grey(rgb565);
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_SET);  // OV7670_RCK_H;
            // 跳过一个
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_RESET);  // OV7670_RCK_L;
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_SET);  // OV7670_RCK_H;
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_RESET);  // OV7670_RCK_L;
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_SET);  // OV7670_RCK_H;
        }
        for (uint16_t j = 0; j < 320; j++) {  // 跳过一行
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_RESET);  // OV7670_RCK_L;
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_SET);  // OV7670_RCK_H;
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_RESET);  // OV7670_RCK_L;
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_SET);  // OV7670_RCK_H;
        }
    }
    __enable_irq();
}
#endif

#endif