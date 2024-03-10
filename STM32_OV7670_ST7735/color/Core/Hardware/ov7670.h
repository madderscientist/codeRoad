#ifndef OV7670_H
#define OV7670_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "i2c.h"
#include "st7735.h"

extern volatile uint8_t take_photo_flag;
void getCameraData(uint8_t*);
void ov7670_init(void);
uint8_t camera_readData(void);

#ifdef __cplusplus
}
#endif

#endif
