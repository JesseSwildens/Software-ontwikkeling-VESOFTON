#include "BL_video_streaming.h"
#include "API_simple_shapes.h"
#include "BL_callbacks.h"
#include "CHAL.h"
#include <bits/stdc++.h>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

#define BAUDRATE_VIDEO 912600
#define H_RES_VIDEO 64
#define V_RES_VIDEO 48

#define BETWEEN(x, y, z) ((x < z) && (x > y))
#define OUTSIDE(x, y, z) ((x > z) || (x < y))

static void BL_set_pixel(int x, int y, uint8_t color);

int frame_pointer = 0;
int offset = 0;
int v_pos = 0;

int BL_hyperdrive()
{

    CHAL_uart_config(BAUDRATE_VIDEO);

    RCC_ClocksTypeDef clocks;
    RCC_ClocksTypeDef* clocks_ptr = &clocks;
    RCC_GetClocksFreq(clocks_ptr);
    int BaudRate = clocks.PCLK1_Frequency / USART2->BRR;

    if ((BaudRate > BAUDRATE_VIDEO - 20000) || (BaudRate < BAUDRATE_VIDEO + 20000))
    {
        log_message("UART has changed baudrate");
        return 0;
    }
    else
    {
        log_message("UART has not changed baudrate");
        return -1;
    }

    return -1;
}

// void BL_video_stream(unsigned char* tempMainBuffer, int size)
// {
//     (void)size;
//     BL_set_pixel(240, 320, 0x49);
//     if ((tempMainBuffer[0] != 0) && (tempMainBuffer[0] != 1))
//     {
//         log_message("Error receiving data for video streaming");
//     }

//     int timeout = 0;
//     while ((frame_pointer < size) && (timeout < 100000))
//     {
//         if ((tempMainBuffer[frame_pointer] == 1))
//         {
//             int len = tempMainBuffer[frame_pointer + 2];
//             v_pos = tempMainBuffer[frame_pointer + 1];
//             if (v_pos == 47)
//                 log_message("working");
//             int pixel_count = 0;

//             for (int i = 3; i < len; i++)
//             {
//                 if ((i % 2) == 1)
//                 {
//                     int cnt = tempMainBuffer[i];
//                     for (int j = 0; j < cnt; j++)
//                     {
//                         BL_set_pixel(pixel_count, v_pos, tempMainBuffer[i + 1 + frame_pointer]);
//                         pixel_count++;
//                         if (pixel_count > 64)
//                             log_message("out of bounds");
//                     }
//                 }
//             }
//             frame_pointer += len + 3;
//         }
//         if ((tempMainBuffer[frame_pointer] == 0))
//         {
//             int len = H_RES_VIDEO;
//             v_pos = tempMainBuffer[frame_pointer + 1];
//             if ((v_pos == 0) && (frame_pointer > 0))
//             {
//                 frame_pointer = 0;
//                 memset(tempMainBuffer, 0, size);
//                 break;
//             }
//             frame_pointer += len + 2;
//         }
//         timeout++;
//     }
// }

void BL_video_stream(unsigned char* tempMainBuffer, int size)
{
    if ((tempMainBuffer[0] != 0) && (tempMainBuffer[0] != 1))
    {
        log_message("Error receiving data for video streaming");
    }
    while ((frame_pointer < size))
    {
        if ((tempMainBuffer[frame_pointer] == 1))
        {
            int cnt = 3;
            int len = tempMainBuffer[2 + frame_pointer];
            v_pos = tempMainBuffer[1 + frame_pointer];
            int count = 0;
            int pixel_count = 0;
            while (cnt < (len + 3))
            {
                count = tempMainBuffer[cnt + frame_pointer];
                for (int j = 0; j < count; j++)
                {
                    BL_set_pixel(pixel_count, v_pos, tempMainBuffer[cnt + 1 + frame_pointer]);
                    pixel_count++;
                }
                cnt += 2;
            }
            frame_pointer += len + 3; // for hdr and len
        }
        else if ((tempMainBuffer[frame_pointer] == 0))
        {
            int len = H_RES_VIDEO;
            v_pos = tempMainBuffer[1 + frame_pointer];
            if ((v_pos == 0) && (frame_pointer > 0))
            {
                frame_pointer = 0;
                break;
            }

            int pixel_count = 0;
            for (int i = 0; i < H_RES_VIDEO; i++)
            {
                BL_set_pixel(pixel_count, v_pos, tempMainBuffer[i + frame_pointer + 1]);
                pixel_count++;
            }
            frame_pointer += len + 2;
        }
        else
        {
            log_message("video our of line");
        }
    }
}

static void BL_set_pixel(int x, int y, uint8_t color)
{
    if (OUTSIDE(x, 0, VGA_DISPLAY_X) || OUTSIDE(y, 0, VGA_DISPLAY_Y))
        return;
    VGA_RAM1[(y * (VGA_DISPLAY_X + 1)) + x] = color;
}