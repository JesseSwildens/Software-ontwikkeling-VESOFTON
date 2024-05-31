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
#include "bitmap.h"
#include "bitmap_calib_large.h"
#include "bitmap_dvd.h"
#include "stm32_ub_vga_screen.h"
#include <math.h>

#include "API_simple_shapes.h"

#define BUFFER_SIZE 128
// #define DEBUG_UART

uint8_t rx_buff[BUFFER_SIZE];
uint8_t eventflagUART = 0;

int main(void)
{
    SystemInit(); // System speed to 168MHz

    UB_VGA_Screen_Init(); // Init VGA-Screen

    CHAL_init_uart();

    CHAL_DMA_Init();

    CHAL_DMA_config((uint32_t)&USART2->DR, (uint32_t)rx_buff, ARRAY_LEN(rx_buff));

    API_clearscreen(VGA_COL_BLACK);
    // UB_VGA_DrawBitmapWithBackground(VGA_COL_GREEN, (unsigned char*)bitmap_calib_large, 240, 240, 0, 0);

    API_draw_circle(100, 200, 20, VGA_COL_YELLOW, 0);
    API_draw_circle(150, 200, 20, VGA_COL_CYAN, 1);

    API_draw_text(100, 0, VGA_COL_GREEN, "Hello C++ enjoyers!\nThis is Working!!!!", "FreeSerif", 1, 0, 0);
    API_draw_text(50, 100, VGA_COL_RED, "Wrapping also works!", "FreeMono", 3, 2, 0);

    while (1)
    {
        if ((((VGA.hsync_cnt < VGA_VSYNC_BILD_START - TIMING_PADDING) || (VGA.hsync_cnt > VGA_VSYNC_BILD_STOP + TIMING_PADDING))))
        {
            if (eventflagUART == 1)
            {
                ASMCHAL_event_call_back(rx_buff, BUFFER_SIZE);
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
