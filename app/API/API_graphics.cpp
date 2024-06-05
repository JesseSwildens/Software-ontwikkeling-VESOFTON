#include "API_graphics.h"

#include "API_fonts.h"
#include "API_gfx_text.h"

#include "API_fonts.h"
#include "API_gfx_text.h"

#if __cplusplus
extern "C"
{
#include "stm32_ub_vga_screen.h"
#include "stm32f4xx.h"
}
#endif

#include "stm32f4xx_conf.h"

#include <math.h>
#include <string>

#define BETWEEN(x, y, z) ((x < z) && (x > y))
#define OUTSIDE(x, y, z) ((x > z) || (x < y))

#ifndef __FILE_NAME__
#define __FILE_NAME__ "Testmessage"
#endif

#define log_message(message) (base_log_message(message, __LINE__, __FILE_NAME__))

#define TICK_PRIORITY 15

/**
 * @note Maximum color value supported
 */
#define MAX_COLOR_DEPTH 0xff // 8 bit for all colors

/**
 * @note Max radius the circle can be without getting gaps in circle
 */
#define MAX_RADIUS_SIZE 25
/**
 * @note Amount of points draw while drawing a circle
 */
#define MAX_CIRCLE_POINTS 100

static void (*logger_callback)(const char* message, int length) = nullptr;
static void base_log_message(std::string message, int line, std::string filename);
static void log_message_callback(std::string);
static void API_set_pixel(int, int, uint8_t);

API_gfx_text API_Text(VGA_DISPLAY_X, VGA_DISPLAY_Y, log_message_callback);
uint64_t Tick;

/**
 * @brief Drawing line
 *
 * @param x_12 X coordinates of the points
 * @param y_12 Y coordinates of the points
 * @param color Color of the line
 * @param weight Thickness of the corresponding line
 * @param reserved Currently unused. Reserved for future use.
 *
 * @return -1 if error occured, otherwise 0 (Also a call to Logger callback is sent if ptr isn't NULL)
 */
int API_draw_line(int x0, int y0, int x1, int y1, int color, int weight, int reserved)
{
    (void)reserved;

    if (OUTSIDE(x0, 0, VGA_DISPLAY_X) || OUTSIDE(x1, 0, VGA_DISPLAY_X)
        || OUTSIDE(y0, 0, VGA_DISPLAY_Y) || OUTSIDE(y1, 0, VGA_DISPLAY_Y))
    {
        log_message("error: Line outside range of display");
        return -1;
    }

    for (int i = 1; i < weight; i++)
    {
        API_draw_circle(x0, y0, i, color, 0);
    }

    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep)
    {
        int temp = x0;
        x0 = y0;
        y0 = x0;
        temp = x1;
        x1 = y1;
        y1 = temp;
    }

    if (x0 > x1)
    {
        int temp = x0;
        x0 = x1;
        x1 = temp;
        temp = y0;
        y0 = y1;
        y1 = temp;
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1)
    {
        ystep = 1;
    }
    else
    {
        ystep = -1;
    }

    for (; x0 <= x1; x0++)
    {
        if (steep)
        {
            API_draw_circle(y0, x0, weight, color, 1);
        }
        else
        {
            API_draw_circle(x0, y0, weight, color, 1);
        }
        err -= dy;
        if (err < 0)
        {
            y0 += ystep;
            err += dx;
        }
    }

    return 0;
}

/**
 * @brief Drawing circle
 *
 * @param x0_y0 position on screen
 * @param radius Radius of the circle
 * @param color Color of the circle
 * @param filled Fill in the circle
 *
 * @return -1 if error occured, otherwise 0
 */
