/*
 * screen.c
 *
 *  Created on: 11 apr. 2020
 *      Author: Ömer
 */

// #include "stm32_ub_vga_screen.h"
#include "screen.h"

void screen(void)
{
    //	initButton();

    FillScreen(VGA_COL_BLACK);
    SetPixel((VGA_DISPLAY_X / 2), (VGA_DISPLAY_Y / 2), VGA_COL_GREEN);
    //	int draai = 0;
    //	int move = 0;

    kubus((VGA_DISPLAY_X / 2), (VGA_DISPLAY_Y / 2), (VGA_DISPLAY_X / 4), VGA_COL_RED);

    //	int i = 0;
    HAL_Delay(1000);

    FillScreen(VGA_COL_BLACK);

    //	circleBres((VGA_DISPLAY_X/2), (VGA_DISPLAY_Y/2), (VGA_DISPLAY_X/4), VGA_COL_RED);
    //	circleBres((VGA_DISPLAY_X/2), (VGA_DISPLAY_Y/2), (VGA_DISPLAY_X/4), VGA_COL_BLACK);

    while (1)
    {
        //		HAL_Delay(3000);
        for (int i = VGA_DISPLAY_X / 4; i >= 0; i--)
        {
            circleBres((VGA_DISPLAY_X / 2), (VGA_DISPLAY_Y / 2), i, VGA_COL_RED);
            circleBres((VGA_DISPLAY_X / 2), (VGA_DISPLAY_Y / 2), i + 1, VGA_COL_BLACK);
            HAL_Delay(10);
        }
        FillScreen(VGA_COL_BLACK);

        //	HAL_Delay(1000);
        for (int i = 0 / 4; i <= VGA_DISPLAY_X / 4; i++)
        {
            circleBres((VGA_DISPLAY_X / 2), (VGA_DISPLAY_Y / 2), i, VGA_COL_RED);
            circleBres((VGA_DISPLAY_X / 2), (VGA_DISPLAY_Y / 2), i - 1, VGA_COL_BLACK);
            HAL_Delay(10);
        }
    }
}

void line(int x1, int y1, int x2, int y2, byte color)
{
    /*
     * Bron: 	 	http://www.brackeen.com/vga/source/djgpp20/lines.c.html
     * Hoofdbron: 	http://www.brackeen.com/vga/shapes.html
     * Hoofdbron2:	https://rosettacode.org/wiki/Bitmap/Bresenham%27s_line_algorithm#C
     */

    int dx = x2 - x1;
    dx = abs(dx);
    int sx = x1 < x2 ? 1 : -1;
    int dy = y2 - y1;
    dy = abs(dy);
    int sy = y1 < y2 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    for (;;)
    {
        SetPixel(x1, y1, color);
        if (x1 == x2 && y1 == y2)
            break;
        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            y1 += sy;
        }
    }
}

void polygon(int num_vertices, int* vertices, byte color)
{
    int i;

    for (i = 0; i < num_vertices - 1; i++)
    {
        line(vertices[(i << 1) + 0],
            vertices[(i << 1) + 1],
            vertices[(i << 1) + 2],
            vertices[(i << 1) + 3],
            color);
    }
    line(vertices[0],
        vertices[1],
        vertices[(num_vertices << 1) - 2],
        vertices[(num_vertices << 1) - 1],
        color);
}

// Algemene niet ingevulde rechthoek
void rechthoek(int LinksBoven[2], int RechtsOnder[2], byte color)
{
    int i;

    for (i = LinksBoven[0]; i <= RechtsOnder[0]; i++)
    {
        SetPixel(i, LinksBoven[1], color); // horizontaal boven
        SetPixel(i, RechtsOnder[1], color); // horizontaal onder
    }

    for (i = LinksBoven[1]; i <= RechtsOnder[1]; i++)
    {
        SetPixel(LinksBoven[0], i, color); // verticaal links
        SetPixel(RechtsOnder[0], i, color); // verticaal rechts
    }
}

// Tekent een vierkant m.b.v. een middelpunt en de lengte van de ribben
void vierkantM(int middelpuntX, int middelpuntY, int lengte, byte color)
{
    int LinksBoven[] = { (middelpuntX - (lengte / 2)), (middelpuntY - (lengte / 2)) };
    int RechtsOnder[] = { (middelpuntX + (lengte / 2)), (middelpuntY + (lengte / 2)) };
    rechthoek(LinksBoven, RechtsOnder, color);
}

