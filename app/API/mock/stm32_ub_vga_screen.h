#ifndef STM32_UB_VGA_SCREEN_H
#define STM32_UB_VGA_SCREEN_H

#include <stdint.h>

#define VGA_DISPLAY_X 320
#define VGA_DISPLAY_Y 240

extern uint8_t VGA_RAM1[(VGA_DISPLAY_X + 1) * VGA_DISPLAY_Y];

void UB_VGA_Screen_Init(void);

#endif