int API_draw_circle(int x, int y, int radius, int color, int filled)
{
    if (OUTSIDE(x, 0, VGA_DISPLAY_X) || OUTSIDE(y, 0, VGA_DISPLAY_Y))
    {
        log_message("error: X or Y is outside the allowed range");
        return -1;
    }

    if (OUTSIDE(color, 0, MAX_COLOR_DEPTH))
    {
        log_message("warning: Color outside of allowed range. Color scaling isn't supported.");
    }

    if (OUTSIDE(radius, 0, MAX_RADIUS_SIZE))
    {
        log_message("error: Radius is outside of allowed range");
        return -1;
    }

    if (filled)
    {
        for (int y_temp = -radius; y_temp <= radius; y_temp++)
            for (int x_temp = -radius; x_temp <= radius; x_temp++)
                if (x_temp * x_temp + y_temp * y_temp <= radius * radius)
                    API_set_pixel(x + x_temp, y + y_temp, color);
        return 0;
    }

    int x_temp = radius;
    int y_temp = 0;
    int decision_over_2 = 1 - x_temp; // Decision criterion divided by 2 evaluated at x=r, y=0

    while (y_temp <= x_temp)
    {
        // Draw the 8 octants
        API_set_pixel(x + x_temp, y + y_temp, color);
        API_set_pixel(x + y_temp, y + x_temp, color);
        API_set_pixel(x - y_temp, y + x_temp, color);
        API_set_pixel(x - x_temp, y + y_temp, color);
        API_set_pixel(x - x_temp, y - y_temp, color);
        API_set_pixel(x - y_temp, y - x_temp, color);
        API_set_pixel(x + y_temp, y - x_temp, color);
        API_set_pixel(x + x_temp, y - y_temp, color);

        y_temp++;
        if (decision_over_2 <= 0)
        {
            decision_over_2 += 2 * y_temp + 1; // Change in decision criterion for y -> y+1
        }
        else
        {
            x_temp--;
            decision_over_2 += 2 * (y_temp - x_temp) + 1; // Change for y -> y+1, x -> x-1
        }
    }

    return 0; // Success
}

/**
 * @brief Helper function for calling the logger callback for in the API layer
 *
 * @param message std::string type with the corresponding message
 *
 * @return None
 */
static void base_log_message(std::string message, int line, std::string filename)
{
    if (logger_callback == nullptr)
        return;

    std::string out_string = "[" + filename + ":" + std::to_string(line) + "] " + message + "\n";

    (*logger_callback)(message.c_str(), message.length());
}

/**
 * @brief Register a callback that can be used to transfer messages from this layer to other layers. Otherwise logger functionality it is disregarded and not used.
 *
 * @param pFunction Function pointer with a const char* for the string and int length for the length of the string to prevent buffer overflows.
 *
 * @return Integer 0 if succesfull otherwise -1
 */
int API_register_logger_callback(void (*pFunction)(const char* string, int len))
{
    if (pFunction != nullptr)
    {
        logger_callback = pFunction;
        return 0;
    }

    return -1;
}

/**
 * @brief Draw pixel to screen.
 *
 * @param x X position on screen
 * @param y y position on screen
 * @param color color to set the pixel to
 *
 * @return None
 */
static void API_set_pixel(int x, int y, uint8_t color)
{
    if (OUTSIDE(x, 0, VGA_DISPLAY_X) || OUTSIDE(y, 0, VGA_DISPLAY_Y))
        return;
    VGA_RAM1[(y * (VGA_DISPLAY_X + 1)) + x] = color;
}

/**
 * @brief Drawing rectangle on the screen
 *
 * @param xy upper left corner of the rectangle
 * @param width width of the rectangle
 * @param height height of the rectangle
 * @param color Color of the rectangle
 * @param filled Fill in the rectangle
 * @param Weight Weight of the lines in the unfilled rectangle. Unused in filled
 * @param reserved Currently unused. Reserved for future use.
 *
 * @return 0 if succesfull, otherwise >= -1 if error occured
 */
int API_draw_rectangle(int x, int y, int width, int height, int color, int filled, int weight, int reserved)
{
    (void)reserved;
    if (OUTSIDE(x + width, 0, VGA_DISPLAY_X) || OUTSIDE(y + height, 0, VGA_DISPLAY_Y))
    {
        log_message("error: Outside the display area");
        return -1;
    }

    int ret = 0;
    if (filled)
    {
        for (int i = x; i < (x + width); i++)
        {
            for (int j = y; j < (y + height); j++)
            {
                API_set_pixel(i, j, color);
            }
        }
    }
    else
    {
        ret += API_draw_line(x, y, (x + width), y, color, weight, 0);
        ret += API_draw_line((x + width), (y + height), (x + width), y, color, weight, 0);
        ret += API_draw_line(x, (y + height), (x + width), (y + height), color, weight, 0);
        ret += API_draw_line(x, y, x, (y + height), color, weight, 0);
    }

    return ret;
}

