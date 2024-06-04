//--------------------------------------------------------------
// File     : main.c
// Datum    : 30.03.2016
// Version  : 1.0
// Autor    : UB
// mods by	: J.F. van der Bent
// CPU      : STM32F4
// IDE      : CooCox CoIDE 1.7.x
// Module   : CMSIS_BOOT, M4_CMSIS_CORE
// Function : VGA_core DMA LIB 320x240, 8bit color
//--------------------------------------------------------------

#include "main.h"

#include "API_graphics.h"
#include "BL_parser.h"

#include <math.h>

#define BUFFER_SIZE 4096
// #define DEBUG_UART

uint8_t rx_buff[BUFFER_SIZE] __attribute__((section(".dma_mem")));
uint8_t eventflagUART = 0;

int main(void)
{
    API_Init();

    API_InitTick();

    CHAL_init_uart();

    CHAL_DMA_Init();

    CHAL_DMA_config((uint32_t)&USART2->DR, (uint32_t)rx_buff, ARRAY_LEN(rx_buff));

    UB_VGA_FillScreen(VGA_COL_WHITE); // Greyhhhhh

    while (1)
    {
        if ((((VGA.hsync_cnt < VGA_VSYNC_BILD_START - TIMING_PADDING) || (VGA.hsync_cnt > VGA_VSYNC_BILD_STOP + TIMING_PADDING))))
        {
            if (eventflagUART == 1)
            {
                CHAL_push_to_q(rx_buff, BUFFER_SIZE);
                BL_main_parser();
                eventflagUART = 0;
            }
        }

#ifdef DEBUG_UART1
        uint8_t test
            = CHAL_UART2_get_char();
        CHAL_UART2_SendChar(test);
#endif
    }
}

void DMA1_Stream5_IRQHandler(void)
{
    USART2->CR3 |= (0 << 6);
    DMA1->HIFCR = DMA_Stream5_IT_MASK;
    USART2->CR3 |= (1 << 6);
}

void USART2_IRQHandler(void)
{
    CHAL_clear_idledetect();
    eventflagUART = 1;
}

/**
 * @brief Systick interrupt handler
 */
void SysTick_Handler(void)
{
    Tick++;
}