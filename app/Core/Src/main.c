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

uint8_t test_array[100];
int i = 0;

int main(void)
{
    CHAL_init_uart();

    SystemInit(); // System speed to 168MHz

    UB_VGA_Screen_Init(); // Init VGA-Screen

    UB_VGA_FillScreen(0x49); // Greyhhhhh

    while (1)
    {
        uint8_t test = CHAL_UART2_get_char();
        CHAL_UART2_SendChar(test);
        test_array[i] = test;
        i++;
    }
}