/**
 * @brief clear screen to certain color
 *
 * @param color color that is used to set the background and overwrite all previous images on the screen
 */
int API_clearscreen(int color)
{
    if (color > MAX_COLOR_DEPTH)
    {
        log_message("warning: Color outside the range of the display");
    }

    for (int x = 0; x < VGA_DISPLAY_X; x++)
    {
        for (int y = 0; y < VGA_DISPLAY_Y; y++)
        {
            API_set_pixel(x, y, color);
        }
    }

    return 0;
}

/**
 * @brief Drawing text to the screen
 *
 * @param xy_lup upper left corner of the text
 * @param color color to draw the text in
 * @param text The text that needs to be drawn
 * @param fontname The selected font in which the text needs to be drawn
 * @param fontsize Size of the font
 * @param fontstyle Style (Italic, normal, etc..)
 * @param reserved Currently unused. Reserved for future use.
 *
 * @return 0 if succesfull, otherwise -1 if error occured
 */
int API_draw_text(int x_lup, int y_lup, int color, char* text, char* fontname, int fontsize, int fontstyle, int reserved)
{
    (void)reserved;

    if ((text == NULL) || (fontname == NULL) || (fontstyle < 0))
    {
        log_message("error: Text, fontname or fontstyle is not given");
        return -1;
    }

    if (OUTSIDE(x_lup, 0, VGA_DISPLAY_X) || (OUTSIDE(y_lup, 0, VGA_DISPLAY_Y)))
    {
        log_message("error: Outside the display area");
        return -1;
    }

    if (OUTSIDE(color, 0, MAX_COLOR_DEPTH))
    {
        log_message("warning: Color is outside the allowed color depth");
    }

    std::string cxx_fontname(fontname);

    for (char& character : cxx_fontname)
    {
        character = std::tolower(character);
    }

    if (API_Text.select_font(cxx_fontname, fontstyle))
    {
        log_message("error: Font " + cxx_fontname + " not found with style number " + std::to_string(fontstyle));
        return -1;
    }

    log_message("font = " + std::string((char*)API_Text.get_selected_font_name()));

    API_Text.set_font_size(fontsize);
    API_Text.set_text_color((uint8_t)color);

    API_Text.set_cursor(x_lup, y_lup);

    API_Text << text;

    return 0;
}

/**
 * @brief Helper function to forward a callback from API_Text layer
 */
static void log_message_callback(std::string msg)
{
    log_message(msg);
}

/**
 * @brief Initialize the system with Clocks and DMA for the Screen signal generation
 *
 * @param None
 *
 * @return None
 */
void API_Init(void)
{
    SystemInit(); // System speed to 168MHz

    UB_VGA_Screen_Init(); // Init VGA-Screen
}

/**
 * @brief Enabling systick for the timekeeping
 *
 * @param None
 *
 * @return None
 */
void API_InitTick(void)
{
    SysTick_Config(SystemCoreClock / 1000); // Clk frequency -> milliseconds

    NVIC_SetPriority(SysTick_IRQn, TICK_PRIORITY);

    Tick = 0;
}

/**
 * @brief Get the current Tick count
 *
 * @param None
 *
 * @return uint64_t current Tick
 */
uint64_t API_get_tick(void)
{
    return Tick;
}

/**
 * @brief Waiting to next command
 *
 * @param msecs milliseconds to wait
 *
 * @return 0 if wait was succesful, -1 if the timer had failed to count
 */
int API_wait(int msecs)
{
    if (msecs < 0)
    {
        log_message("warning: Invalid amount of time. We aren't time travelers!");
        return -1;
    }
    uint64_t startTick = API_get_tick();

    while ((API_get_tick() - startTick) < (unsigned int)msecs)
    {
    }

    return 0;
}