/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */
/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
void Derived_HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim2;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */ 
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */
	// Interrupt of Timer2 CH3 occurred (for Trigger start)

  /* USER CODE END TIM2_IRQn 0 */
	//Clearing the interrupt flags takes some time.
	//First clear them so by the time of the next interrupt the flags are cleared
//  HAL_TIM_IRQHandler(&htim2);
	Derived_HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */

	VGA.hsync_cnt++;
	if(VGA.hsync_cnt >= VGA_VSYNC_PERIODE)
	{
		VGA.hsync_cnt = 0;
		VGA.start_adr = (uint32_t)(&VGA_RAM[0]);
	}

	if(VGA.hsync_cnt < VGA_VSYNC_IMP)
	{
		// HSync low
		GPIOB->BSRR = (uint32_t)GPIO_PIN_12 << 16U; //The BSSR register is a quick way of setting the GPIO pins HIGH/LOW
//		GPIOB->ODR &= ~(1 << 12); //The alternative to BSSR, but just a little slower
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	}
	else
	{
		// HSync High
		GPIOB->BSRR = (uint32_t)GPIO_PIN_12;
//		GPIOB->ODR |= 1 << 12;
//		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
	}
	if((VGA.hsync_cnt>=VGA_VSYNC_BILD_START) && (VGA.hsync_cnt<=VGA_VSYNC_BILD_STOP)){
		// DMA2 init
		DMA2_Stream5->CR=VGA.dma2_cr_reg;
	    // set adress
	    DMA2_Stream5->M0AR=VGA.start_adr;
	    // Timer1 start
	    TIM1->CR1|= TIM_CR1_CEN;
	    // DMA2 enable
	    DMA2_Stream5->CR|=DMA_SxCR_EN;

	    // Test Adrespointer for high
	    if((VGA.hsync_cnt & 0x01)!=0) {
	      // inc after Hsync
	      VGA.start_adr+=(VGA_DISPLAY_X+1);
	    }
	}
  /* USER CODE END TIM2_IRQn 1 */
}

/* USER CODE BEGIN 1 */

void Derived_HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim)
{
/*
 * The default HAL_TIM_IRQHandler has a lot of if statements that are not used by us.
 * The code that the HAL_TIM_IRQHandler actually uses is given below
 */
	__HAL_TIM_CLEAR_IT(htim, TIM_IT_CC3);
	htim->Channel = HAL_TIM_ACTIVE_CHANNEL_3;
	//These next three commands are also executed by HAL_TIM_IRQHandler, but they can be skipped
	//	HAL_TIM_OC_DelayElapsedCallback(htim);
	//	HAL_TIM_PWM_PulseFinishedCallback(htim);
	//	htim->Channel = HAL_TIM_ACTIVE_CHANNEL_CLEARED;
}

void DMA2_Stream5_IRQHandler(void)
{
	// All the pixels have been sent to the screen.
    // TransferInterruptComplete Interrupt from DMA2
    // DMA_ClearITPendingBit(DMA2_Stream5, DMA_IT_TCIF5);
	// Clear the Pending bit in the DMA high interrupt flag clear register
	DMA2->HIFCR = (uint32_t)(0x20008800 & 0x0F7D0F7D);

    // Timer1 stop
    TIM1->CR1 &= ~TIM_CR1_CEN;
    // DMA2 disable
    DMA2_Stream5->CR=0;
    // switch on black

    GPIOE->ODR = 0x0000; //Set all the GPIO pins low
    //The alternatives to this command above should be more readable/quicker, but they tend to give the screen a "green tint"
//	GPIOE->ODR = VGA_GPIO_HINIBBLE; //Set all the GPIO pins low
//	GPIOE->BSRR = VGA_GPIO_HINIBBLE;
}

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
