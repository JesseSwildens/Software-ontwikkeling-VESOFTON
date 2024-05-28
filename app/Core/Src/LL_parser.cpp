#include "LL_parser.h"
#include "CHAL.h"
#include "stm32_ub_vga_screen.h"

#include <cstdio>
#include <iostream> // std::cin, std::
#include <math.h>
#include <queue>
#include <sstream>
#include <string>
#include <vector>

extern std::queue<std::string> incoming_commands_q;
float test = 0;

#define DEBUG_LL
#define PI 3.1415926
#define MAX_CIRCLE_POINTS 100

static void API_setPixel(int x, int y, uint8_t color)
{
    VGA_RAM1[(y * (VGA_DISPLAY_X + 1)) + x] = color;
}

int API_draw_circle(int x0, int y0, int radius, int color, int reserved)
{
    (void)reserved; // reserved parameter not used

    for (int i = 0; i < MAX_CIRCLE_POINTS; i++)
    {
        float angle = i * 2 * (PI / 100);
        API_setPixel(x0 + (cosf(angle) * radius), y0 + (sinf(angle) * radius), (uint8_t)color);
    }

    return 0; // Success
}

void function()
{
    while (!incoming_commands_q.empty())
    {
        std::string commandString = incoming_commands_q.front();

        // Find the position of the first comma (if any)
        size_t commaPos = commandString.find(',');

        // Extract the first word before the comma (or the entire string if no comma)
        std::string firstWord = commandString.substr(0, commaPos);

        // Trim leading and trailing whitespaces
        size_t start = firstWord.find_first_not_of(" \t");
        size_t end = firstWord.find_last_not_of(" \t");
        if (start != std::string::npos && end != std::string::npos)
        {
            firstWord = firstWord.substr(start, end - start + 1);
        }

#ifdef DEBUG_LL
        const char* back_command_cstr = firstWord.c_str();
        CHAL_UART2_SendString((char*)back_command_cstr);
#endif

        incoming_commands_q.pop();
    }
}

// int x = radius;
// int y = 0;
// int decision_over_2 = 1 - x; // Decision criterion divided by 2 evaluated at x=r, y=0

// while (y <= x)
// {
//     // Draw the 8 octants
//     API_setPixel(x0 + x, y0 + y, color);
//     API_setPixel(x0 + y, y0 + x, color);
//     API_setPixel(x0 - y, y0 + x, color);
//     API_setPixel(x0 - x, y0 + y, color);
//     API_setPixel(x0 - x, y0 - y, color);
//     API_setPixel(x0 - y, y0 - x, color);
//     API_setPixel(x0 + y, y0 - x, color);
//     API_setPixel(x0 + x, y0 - y, color);

//     y++;
//     if (decision_over_2 <= 0)
//     {
//         decision_over_2 += 2 * y + 1; // Change in decision criterion for y -> y+1
//     }
//     else
//     {
//         x--;
//         decision_over_2 += 2 * (y - x) + 1; // Change for y -> y+1, x -> x-1
//     }
// }