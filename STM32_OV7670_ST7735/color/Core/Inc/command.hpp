#ifndef __COMMAND_HPP__
#define __COMMAND_HPP__

#include "main.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"

extern UART_HandleTypeDef huart3;

#define PRINTER &huart3

static char msg[40];
void print(const char* format, ...) {
    __disable_irq();
    va_list args;
    va_start(args, format);
    vsprintf(msg, format, args);
    va_end(args);
    HAL_UART_Transmit(PRINTER, (uint8_t*)msg, strlen(msg), 100);
    __enable_irq();
}
void print_IT(const char* format, ...) {
    __disable_irq();
    va_list args;
    va_start(args, format);
    vsprintf(msg, format, args);
    va_end(args);
    HAL_UART_Transmit_IT(PRINTER, (uint8_t*)msg, strlen(msg));
    __enable_irq();
}
void print2(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsprintf(msg, format, args);
    va_end(args);
    HAL_UART_Transmit(PRINTER, (uint8_t*)msg, strlen(msg), 100);
}
void print_IT2(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vsprintf(msg, format, args);
    va_end(args);
    HAL_UART_Transmit_IT(PRINTER, (uint8_t*)msg, strlen(msg));
}
#endif