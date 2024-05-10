/*
 * screen.h
 *
 *  Created on: 11 apr. 2020
 *      Author: Ömer
 */

#ifndef INC_SCREEN_H_
#define INC_SCREEN_H_

void screen(void);

#include "main.h"
#include "stdlib.h"
// #include "screen.c"
// #include "images.h"

// #include "stm32f4xx.h"
// #include "stm32f4xx_rcc.h"
// #include "stm32f4xx_gpio.h"
// GPIO_InitTypeDef  GPIO_InitStruct;

#define sgn(x) ((x < 0) ? -1 : ((x > 0) ? 1 : 0)) /* macro to return the sign of a \
                                                     number */
typedef unsigned char byte;
typedef unsigned short word;

void line(int x1, int y1, int x2, int y2, byte color);
void polygon(int num_vertices, int* vertices, byte color);
void rechthoek(int LinksBoven[2], int RechtsOnder[2], byte color);
void vierkantM(int middelpuntX, int middelpuntY, int lengte, byte color);
void kubus(int Xmiddelpunt, int Ymiddelpunt, int lengte, byte color);
void hoek(int LinksBoven[2], int RechtsOnder[2], int Mx, int My, int lengte);
void kubusTest(int Xmiddelpunt, int Ymiddelpunt, int lengte, int Xdraai, byte color);

void drawCircle(int xc, int yc, int x, int y, byte color);
void circleBres(int xc, int yc, int r, byte color);

void FillScreen(uint8_t color);
void SetPixel(uint16_t xp, uint16_t yp, uint8_t color);

void screen(void);

// void initButton(void);
// unsigned char buttonPressed(void);

#endif /* INC_SCREEN_H_ */
