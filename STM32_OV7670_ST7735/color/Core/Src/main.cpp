/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "rtc.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "ov7670.h"
#include "st7735.h"
#include "pixmap.hpp"
#include "channel.hpp"
#include "command.hpp"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern char msg[40];
//== 用rgb565的版本 ==//
//uint8_t screen[ST7735_WIDTH * ST7735_HEIGHT * 2];   // 这个数组必须建立在flash内，不然爆栈和爆堆
//U8Drawer drawer(ST7735_WIDTH, ST7735_HEIGHT, screen);
//void drawTest() {
//    drawer.clear();
//    drawer.point(drawer.width>>1, drawer.height>>1, U8Drawer::rgb2uint(114,51,4));
//    auto rgb = U8Drawer::uint2rgb(drawer.getPixel(drawer.width>>1, drawer.height>>1));
//    sprintf(msg, "r%d,g%d,b%d", rgb[0], rgb[1], rgb[2]);
//    drawer.writeString(0,0,msg);
//    drawer.fillRect(100,40,-2000,300,U8Drawer::rgb2uint(255, 0, 0));
//    drawer.strokeCircle(25,25,30,U8Drawer::rgb2uint(0,255,255));
//    drawer.line(0, drawer.height, drawer.width, drawer.height>>1, U8Drawer::rgb2uint(170, 150, 240));
//    ST7735_WriteAll(drawer.array);
//}

//== 用单通道的版本 ==//
uint8_t screen[ST7735_WIDTH * ST7735_HEIGHT];
Channel drawer(ST7735_WIDTH, ST7735_HEIGHT, screen);

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* USER CODE BEGIN SysInit */

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_SPI2_Init();
    MX_USART3_UART_Init();
    MX_RTC_Init();
    MX_I2C1_Init();
    /* USER CODE BEGIN 2 */
    ST7735_Init();

    HAL_Delay(200);
    ov7670_init();
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    int8_t Laplacian[9] = {-1,-1,-1, -1,8,-1, -1,-1,-1};
    while (1) {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        if (take_photo_flag == 0) {
            // 使用U8Drawer的版本
//            getCameraData(screen);
//            drawer.gray();
//            drawer.strokeCircle(80,60,2,ST7735_RED);
//            auto rgb = U8Drawer::uint2rgb(drawer.getPixel(80, 60));
//            sprintf(msg, "r%d,g%d,b%d", rgb[0], rgb[1], rgb[2]);
//            drawer.writeString(0,0,msg,ST7735_MAGENTA);
//            ST7735_WriteAll(screen);
            // 使用Channel的版本
            getCameraData_grey(screen);
            //drawer.threshold(drawer.OTSU());  // 大津法二值化
            drawer.conv2d(Laplacian, 3, 127);   // 边缘提取
            uint8_t* buffer = drawer.getBuffer();
            ST7735_BeginWrite();
            uint8_t line = drawer.nextBuffer();
            while(line != 0) {
                HAL_SPI_Transmit(&ST7735_SPI_INSTANCE, buffer, drawer.width << 1, HAL_MAX_DELAY);
                line = drawer.nextBuffer();
            }
            ST7735_EndWrite();

            take_photo_flag = 1;
            HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
        }
    }
    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
  */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 168;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
  */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
