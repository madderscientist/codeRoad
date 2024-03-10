#ifndef ST7735_H
#define ST7735_H

#ifdef __cplusplus
extern "C" {
#endif


#include "main.h"
#include "spi.h"

#define ST7735_RST_Pin SPI2_RST_Pin
#define ST7735_RST_GPIO_Port SPI2_RST_GPIO_Port
#define ST7735_DC_Pin SPI2_DC_Pin
#define ST7735_DC_GPIO_Port SPI2_DC_GPIO_Port
#define ST7735_CS_Pin SPI2_CS_Pin
#define ST7735_CS_GPIO_Port SPI2_CS_GPIO_Port

#define ST7735_SPI_INSTANCE hspi2

#define ST7735_XSTART 0
#define ST7735_YSTART 0
#define ST7735_WIDTH 160
//#define ST7735_HEIGHT 128 // 实际值
#define ST7735_HEIGHT 120   // 相机的宽度

#define ST7735_ROTATION 1

// Color definitions
#define ST7735_BLACK 0x0000
#define ST7735_BLUE 0x001F
#define ST7735_RED 0xF800
#define ST7735_GREEN 0x07E0
#define ST7735_CYAN 0x07FF
#define ST7735_MAGENTA 0xF81F
#define ST7735_YELLOW 0xFFE0
#define ST7735_WHITE 0xFFFF
#define ST7735_COLOR565(r, g, b) (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3))

void ST7735_Init(void);
void ST7735_WriteAll(uint8_t*);
void ST7735_BeginWrite(void);
void ST7735_EndWrite(void);

#ifdef __cplusplus
}
#endif

#endif