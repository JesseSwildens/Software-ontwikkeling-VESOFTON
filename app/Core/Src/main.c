/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
 * Datum    : 13-04-2020
 * Version  : 1.0
 * Autor    : UB
 * mods by	 : J.F. van der Bent, Ömer Yildiz, Martin Hardeveld
 * CPU      : STM32F4 (STM32F407VG)
 * IDE      : STM32CubeIDE Version: 1.3.0 Build: 5720_20200220_1053 (UTC)
 * Module   : HAL
 * Function : VGA_core DMA LIB 320x240, 8bit color
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "screen.h"
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
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
VGA_t VGA;
uint8_t VGA_RAM[RAM_SIZE];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
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
    VGA.dma2_cr_reg = 0;
    VGA.hsync_cnt = 0;
    VGA.start_adr = 0;

    uint16_t xp, yp;

    // RAM init total white
    for (yp = 0; yp < VGA_DISPLAY_Y; yp++)
    {
        for (xp = 0; xp < (VGA_DISPLAY_X + 1); xp++)
        {
            VGA_RAM[(yp * (VGA_DISPLAY_X + 1)) + xp] = VGA_COL_BLACK;
        }
    }
    //  //Blue line on screen
    for (yp = 0; yp < VGA_DISPLAY_Y; yp++)
    {
        VGA_RAM[(yp * (VGA_DISPLAY_X + 1)) + xp] = VGA_COL_BLUE;
        xp++;
    }

    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_TIM2_Init();
    /* USER CODE BEGIN 2 */

    // Enable TIM1 clock
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

    // Set the period to 11
    TIM1->ARR = 11; // Set to the required period - 1, in this case 11 (0xb)

    // Hierin verschilt mijn code een beetje van de originele
    // TIM1->SR register heeft de UIF bit set op deze plek in de originele code, niet in die van mij. ik krijg het niet enabled
    // Set the Status Register UpdateInterruptFlag
    TIM1->SR |= TIM_SR_UIF; // Same as (TIM1->SR = 0x01;) //Doet niets

    // Set the DMA AddressForFullTransfer register
    TIM1->DMAR |= TIM_DMAR_DMAB; // Same as (TIM1->DMAR = 0x80)

    // Set AutoReloadPreload register
    //  TIM1->CR1 |= TIM_CR1_ARPE; //Same as (TIM1->CR1 = 0x80;)
    TIM1->CR1 = 0x80;

    // Init DMA
    // Enable DMA2 clock
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    /*
     * The reset state of the bits are 0x0.
     * If the bit is not set, it is 0x0
     * For more info on the configuration fo the DMA see: https://community.st.com/s/feed/0D53W000003yufE
     *
     * Channel select: Channel 6
     * Memory burst: off (reset state)
     * Peripheral Burst: off (reset state)
     * Curent target: Memory Zero (only used in double buffer mode) (reset state)
     * Double buffer mode: off (reset state)
     * Priority Level: Very High
     * Peripheral increment offset size: off (The offset size for the peripheral address calculation is linked to the PSIZE) (reset state)
     * Memory size: 0x00 (byte) (also reset state, but still configured for good measure)
     * Peripheral size: 0x00 (byte)
     * Memory increment mode: True, 0x1
     * Peripheral increment mode: False, 0x0 (reset state)
     * Circular mode: disabled (reset state)
     * Data transfer direction: Memory to peripheral, 0x1
     * Peripheral Flow controller: DMA is the flow controller, 0x0 (reset state)
     * Transfer complete interrupt: disabled (enabled in TIM2 channel 3 interrupt, disabled in DMA transfer complete interrupt)
     * Half transfer interrupt: disabled (reset state)
     * Transfer error interrupt: disabled (reset state)
     * Direct mode error interrupt: disabled (reset state)
     * Stream enable: disabled, to configure some registers this must be disabled (reset state)
     */

    DMA2_Stream5->CR = (6u << DMA_SxCR_CHSEL_Pos) | // select channel 6, see DMA request mapping in the reference manual
        DMA_SxCR_MINC | // memory increment enable
        DMA_SxCR_DIR_0 | // memory to peripheral
        (0x3UL << DMA_SxCR_PL_Pos); // Set to highest priority

    DMA2_Stream5->NDTR = VGA_DISPLAY_X + 1; // 320 + 1 //Send one horizontal line at a time

    // Set the Output Data Register to 0, result: all the GPIOE pins are set LOW
    GPIOE->ODR = 0x0;

    // This is the address of the GPIOE Output Data Register.
    DMA2_Stream5->PAR = (uint32_t)(&GPIOE->ODR + 1);

    // Give the address of the frame buffer. The contents of this array will be pushed onto the GPIO pins.
    // The NDTR register tells how much data of the array should be sent every time
    DMA2_Stream5->M0AR = (uint32_t)&VGA_RAM;

    // TIM1 DMA/Interrupt enable register: Set the UpdateDMARequestEnable bit
    TIM1->DIER |= TIM_DIER_UDE; // UDE = 0x1

    /*
     * Set NVIC settings
     * See "ST Programmaing Manual PM0214" section 4.3 and the "STM32F407VG Referance Manual" for more info on setting the correct
     * NVIC registers.
     * The values were found by stepping through the registers with a debugger on the old code.
     */

    DMA2_Stream5->CR |= DMA_SxCR_TCIE; // Enabe DMA2 stream 5 channel 6 TransferCompleteInterrup
    NVIC->ICER[2] = (uint32_t)16; // Set the Interrupt Clear enable Register to 0x16
    NVIC->IP[68] = 0x0; // Set the Interrupt priority Register to 0x0
    NVIC->ISER[2] = (uint32_t)16; // Set the Interrupt Set Enale Register to 0x16

    /*
     * Store the DMA Control Register configuration.
     * The DMA CR is cleared in the DMA interrupt to quickly stop it.
     * In the TIM2 interrupt it needs to be reconfigured, it uses the stored settings for it
     */
    VGA.dma2_cr_reg = DMA2_Stream5->CR;

    HAL_TIM_OC_Start(&htim2, TIM_CHANNEL_4); //  Start H-Sync PWM, this always runs in the background

    HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_3); //  Start V-sync interrupt and DMA_enabling

    screen();
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1)
    {

        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the CPU, AHB and APB busses clocks
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 168;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 4;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB busses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
        | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void)
{

    /* USER CODE BEGIN TIM2_Init 0 */

    /* USER CODE END TIM2_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
    TIM_MasterConfigTypeDef sMasterConfig = { 0 };
    TIM_OC_InitTypeDef sConfigOC = { 0 };

    /* USER CODE BEGIN TIM2_Init 1 */

    /* USER CODE END TIM2_Init 1 */
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 0;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 2667;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 420;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        Error_Handler();
    }
    sConfigOC.Pulse = 320;
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN TIM2_Init 2 */

    /* USER CODE END TIM2_Init 2 */
    HAL_TIM_MspPostInit(&htim2);
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = { 0 };

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOH_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

    /*Configure GPIO pins : PE8 PE9 PE10 PE11
                             PE12 PE13 PE14 PE15 */
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11
        | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /*Configure GPIO pin : PB12 */
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */

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
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
