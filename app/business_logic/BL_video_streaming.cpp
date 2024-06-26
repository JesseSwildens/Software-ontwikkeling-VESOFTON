#include "BL_video_streaming.h"
#include "API_graphics.h"
#include "BL_callbacks.h"
#include "CHAL.h"
#include <bits/stdc++.h>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>

#define BAUDRATE_VIDEO 2000000
#define H_RES_VIDEO 200
#define V_RES_VIDEO 150

#define BETWEEN(x, y, z) ((x < z) && (x > y))
#define OUTSIDE(x, y, z) ((x > z) || (x < y))

int frame_pointer = 0;
int offset = 0;
int v_pos = 0;

int BL_hyperdrive()
{

    API_clearscreen(VGA_COL_BLACK);
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
                    API_set_pixel(pixel_count + 50, v_pos + 50, tempMainBuffer[cnt + 1 + frame_pointer]);
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
                API_set_pixel(pixel_count + 50, v_pos + 50, tempMainBuffer[i + frame_pointer + 1]);
                pixel_count++;
            }
            frame_pointer += len + 2;
        }
        else
        {
            log_message("video our of line");
        }
    }
    offset = frame_pointer;
}