void hoek(int LinksBoven[2], int RechtsOnder[2], int Mx, int My, int lengte)
{
    LinksBoven[0] = Mx - (lengte / 2);
    LinksBoven[1] = My - (lengte / 2);

    RechtsOnder[0] = Mx + (lengte / 2);
    RechtsOnder[1] = My + (lengte / 2);
}
/*
 * De coordinaten van het middelpunt zijn van de voorste vierkant
 */
void kubus(int Xmiddelpunt, int Ymiddelpunt, int lengte, byte color)
{
    // Voorste vierkant
    int V_LB[2];
    int V_RO[2];
    int A_LB[2];
    int A_RO[2];

    hoek(V_LB, V_RO, Xmiddelpunt, Ymiddelpunt, lengte);
    hoek(A_LB, A_RO, Xmiddelpunt + (lengte / 2), Ymiddelpunt - (lengte / 2), lengte);

    rechthoek(V_LB, V_RO, color);
    rechthoek(A_LB, A_RO, color);

    line(V_LB[0], V_LB[1], A_LB[0], A_LB[1], color); // Links bovens verbinden
    line(V_RO[0], V_RO[1], A_RO[0], A_RO[1], color); // Rechts onders verbinden
    line(V_LB[0], V_RO[1], A_LB[0], A_RO[1], color); // Links onders verbinden
    line(V_RO[0], V_LB[1], A_RO[0], A_LB[1], color); // Rechts bovens verbinden
}

void kubusTest(int Xmiddelpunt, int Ymiddelpunt, int lengte, int Xdraai, byte color)
{
    // Voorste vierkant
    int V_LB[2];
    int V_RO[2];
    int A_LB[2];
    int A_RO[2];

    hoek(V_LB, V_RO, Xmiddelpunt - Xdraai, Ymiddelpunt, lengte);
    hoek(A_LB, A_RO, Xmiddelpunt + Xdraai, Ymiddelpunt, lengte);

    rechthoek(V_LB, V_RO, color);
    rechthoek(A_LB, A_RO, color);

    line(V_LB[0], V_LB[1], A_LB[0], A_LB[1], color); // Links bovens verbinden
    line(V_RO[0], V_RO[1], A_RO[0], A_RO[1], color); // Rechts onders verbinden
    line(V_LB[0], V_RO[1], A_LB[0], A_RO[1], color); // Links onders verbinden
    line(V_RO[0], V_LB[1], A_RO[0], A_LB[1], color); // Rechts bovens verbinden
}

// Function to put pixels
// at subsequence points
void drawCircle(int xc, int yc, int x, int y, byte color)
{
    // Source: https://www.geeksforgeeks.org/bresenhams-circle-drawing-algorithm/
    SetPixel(xc + x, yc + y, color);
    SetPixel(xc - x, yc + y, color);
    SetPixel(xc + x, yc - y, color);
    SetPixel(xc - x, yc - y, color);
    SetPixel(xc + y, yc + x, color);
    SetPixel(xc - y, yc + x, color);
    SetPixel(xc + y, yc - x, color);
    SetPixel(xc - y, yc - x, color);
}

// Function for circle-generation
// using Bresenham's algorithm
void circleBres(int xc, int yc, int r, byte color)
{
    // Source: https://www.geeksforgeeks.org/bresenhams-circle-drawing-algorithm/
    int x = 0, y = r;
    int d = 3 - 2 * r;
    drawCircle(xc, yc, x, y, color);
    while (y >= x)
    {
        // for each pixel we will
        // draw all eight pixels

        x++;

        // check for decision parameter
        // and correspondingly
        // update d, x, y
        if (d > 0)
        {
            y--;
            d = d + 4 * (x - y) + 10;
        }
        else
            d = d + 4 * x + 6;
        drawCircle(xc, yc, x, y, color);
        //        delay(50);
    }
}

void FillScreen(uint8_t color)
{
    uint16_t xp, yp;

    for (yp = 0; yp < VGA_DISPLAY_Y; yp++)
    {
        for (xp = 0; xp < VGA_DISPLAY_X; xp++)
        {
            SetPixel(xp, yp, color);
        }
    }
}

void SetPixel(uint16_t xp, uint16_t yp, uint8_t color)
{
    if (xp >= VGA_DISPLAY_X)
        xp = 0;
    if (yp >= VGA_DISPLAY_Y)
        yp = 0;

    // Write pixel to ram
    VGA_RAM[(yp * (VGA_DISPLAY_X + 1)) + xp] = color;
}
