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
#include "stm32_ub_vga_screen.h"
#include <math.h>

#define BUFFER_SIZE 128
// #define DEBUG_UART

uint8_t test_array[100];
uint8_t rx_buff[BUFFER_SIZE];
int i = 0;
uint8_t eventflagUART = 0;

extern CHAL_UART_HandleTypeDef CHAL_UART2;
extern CHAL_DMA_handler CHAL_DMA2_Stream5;
extern CHAL_DMA_Stream_TypeDef stream5;

int main(void)
{
    SystemInit(); // System speed to 168MHz

    UB_VGA_Screen_Init(); // Init VGA-Screen

    CHAL_init_uart();

    CHAL_DMA_Init();

    CHAL_DMA_config((uint32_t)&USART2->DR, (uint32_t)rx_buff, ARRAY_LEN(rx_buff));

    UB_VGA_FillScreen(VGA_COL_WHITE); // Greyhhhhh

    while (1)
    {
        if ((eventflagUART == 1) && (((VGA.hsync_cnt < VGA_VSYNC_BILD_START - TIMING_PADDING) || (VGA.hsync_cnt > VGA_VSYNC_BILD_STOP + TIMING_PADDING))))
        {
            CHAL_event_call_back(rx_buff, BUFFER_SIZE);
            eventflagUART = 0;
        }

#ifdef DEBUG_UART
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