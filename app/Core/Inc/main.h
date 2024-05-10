/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

    /* Private includes ----------------------------------------------------------*/
    /* USER CODE BEGIN Includes */

    /* USER CODE END Includes */

    /* Exported types ------------------------------------------------------------*/
    /* USER CODE BEGIN ET */

    /* USER CODE END ET */

    /* Exported constants --------------------------------------------------------*/
    /* USER CODE BEGIN EC */

    /* USER CODE END EC */

    /* Exported macro ------------------------------------------------------------*/
    /* USER CODE BEGIN EM */

    /* USER CODE END EM */

    void HAL_TIM_MspPostInit(TIM_HandleTypeDef* htim);

    /* Exported functions prototypes ---------------------------------------------*/
    void Error_Handler(void);

    /* USER CODE BEGIN EFP */

    /* USER CODE END EFP */

    /* Private defines -----------------------------------------------------------*/
    /* USER CODE BEGIN Private defines */
    // Used:

//--------------------------------------------------------------
// color designation
// 8bit color (R3G3B2)
// Red   (3bit) -> Bit7-Bit5
// Green (3bit) -> Bit4-Bit2
// Blue  (2bit) -> Bit1-Bit0
//--------------------------------------------------------------
#define VGA_COL_BLACK 0x00
#define VGA_COL_BLUE 0x03
#define VGA_COL_GREEN 0x1C
#define VGA_COL_RED 0xE0
#define VGA_COL_WHITE 0xFF

#define VGA_COL_CYAN 0x1F
#define VGA_COL_MAGENTA 0xE3
#define VGA_COL_YELLOW 0xFC

    typedef struct
    {
        uint16_t hsync_cnt; // counter
        uint32_t start_adr; // start_adres
        uint32_t dma2_cr_reg; // Register constant CR-Register
    } VGA_t;
    extern VGA_t VGA;

//--------------------------------------------------------------
// define the VGA_display
//--------------------------------------------------------------
#define VGA_DISPLAY_X 320
#define VGA_DISPLAY_Y 240
#define RAM_SIZE (VGA_DISPLAY_X + 1) * VGA_DISPLAY_Y

    extern uint8_t VGA_RAM[RAM_SIZE];

//--------------------------------------------------------------
// VSync-Signal
// Trigger   = Timer2 Update (f=31,48kHz => T = 31,76us)
// 1TIC      = 31,76us
//--------------------------------------------------------------
#define VGA_VSYNC_PERIODE 525
#define VGA_VSYNC_IMP 2
#define VGA_VSYNC_BILD_START 36
#define VGA_VSYNC_BILD_STOP 514

    // Sortof used:
    //--------------------------------------------------------------
    //  Timer-1
    //  Function  = Pixelclock (Speed for  DMA Transfer)
    //
    //  basefreq = 2*APB2 (APB2=84MHz) => TIM_CLK=168MHz
    //  Frq       = 168MHz/1/12 = 14MHz
    //
    //--------------------------------------------------------------
#define VGA_TIM1_PERIODE 11
#define VGA_TIM1_PRESCALE 0

//--------------------------------------------------------------
// Timer-2
// Function  = CH4 : HSync-Signal on PB11
//             CH3 : Trigger point for DMA start
//
// basefreq = 2*APB1 (APB1=48MHz) => TIM_CLK=84MHz
// Frq       = 84MHz/1/2668 = 31,48kHz => T = 31,76us
// 1TIC      = 11,90ns
//
//--------------------------------------------------------------
#define VGA_TIM2_HSYNC_PERIODE 2667
#define VGA_TIM2_HSYNC_PRESCALE 0

#define VGA_TIM2_HSYNC_IMP 320 // HSync-length (3,81us)
#define VGA_TIM2_HTRIGGER_START 480 // HSync+BackPorch (5,71us)
#define VGA_TIM2_DMA_DELAY 60 // ease the delay when DMA START (Optimization = none)
    // #define  VGA_TIM2_DMA_DELAY        30  // ease the delay when DMA START (Optimization = -O1)

    // Not used:
#define VGA_GPIOE_BASE_ADR ((uint32_t)0x40021000) // ADR from Port-E
#define VGA_GPIO_ODR_OFFSET ((uint32_t)0x00000014) // ADR from Register ODR
#define VGA_GPIO_BYTE_OFFSET ((uint32_t)0x00000001) // Data for 8bit
#define VGA_GPIOE_ODR_ADDRESS (VGA_GPIOE_BASE_ADR | VGA_GPIO_ODR_OFFSET | VGA_GPIO_BYTE_OFFSET)
#define VGA_GPIO_HINIBBLE ((uint16_t)0xFF00) // GPIO_Pin_8 to GPIO_Pin_15

    /* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
