#ifndef __PIXMAP_HPP__
#define __PIXMAP_HPP__

#include "font.h"
#include "imgArray.hpp"

// 直接处理屏幕数据的类
/*
之所以不用uint16存储，是因为stm32是小端存储(低字节放在地地址)，而屏幕要求是：
array[0] = 0brrrrrggg
array[1] = 0bgggbbbbb
其实是高字节放在了地地址。
如果用uint16：
array[0] = 0brrrrrggggggbbbbb
uint8_t* p = array;
p[0] = 0bgggbbbbb(取低字节)
*/
class U8Drawer: public ImgArray {
public:
    U8Drawer(uint16_t Width, uint16_t Height, uint8_t* parray = nullptr)
        : ImgArray(Width, Height, parray){
        if (parray == nullptr) {
            array = new uint8_t[width * height << 1];  // 因为两位算一个像素
        }
    }
    U8Drawer(const U8Drawer& other): ImgArray(other) {
        array = new uint8_t[width * height << 1];
        for (uint16_t i = 0; i < width * height << 1; i++) {
            array[i] = other.array[i];
        }
    }

    /**
     * @brief  获取像素
     * @param  x x坐标
     * @param  y y坐标
     * @retval 16位rgb565
    */
    uint16_t getPixel(uint16_t x, uint16_t y) {
        uint32_t index = (y * width + x) << 1;
        return (array[index] << 8) | array[index + 1];
    }
    /**
     * @brief  设置像素 不会判断范围
     * @param  x x坐标
     * @param  y y坐标
     * @param  color 16位rgb565
    */
    void setPixel(uint16_t x, uint16_t y, uint16_t color = 0xffff) {
        uint32_t index = (y * width + x) << 1;
        array[index] = color >> 8;
        array[index + 1] = color & 0xff;
    }
    /**
     * @brief  清屏
     * @param  color 16位rgb565
    */
    void clear(uint16_t color = 0) {
        uint32_t size = width * height;
        for (uint32_t i = 0; i < size; i++) {
            array[i << 1] = color >> 8;
            array[(i << 1) + 1] = color & 0xff;
        }
    }
    /**
     * @brief  设置像素 会判断范围
     * @param  x x坐标
     * @param  y y坐标
     * @param  color 16位rgb565
     * @retval 是否设置成功
    */
    bool point(int32_t x, int32_t y, uint16_t color = 0xffff) {
        if (x >= width || y >= height || x < 0 || y < 0)
            return false;
        setPixel(x, y, color);
        return true;
    }
    /**
     * @brief  画线 参数支持负数 Bresenham's line algorithm
     * @param  x0 起点x坐标
     * @param  y0 起点y坐标
     * @param  x1 终点x坐标
     * @param  y1 终点y坐标
     * @param  color 16位rgb565
    */
    void line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t color = 0xffff) {
        int32_t dx = x1 - x0;
        if (dx < 0) dx = -dx;
        int32_t sx = x0 < x1 ? 1 : -1;
        int32_t dy = y1 - y0;
        if (dy > 0) dy = -dy;
        int32_t sy = y0 < y1 ? 1 : -1;
        int32_t err = dx + dy, e2;
        for (;;) {
            point(x0, y0, color);
            if (x0 == x1 && y0 == y1)
                break;
            e2 = err << 1;
            if (e2 >= dy) err += dy, x0 += sx;
            if (e2 <= dx) err += dx, y0 += sy;
        }
    }
    /**
     * @brief  填充矩形 参数支持负数
     * @param  x x坐标
     * @param  y y坐标
     * @param  w 宽度
     * @param  h 高度
     * @param  color 16位rgb565
    */
    void fillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint16_t color = 0xffff) {
        // 首先确定裁剪后的矩形
        int32_t x1 = x + w;
        int32_t y1 = y + h;
        x = clamp(x, 0, width - 1);
        y = clamp(y, 0, height - 1);
        x1 = clamp(x1, 0, width - 1);
        y1 = clamp(y1, 0, height - 1);
        if (x > x1) SWAP(x, x1);
        if (y > y1) SWAP(y, y1);
        for (uint16_t h = y; h < y1; h++) {
            uint32_t index = (h * width + x) << 1;
            for (uint16_t w = x; w < x1; w++) {
                array[index++] = color >> 8;
                array[index++] = color & 0xff;
            }
        }
    }
    /**
     * @brief  画空心圆形 参数支持负数 Bresenham's line algorithm
     * @param  x0 圆心x坐标
     * @param  y0 圆心y坐标
     * @param  r 半径
     * @param  color 16位rgb565
    */
    void strokeCircle(int32_t x0, int32_t y0, int32_t r, uint16_t color = 0xffff) {
        int32_t x = -r, y = 0, err = 2 - (r << 1);
        do {
            // 只关心一个象限。其余的三个象限可以通过对称性得到
            point(x0 - x, y0 + y, color);
            point(x0 - y, y0 - x, color);
            point(x0 + x, y0 - y, color);
            point(x0 + y, y0 + x, color);
            r = err;
            if (r <= y)
                err += (++y << 1) + 1;
            if (r > x || err > y)
                err += (++x << 1) + 1;
        } while (x < 0);
    }
    /**
     * @brief  深剪贴矩形 参数支持负数 若宽高为负数，会导致图像翻转
     * @param  x x坐标
     * @param  y y坐标
     * @param  w 宽度
     * @param  h 高度
     * @retval 剪贴后的U8Drawer 是深构造
    */
    U8Drawer cilp(int32_t x, int32_t y, int32_t w, int32_t h) {
        int8_t stepx = w > 0 ? 1 : -1;
        int8_t stepy = h > 0 ? 1 : -1;
        uint16_t xbegin = clamp(x, 0, width - 1);
        uint16_t ybegin = clamp(y, 0, height - 1);
        uint16_t xend = clamp(x + w, 0, width - 1);
        uint16_t yend = clamp(y + h, 0, height - 1);
        w = ABS(int32_t(xend - xbegin));
        h = ABS(int32_t(yend - ybegin));
        uint8_t* parray = new uint8_t[w * h << 1];
        uint32_t pindex = 0;
        for (uint16_t i = ybegin; i != yend; i += stepy) {
            for (uint16_t j = xbegin; j != xend; j += stepx) {
                uint32_t index = (i * width + j) << 1;
                parray[pindex++] = array[index++];
                parray[pindex++] = array[index];
            }
        }
        return U8Drawer(w, h, parray);
    }
    /**
     * @brief  粘贴矩形 不支持负数
     * @param  x 粘贴位置左上角的x坐标
     * @param  y 粘贴位置左上角的y坐标
     * @param  drawer 要粘贴的U8Drawer
     * @param  mapx 粘贴的U8Drawer的左上角x坐标
     * @param  mapy 粘贴的U8Drawer的左上角y坐标
     * @param  w 粘贴的宽度
     * @param  h 粘贴的高度
    */
    void paste(uint16_t x, uint16_t y, U8Drawer&& drawer, uint16_t mapx = 0, uint16_t mapy = 0, uint16_t w = 0, uint16_t h = 0) {
        if (w == 0 || w > drawer.width)
            w = drawer.width;
        if (h == 0 || h > drawer.height)
            h = drawer.height;
        uint16_t xend = x + w;
        uint16_t yend = y + h;
        if (xend > width) xend = width;
        if (yend > height) yend = height;
        for (uint16_t i = y; i < yend; i++) {
            uint32_t index = (i * width + x) << 1;
            uint32_t index2 = ((i - y + mapy) * drawer.width + mapx) << 1;
            for (uint16_t j = x; j < xend; j++) {
                array[index++] = drawer.array[index2++];
                array[index++] = drawer.array[index2++];
            }
        }
    }
    /**
     * @brief  使用oled取的模绘制图像 做了防溢出
     * 规则：每个uint8表示一列，前w个uint8表示第一大行的w列，低字节在左上角
     * 比如[0]&1表示左上角第一个，[1]&0x80表示第2列第8行的像素，[w]&2表示第一列第10个像素
     * @param x 左上角的x坐标
     * @param y 左上角的y坐标
     * @param mask 取模结果
     * @param w 宽度，几列
     * @param h 高度有几个8像素
     * @param color 16位rgb565
     */
    void mask(uint16_t x, uint16_t y, uint8_t* mask, uint16_t w, uint16_t h, uint16_t color = 0xffff) {
        if (x + w > width) w = width - x;
        h = h << 3;  // h变为像素数
        if (y + h > height) h = height - y;
        uint16_t maskindex = 0;
        uint16_t index1 = y * width + x;
        for (uint16_t row = 0; row < h; row += 8) {  // 每一大行
            // 每个字节能读几位
            uint8_t safeH = h - row;
            if (safeH > 8) safeH = 8;
            for (uint16_t col = 0; col < w; col++) {  // 每一列(每个字符)
                uint16_t index2 = (index1 + col) << 1;
                uint8_t maskbyte = mask[maskindex++];
                for (uint8_t bit = 0; bit < safeH; bit++) {  // 每一小行
                    if (maskbyte & (1 << bit)) {
                        array[index2] = color >> 8;
                        array[index2 + 1] = color & 0xff;
                    }
                    index2 += width << 1;
                }
            }
            index1 += width << 3;
        }
    }
    /**
     * @brief  画字符 使用uint8_y[16]的字模 一个字符宽8像素，高16像素
     * @param  x 左上角的x坐标
     * @param  y 左上角的y坐标
     * @param  c 字符
     * @param  color 16位rgb565
    */
    void writeChar(uint16_t x, uint16_t y, char c, uint16_t color = 0xffff) {
        mask(x, y, (uint8_t*)LCD_F8x16[c - ' '], 8, 2, color);
    }
    void writeString(uint16_t x, uint16_t y, const char* str, uint16_t color = 0xffff) {
        for (uint16_t i = 0; str[i] != '\0'; i++) {
            writeChar(x + i * 8, y, str[i], color);
        }
    }
    /**
     * @brief  画数字
     * @param  x 左上角的x坐标
     * @param  y 左上角的y坐标
     * @param  num 数字
     * @param  size 位数 如果是非正数则使用最小位数
     * @param  color 16位rgb565
    */
    void writeNumber(uint16_t x, uint16_t y, uint32_t num, int8_t size = 0, uint16_t color = 0xffff) {
        if (size <= 0) {
            size = 1;
            uint32_t temp = num;
            while (temp /= 10) size++;
        }
        for (int8_t i = size - 1; i >= 0; i--) {
            writeChar(x + (i << 3), y, num % 10 + '0', color);
            num /= 10;
        }
    }

    void gray() {
        uint32_t size = width * height << 1;
        for (uint32_t i = 0; i < size; i += 2) {
            uint16_t grey = ImgArray::grey2uint(ImgArray::uint2grey((array[i] << 8) | array[i + 1]));
            array[i] = grey >> 8;
            array[i + 1] = grey & 0xff;
        }
    }
};
#endif